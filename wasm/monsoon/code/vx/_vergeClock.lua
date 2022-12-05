
vergeclass "_vergeClock"
	function _vergeClock:__init()
	end
	
	_vergeClock._property('timer', function(self) return v3.timer end, function(self, value) v3.timer = num(value) end)
	_vergeClock._property('systemtime', function(self) return v3.systemtime end)
	
	function _vergeClock:Hook(name)
		v3.HookTimer(name)
	end
