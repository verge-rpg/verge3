vergeclass "Font"
	function Font:__init(...)
		local t = {...}
		local possible_failure = ""
		
		self.font_handle = 0
		if type(t[1]) == "number" then
			local handle = unpack(t)
			if handle == 0 then
				self.font_handle = handle
			else
				error("Failed to load font. The only number index that can be passed is 0, the default Verge font. Already loaded fonts can't be copied in. Load using a string instead.", 2)
			end
		elseif type(t[1]) == "string" then
			local filename, width, height = unpack(t)			
			if FileExists(filename) then
				self.font_handle = v3.LoadFont(filename)
			end
			if self.font_handle == 0 then
				error("Failed to load font. The file '" .. filename .. "' couldn't be opened for reading.", 2)
			end
		else
			error("Constructor Font(" .. IndexedTypesToString(t) .. ") is not defined.", 2)
		end
		
		-- Due to some special reserved handles we need to be careful.
		if self.font_handle ~= 0 then
			-- Takes care of garbage collection of the handle
			self.magic_handle = v3.GCHandle(self.font_handle, "FreeFont")
		end
		
		self.height = v3.FontHeight(self.font_handle)
	end
	
	function Font:Free()
		self.magic_handle = nil
	end
	
	function Font:EnableVariableWidth()
		v3.EnableVariableWidth(self.font_handle)
	end
	
	function Font:SetCharacterWidth(character, width)
		v3.SetCharacterWidth(self.font_handle, character, width)
	end
	
	function Font:TextWidth(text)
		return v3.TextWidth(self.font_handle, text)
	end
	
	function Font:Print(x, y, text, dest)	
		v3.PrintString(x, y, vx.Image.GDH(dest), self.font_handle, text)
	end
	
	function Font:PrintRight(x, y, text, dest)
		v3.PrintRight(x, y, vx.Image.GDH(dest), self.font_handle, text)
	end
	
	function Font:PrintCenter(x, y, text, dest)	
		v3.PrintCenter(x, y, vx.Image.GDH(dest), self.font_handle, text)
	end
	
	function Font:WrapText(text, width)
		return v3.WrapText(self.font_handle, text, width)
	end
	
	function Font:__tostring()
		return "font " .. ObjectAttributesToString(self)
	end
