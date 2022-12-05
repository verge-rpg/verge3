vergeclass "Entity"
	function Entity:__init(...)
		local t = {...}
		
		self.entity_handle = -1
		if type(t[1]) == "number" and type(t[2]) == "number" and type(t[3]) == "string" then
			local x, y, filename = unpack(t)
			if FileExists(filename) then
				self.entity_handle = v3.EntitySpawn(x, y, filename)
			else
				error("Failed to spawn entity. The file '" .. filename .. "' couldn't be opened for reading.", 2)
			end
		elseif type(t[1]) == "number" then
			local ent = unpack(t)
			self.entity_handle = ent
		else
			error("Constructor Entity(" .. IndexedTypesToString(t) .. ") is not defined.", 2)
		end
	end
	
	Entity._property('x',
		function(self) return v3.get_entity_x(self.entity_handle) end,
		function(self, value) v3.set_entity_x(self.entity_handle, num(value)) end)
	Entity._property('y',
		function(self) return v3.get_entity_y(self.entity_handle) end,
		function(self, value) v3.set_entity_y(self.entity_handle, num(value)) end)
	Entity._property('special_frame',
		function(self) return v3.get_entity_specframe(self.entity_handle) end,
		function(self, value) v3.set_entity_specframe(self.entity_handle, num(value)) end)
	Entity._property('hotspot_x',
		function(self) return v3.get_entity_hotx(self.entity_handle) end)
	Entity._property('hotspot_y',
		function(self) return v3.get_entity_hoty(self.entity_handle) end)
	Entity._property('hotspot_width',
		function(self) return v3.get_entity_hotw(self.entity_handle) end)
	Entity._property('hotspot_height',
		function(self) return v3.get_entity_hoth(self.entity_handle) end)
	Entity._property('direction',
		function(self) return v3.get_entity_face(self.entity_handle) end,
		function(self, value) v3.set_entity_face(self.entity_handle, num(value)) end)
	Entity._property('speed',
		function(self) return v3.get_entity_speed(self.entity_handle) end,
		function(self, value) return v3.set_entity_speed(self.entity_handle, num(value)) end)
	Entity._property('lucent',
		function(self) return v3.get_entity_lucent(self.entity_handle) end,
		function(self, value) v3.set_entity_lucent(self.entity_handle, num(value)) end)
	Entity._property('frame_width',
		function(self) return v3.get_entity_framew(self.entity_handle) end)
	Entity._property('frame_height',
		function(self) return v3.get_entity_frameh(self.entity_handle) end)
	Entity._property('visible',
		function(self) return v3.get_entity_visible(self.entity_handle) end,
		function(self, value) v3.set_entity_visible(self.entity_handle, bool(value)) end)
	Entity._property('is_obstruction',
		function(self) return v3.get_entity_obstruct(self.entity_handle) end,
		function(self, value) v3.set_entity_obstruct(self.entity_handle, bool(value)) end)
	Entity._property('obstructable',
		function(self) return v3.get_entity_obstructable(self.entity_handle) end,
		function(self, value) v3.set_entity_obstructable(self.entity_handle, bool(value)) end)
	Entity._property('filename',
		function(self) return v3.get_entity_chr(self.entity_handle) end,
		function(self, value) v3.set_entity_chr(self.entity_handle, tostring(value)) end)
	Entity._property('description',
		function(self) return v3.get_entity_description(self.entity_handle) end,
		function(self, value) v3.set_entity_description(self.entity_handle, tostring(value)) end)

	function Entity.Count()
		return v3.entities
	end
	
	function Entity.PauseAll()
		SetEntitiesPaused(1)
	end
	
	function Entity.UnpauseAll()
		SetEntitiesPaused(0)
	end

	function Entity:SetSprite(filename)
		if FileExists(filename) then
			v3.ChangeCHR(self.entity_handle, filename)
		else
			error("Failed to change entity's sprite. The file '" .. filename .. "' couldn't be opened for reading.", 2)
		end
	end
	
	function Entity:Move(movescript)
		v3.EntityMove(self.entity_handle, movescript)
	end
	
	function Entity:SetWanderDelay(delay)
		v3.EntitySetWanderDelay(self.entity_handle, delay)
	end
	
	function Entity:WanderRect(x, y, x2, y2)
		v3.EntitySetWanderRect(self.entity_handle, x, y, x2, y2)
	end
	
	function Entity:WanderZone()
		v3.EntitySetWanderZone(self.entity_handle)
	end
	
	function Entity:HookRender(func)
		v3.HookEntityRender(self.entity_handle, func)
	end
	
	function Entity:Stalk(target_ent)
		v3.EntityStalk(self.entity_handle, target_ent)
	end
	
	function Entity:StopStalking()
		v3.EntityStalk(self.entity_handle, -1)
	end
	
	function Entity:Stop()
		v3.EntityStop(self.entity_handle)
	end
	
	function Entity:BecomePlayer()
		v3.SetPlayer(self.entity_handle)
	end
	
	function Entity:__tostring()
		return "entity " .. ObjectAttributesToString(self)
	end
