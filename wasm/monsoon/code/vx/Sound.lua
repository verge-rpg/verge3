vergeclass "Sound"
	-- Use this when referring to the classese themselves in methods.
	-- This is important, since the global reference 'Sound' is wiped when exported.
	local _sound_class = Sound
	
	function Sound:__init(...)
		local t = {...}
		
		if type(t[1]) == "string" then
			local filename = unpack(t)
			if FileExists(filename) then
				self.sound_handle = v3.LoadSound(filename)
			else
				self.sound_handle = 0
			end
		else
			error("Constructor Sound(" .. IndexedTypesToString(t) .. ") is not defined.", 2)
		end
		
		-- Takes care of garbage collection of the handle
		self.magic_handle = v3.GCHandle(self.sound_handle, "FreeSound")
	end
	
	
	function Sound:Free()
		self.magic_handle = nil
	end
	
	function Sound:Play(volume)
		volume = volume or 100
		return vx.SoundChannel(v3.PlaySound(self.sound_handle, volume))
	end
	
	function Sound:__tostring()
		return "sound " .. ObjectAttributesToString(self)
	end
	
vergeclass "SoundChannel"
	function SoundChannel:__init(...)
		local t = {...}
		
		if type(t[1]) == "number" then
			self.channel = t[1]
		else
			error("Constructor SoundChannel(" .. IndexedTypesToString(t) .. ") is not defined.", 2)
		end
		
		-- THIS CLASS SHOULD NEVER HAVE A MAGIC HANDLE, FMOD ALREADY TAKES CARE OF CHANNEL STUFF INTERNALLY
	end
	
	function SoundChannel:Stop()
		self.magic_handle = nil
	end
	
	function SoundChannel:IsPlaying()
		return v3.SoundIsPlaying(self.channel)
	end
	
	function SoundChannel:__tostring()
		return "sound channel " .. ObjectAttributesToString(self)
	end
