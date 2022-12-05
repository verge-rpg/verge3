vergeclass "_vergeMap"
	local _tileset = nil	
	-- The song constructor allows the objects to be created without overlap.
	function _vergeMap:__init()
		self._attr = { tileset = 0, width = 0, height = 0, title = "", filename = "", start_x = 0, start_y = 0, music_filename = "", music = 0 }
		
		-- Metatable the layer stuff so we can have a magically sized array
		local meta = {}
		meta.__index =	function(self, idx)
					return vx._vergeMapLayer(num(idx))
				end
		meta.__newindex =	function(self, idx, value)
						error("The map's layer objects cannot be replaced.", 2)
					end
		self.layer = {}
		setmetatable(self.layer, meta)
		
		-- Metatable the zone stuff so we can have a magically sized array
		local meta = {}
		meta.__index =	function(self, idx)
					return vx._vergeMapZone(num(idx))
				end
		meta.__newindex =	function(self, idx, value)
						error("The map's zone objects cannot be replaced.", 2)
					end
		self.zone = {}
		setmetatable(self.zone, meta)
	end
	
	_vergeMap._property('tileset',
			function(self)
				-- Once an image handle for the tileset exists, verge just keeps reusing it.
				if _tileset then
					return _tileset
				-- After a map is loaded, the handle can be attached to an image
				elseif v3.curmap.tileset ~= 0 then
					_tileset = vx.Image(v3.curmap.tileset)
					return _tileset
				-- Map not loaded, can't return a tileset
				else
					return nil
				end
			end
		)
	_vergeMap._property('width', function(self) return v3.curmap.w end)
	_vergeMap._property('height', function(self) return v3.curmap.h end)
	_vergeMap._property('title', function(self) return v3.curmap.name end)
	_vergeMap._property('filename', function(self) return v3.curmap.path end, function(self, value) self:Switch(tostring(value)) end)
	_vergeMap._property('start_x', function(self) return v3.curmap.startx end)
	_vergeMap._property('start_y', function(self) return v3.curmap.starty end)
	_vergeMap._property('render_string', function(self) return v3.curmap.rstring end)
	_vergeMap._property('music_filename', function(self) return v3.curmap.music end)

	-- Switches the map.
	-- Unlike VC, Map will not break from loops automatically.
	-- It doesn't change maps until all code blocks have exited.
	function _vergeMap:Switch(filename)
		v3.Map(filename)
	end
	
	function _vergeMap:Render(x, y, dest)
		x = x or v3.xwin
		y = y or v3.ywin
		v3.RenderMap(x, y, vx.Image.GDH(dest))
	end
	
	function _vergeMap:GetTile(x, y, layer)
		return v3.GetTile(x, y, layer)
	end
	
	function _vergeMap:SetTile(x, y, layer, tile)
		v3.SetTile(x, y, layer, tile)
	end
	
	function _vergeMap:GetObs(x, y)
		return v3.GetObs(x, y)
	end
	
	function _vergeMap:GetObsPixel(x, y)
		return v3.GetObsPixel(x, y)
	end
	
	function _vergeMap:SetObs(x, y, tile)
		v3.SetObs(x, y, tile)
	end
	
	function _vergeMap:GetZone(x, y)
		return v3.GetZone(x, y)
	end
	
	function _vergeMap:SetZone(x, y, tile)
		v3.SetZone(x, y, tile)
	end
	
	function _vergeMap:HookRetrace(name)
		v3.HookRetrace(name)
	end

	function _vergeMap:__tostring()
		return "verge map " .. ObjectAttributesToString(self)
	end
	
vergeclass "_vergeMapLayer"
	function _vergeMapLayer:__init(layer_index)	
		self.layer_index = layer_index
	end
	
	_vergeMapLayer._property('lucent',
		function(self) return v3.get_layer_lucent(self.layer_index) end,
		function(self, value) v3.set_layer_lucent(self.layer_index, num(value)) end)
	_vergeMapLayer._property('parallax_x',
		function(self) return v3.get_layer_parallaxx(self.layer_index) end,
		function(self, value) v3.set_layer_parallaxx(self.layer_index, num(value)) end)
	_vergeMapLayer._property('parallax_y',
		function(self) return v3.get_layer_parallaxy(self.layer_index) end,
		function(self, value) v3.set_layer_parallaxy(self.layer_index, num(value)) end)
	_vergeMapLayer._property('width', function(self) return v3.get_layer_w(self.layer_index) end)
	_vergeMapLayer._property('height', function(self) return v3.get_layer_h(self.layer_index) end)
	
	
	function _vergeMapLayer:__tostring()
		return "verge map layer " .. ObjectAttributesToString(self)
	end
	
vergeclass "_vergeMapZone"
	function _vergeMapZone:__init(zone_index)
		self.zone_index = zone_index
	end
	
	_vergeMapZone._property('name',
		function(self) return v3.get_zone_name(self.zone_index) end,
		function(self, value) v3.set_zone_name(self.zone_index, num(value)) end)
	_vergeMapZone._property('event',
		function(self) return v3.get_zone_event(self.zone_index) end,
		function(self, value) v3.set_zone_event(self.zone_index, num(value)) end)
	
	function _vergeMapZone:__tostring()
		return "verge map zone " .. ObjectAttributesToString(self)
	end
