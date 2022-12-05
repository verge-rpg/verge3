vergeclass "_vergeMouseInput"
	function _vergeMouseInput:__init(...)
		self.left = vx._vergeMouseButtonInput("l")
		self.right = vx._vergeMouseButtonInput("r")
		self.middle = vx._vergeMouseButtonInput("m")
	end
	
	_vergeMouseInput._property('x', function(self) return v3.mouse.x end)
	_vergeMouseInput._property('y', function(self) return v3.mouse.y end)
	_vergeMouseInput._property('wheel', function(self) return v3.mouse.w end)
	
	function _vergeMouseInput:__tostring()
		return "verge mouse input " .. ObjectAttributesToString(self)
	end

vergeclass "_vergeMouseButtonInput"
	function _vergeMouseButtonInput:__init(button)
		self.button = button
	end
	
	_vergeMouseButtonInput._property('pressed',
		function(self) return v3.mouse[self.button] end,
		function(self, value) v3.mouse[self.button] = bool(value) end)
	
	function _vergeMouseButtonInput:Unpress()
		self.pressed = false
	end
	
	function _vergeMouseButtonInput:__tostring()
		return "verge mouse button " .. ObjectAttributesToString(self)
	end
