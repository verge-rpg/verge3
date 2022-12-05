vergeclass "_vergeMusic"
	function _vergeMusic:__init()
	end
	
	function _vergeMusic:Play(filename)
		v3.PlayMusic(filename)
	end
	
	function _vergeMusic:Stop()
		v3.StopMusic()
	end
	
	function _vergeMusic:SetVolume(value)
		v3.SetMusicVolume(value)
	end
