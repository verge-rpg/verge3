vergeclass "_vergeJoystickInput"
	function _vergeJoystickInput:__init()
		self.up = vx._vergeJoystickDirectionInput("up")
		self.down = vx._vergeJoystickDirectionInput("down")
		self.left = vx._vergeJoystickDirectionInput("left")
		self.right = vx._vergeJoystickDirectionInput("right")
		
		self.button = {}
		for i = 1, 32 do
			table.insert(self.button, vx._vergeJoystickButtonInput(i - 1))
		end
	end
	
	_vergeJoystickInput._property('joy_index', function(self) return v3.get_joystick() end, function(self, value) v3.set_joystick(num(value)) end)
	_vergeJoystickInput._property('active', function(self) return v3.get_joy_active() end)
	_vergeJoystickInput._property('analog_x', function(self) return v3.get_joy_analogx() end)
	_vergeJoystickInput._property('analog_y', function(self) return v3.get_joy_analogy () end)
	
	function _vergeKeyboardInput:__tostring()
		return "verge joystick input " .. ObjectAttributesToString(self)
	end	

vergeclass "_vergeJoystickDirectionInput"
	function _vergeJoystickDirectionInput:__init(direction)
		self.direction = direction
	end
	
	_vergeJoystickDirectionInput._property('pressed',
		function(self) return v3.joy[self.direction] end,
		function(self, value) v3.joy[self.direction] = bool(value) end)
	
	function _vergeJoystickDirectionInput:Unpress()
		self.pressed = false
	end
	
	function _vergeJoystickDirectionInput:__tostring()
		return "verge joystick button " .. ObjectAttributesToString(self)
	end

vergeclass "_vergeJoystickButtonInput"
	function _vergeJoystickButtonInput:__init(button)
		self.button = button
	end
	
	_vergeJoystickButtonInput._property('pressed',
		function(self) return v3.get_joy_button(self.button) end,
		function(self, value) v3.set_joy_button(self.button, bool(value)) end)
	
	function _vergeJoystickButtonInput:Unpress()
		self.pressed = false
	end
	
	function _vergeJoystickButtonInput:__tostring()
		return "verge joystick button " .. ObjectAttributesToString(self)
	end
