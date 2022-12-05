-- In order to get custom __index methods, we can't use LuaBind's class thing :(
vergeclass "_vergeCamera"
	function _vergeCamera:__init()
	end
	
	_vergeCamera.TARGET_NOTHING = 0
	_vergeCamera.TARGET_PLAYER = 1
	_vergeCamera.TARGET_ENTITY = 2
	
	_vergeCamera._property('x', function(self) return v3.xwin end, function(self, value) v3.xwin = math.floor(num(value)) end)
	_vergeCamera._property('y', function(self) return v3.ywin end, function(self, value) v3.ywin = math.floor(num(value)) end)
	_vergeCamera._property('target',
			function(self) return v3.cameratracking end,
			function(self, value) v3.cameratracking = num(value) end)
			
	-- Entity name collision forces me to call an Entity a "Critter" atm.
	_vergeCamera._property('entity',
			function(self) return vx.Entity(v3.cameratracker) end,
			function(self, value)
				if type(value) == "table" or type(value) == "userdata" then
					v3.cameratracker = value.entity_handle
				else
					v3.cameratracker = num(value)
				end
			end)
	
	function _vergeCamera:TargetNothing()
		self.target = self.TARGET_NOTHING
	end
	
	function _vergeCamera:TargetPlayer()
		self.target = self.TARGET_PLAYER
	end
	
	function _vergeCamera:TargetEntity(ent)
		self.target = self.TARGET_ENTITY
		self.entity = ent
	end
	
