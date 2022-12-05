vergeclass "Image"
	function Image:__init(...)
		local t = {...}
		local possible_failure = ""
		
		self.image_handle = 0
		if tonumber(t[1]) and not tonumber(t[2]) then
			local handle = unpack(t)
			self.image_handle = handle
			possible_failure = " The number passed doesn't seem to be a valid handle. It could have been freed already or is a null image handle."
		elseif type(t[1]) == "string" then
			local filename = unpack(t)
			if FileExists(filename) then
				self.image_handle = v3.LoadImage(filename)
			else
				possible_failure = " The file '" .. filename .. "' couldn't be opened for reading."
			end
		elseif tonumber(t[1]) and tonumber(t[2]) then
			local width, height = unpack(t)
			self.image_handle = v3.NewImage(width, height)
		elseif type(t[1]) == "userdata" or type(t[1]) == "table" then
			local img = unpack(t)
			if img.image_handle then
				if img.image_handle ~= 0 and v3.ImageValid(img.image_handle) then
					self.image_handle = v3.DuplicateImage(img.image_handle)
				else
					possible_failure = " The Image passed doesn't have a valid image handle. It could have been freed already or is a null image handle."
				end
			else
				possible_failure = " The userdata passed does not seem to be an Image, because its image_handle attribute is nil."
			end
		else
			error("Constructor Image(" .. IndexedTypesToString(t) .. ") is not defined.", 2)
		end
		
		if self.image_handle == 0 or not v3.ImageValid(self.image_handle) then
			error("Failed to load image." .. possible_failure, 2)
		end
		
		-- Due to some special reserved handles we need to be careful.
		if self.image_handle ~= v3.screen and self.image_handle ~= v3.curmap.tileset then
			-- Takes care of garbage collection of the handle
			self.magic_handle = v3.GCHandle(self.image_handle, "FreeImage")
		end
	end
	
	Image._property('width', function(self) return v3.ImageWidth(self.image_handle) end)
	Image._property('height', function(self) return v3.ImageHeight(self.image_handle) end)
	
	function Image:Free()
		if self.image_handle ~= 0 and v3.ImageValid(self.image_handle) then
			self.magic_handle = nil
		end
	end

	function Image:Rect(x, y, x2, y2, color)
		v3.Rect(x, y, x2, y2, color, self.image_handle)
	end
	
	function Image:RectFill(x, y, x2, y2, color)
		v3.RectFill(x, y, x2, y2, color, self.image_handle)
	end
	
	function Image:Fill(color)
		v3.RectFill(0, 0, self.width-1, self.height-1, color, self.image_handle)
	end
	
	function Image:Circle(x, y, xrad, yrad, color)
		v3.Circle(x, y, xrad, yrad, color, self.image_handle)
	end
	
	function Image:CircleFill(x, y, xrad, yrad, color)
		v3.CircleFill(x, y, xrad, yrad, color, self.image_handle)
	end
	
	function Image:Triangle(x, y, x2, y2, x3, y3, color)
		v3.Triangle(x, y, x2, y2, x3, y3, color, self.image_handle)
	end
	
	function Image:Line(x, y, x2, y2, color)
		v3.Line(x, y, x2, y2, color, self.image_handle)
	end
	
	function Image:RectHGrad(x, y, x2, y2, color, color2)
		v3.RectHGrad(x, y, x2, y2, color, color2, self.image_handle)
	end
	
	function Image:RectVGrad(x, y, x2, y2, color, color2)
		v3.RectVGrad(x, y, x2, y2, color, color2, self.image_handle)
	end
	
	function Image:RectRGrad(x, y, x2, y2, color, color2)
		v3.RectRGrad(x, y, x2, y2, color, color2, self.image_handle)
	end
	
	function Image:Rect4Grad(x, y, x2, y2, color, color2, color3, color4)
		v3.RectVGrad(x, y, x2, y2, color, color2, color3, color4, self.image_handle)
	end
	
	function Image:GetPixel(x, y)
		return v3.GetPixel(x, y, self.image_handle)
	end
	
	function Image:SetPixel(x, y, color)
		return v3.SetPixel(x, y, color, self.image_handle)
	end
	
	function Image:Mosaic(xgran, ygran)
		v3.Mosaic(xgran, ygran, self.image_handle)
	end
	
	function Image:DefinedColorFilter(filter)
		v3.ColorFilter(filter, self.image_handle)
	end
	
	function Image:Invert()
		self:DefinedColorFilter(vx.FilterEffect.Invert)
	end
	
	function Image:Greyscale()
		self:DefinedColorFilter(vx.FilterEffect.Greyscale)
	end
	
	function Image:GreyscaleInvert()
		self:DefinedColorFilter(vx.FilterEffect.GreyscaleInvert)
	end
	
	function Image:Red()
		self:DefinedColorFilter(vx.FilterEffect.Red)
	end
	
	function Image:Green()
		self:DefinedColorFilter(vx.FilterEffect.Green)
	end
	
	function Image:Blue()
		self:DefinedColorFilter(vx.FilterEffect.Blue)
	end
	
	function Image:ColorFilter(color1, color2)
		v3.SetCustomColorFilter(num(color1), num(color2))
		self:DefinedColorFilter(vx.FilterEffect.Custom)
		vx.RestoreCustomColorFilter();
	end
	
	function Image:HueReplace(oldhue, tolerance, newhue)
		v3.HueReplace(oldhue, tolerance, newhue, self.image_handle)
	end
	
	function Image:ColorReplace(oldcolor, newcolor)
		v3.ColorReplace(oldcolor, newcolor, self.image_handle)
	end
	
	function Image:BlitTile(x, y, tile)
		v3.TBlitTile(x, y, tile, self.image_handle)
	end
	
	function Image.GDH(dest) -- Get Destination Handle
		dest = dest or v3.screen
		return vx.Image.GIH(dest)
	end
	
	function Image.GIH(image) -- Get Image Handle
		if tonumber(image) then
			return image
		else
			return image.image_handle
		end
	end
	
	function Image:Blit(x, y, dest)		
		v3.TBlit(x, y, self.image_handle, vx.Image.GDH(dest))
	end
	
	function Image:FullBlit(x, y, dest)
		v3.Blit(x, y, self.image_handle, vx.Image.GDH(dest))
	end
	
	function Image:AdditiveBlit(x, y, dest)
		v3.TAdditiveBlit(x, y, self.image_handle, vx.Image.GDH(dest))
	end
	
	function Image:SubtractiveBlit(x, y, dest)
		v3.TSubtractiveBlit(x, y, self.image_handle, vx.Image.GDH(dest))
	end
	
	function Image:GrabRegion(x, y, x2, y2, dx, dy, dest)		
		v3.TGrabRegion(x, y, x2, y2, dx, dy, self.image_handle, vx.Image.GDH(dest))
	end
	
	function Image:FullGrabRegion(x, y, x2, y2, dx, dy, dest)
		v3.GrabRegion(x, y, x2, y2, dx, dy, self.image_handle, vx.Image.GDH(dest))
	end
	
	function Image:AlphaBlit(x, y, alpha_map, dest)		
		v3.AlphaBlit(x, y, self.image_handle, vx.Image.GIH(alpha_map), vx.Image.GDH(dest))
	end
	
	function Image:ScaleBlit(x, y, dw, dh, dest)		
		v3.TScaleBlit(x, y, dw, dh, self.image_handle, vx.Image.GDH(dest))
	end
	
	function Image:RotScale(x, y, angle, scalefactor, dest)		
		v3.RotScale(x, y, angle, (1 / math.max(scalefactor, 0.00000001)) * 1000, self.image_handle, vx.Image.GDH(dest))
	end
	Image.RotateScaleBlit = Image.RotScale
	
	function Image:RotateBlit(x, y, angle, dest)
		v3.RotScale(x, y, angle, 1000, self.image_handle, vx.Image.GDH(dest))
	end
	
	function Image:Silhouette(x, y, color, dest)		
		v3.Silhouette(x, y, color, self.image_handle, vx.Image.GDH(dest))
	end
	
	function Image:WrapBlit(x, y, dest)
		v3.TWrapBlit(x, y, self.image_handle, vx.Image.GDH(dest))
	end
	
	function Image:SetClip(x, y, x2, y2)
		v3.SetClip(x, y, x2, y2, self.image_handle)
	end
	
	function Image:ImageShell(x, y, w, h)
		print "imageshell"
		return vx.Image(v3.ImageShell(x, y, w, h, self.image_handle))
	end
	
	function Image:CopyToClipboard()
		v3.CopyImageToClipboard(self.image_handle);
	end
	
	function Image:__tostring()
		return "image " .. ObjectAttributesToString(self)
	end
