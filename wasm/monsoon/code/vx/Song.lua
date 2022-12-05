vergeclass "Song"
	function Song:__init(...)
		local t = {...}
		
		if tostring(t[1]) then
			local filename = unpack(t)
			self.song_handle = v3.LoadSong(filename)
		else
			error("Constructor Song(" .. IndexedTypesToString(t) .. ") is not defined.", 2)
		end
		
		-- Takes care of garbage collection of the handle
		self.magic_handle = v3.GCHandle(self.song_handle, "FreeSong")
	end
	
	Song._property('volume',
		function(self) return v3.GetSongVolume(self.song_handle) end,
		function(self, value) v3.SetSongVolume(self.song_handle, num(value)) end)
	Song._property('position',
		function(self) return v3.GetSongPos(self.song_handle) end,
		function(self, value) v3.SetSongPos(self.song_handle, num(value)) end)
	
	function Song:Free()
		self.magic_handle = nil
	end
	
	function Song:Play(volume)
		v3.PlaySong(self.song_handle)
		self.volume = volume or 100
	end
	
	function Song:Stop()
		v3.StopSong(self.song_handle)
	end
	
	function Song:Pause()
		v3.SetSongPaused(self.song_handle, 1)
	end
	
	function Song:Resume()
		v3.SetSongPaused(self.song_handle, 0)
	end
	
	function Song:__tostring()
		return "song " .. ObjectAttributesToString(self)
	end
