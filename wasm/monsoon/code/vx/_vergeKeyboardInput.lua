vergeclass "_vergeKeyboardInput"
	function _vergeKeyboardInput:__init(scancode)
		self.scancode = scancode
	end
	
	_vergeKeyboardInput._property('pressed',
		function(self) return v3.get_key(self.scancode) end,
		function(self, value) v3.set_key(self.scancode, bool(value)) end)
	
	function _vergeKeyboardInput:Unpress()
		self.pressed = false
	end
	
	function _vergeKeyboardInput:Hook(func)
		v3.HookKey(self.scancode, func)
	end
	
	function _vergeKeyboardInput:__tostring()
		return "verge keyboard input " .. ObjectAttributesToString(self)
	end
