vergeclass "Socket"
	local DEFAULT_SOCKET_PORT = 45150;
	
	function Socket:__init(...)
		local t = {...}
		
		self.socket_handle = 0
		
		if t[1] == nil then
			-- Do nothing
		elseif type(t[1]) == "string" then
			self:Connect(t[1], t[2])
		else
			error("Constructor Socket(" .. IndexedTypesToString(t) .. ") is not defined.", 2)
		end
		-- Takes care of garbage collection of the handle
		self.magic_handle = v3.GCHandle(self.socket_handle, "SocketClose")
	end

	function Socket:Close()	
		self.magic_handle = nil
	end
	Socket.Free = Socket.Close
	
	function Socket:Connect(address, port)
	
		v3.SetConnectionPort(port or DEFAULT_SOCKET_PORT)
		self.socket_handle = v3.Connect(address)
		v3.SetConnectionPort(DEFAULT_SOCKET_PORT)

		if self.socket_handle == 0 then
			error("Failed to connect to '" .. address .. "' on port " .. port .. ".", 2)
		end
		
		-- Takes care of garbage collection of the handle
		self.magic_handle = vx.MagicHandle(self.socket_handle, "socket", v3.SocketClose)
	end
	
	function Socket:GetConnection(port)
		v3.SetConnectionPort(port or DEFAULT_SOCKET_PORT)
		self.socket_handle = v3.GetConnection()
		v3.SetConnectionPort(DEFAULT_SOCKET_PORT)
		
		-- Takes care of garbage collection of the handle
		self.magic_handle = vx.MagicHandle(self.socket_handle, "socket", v3.SocketClose)
	end
	Socket.Listen = Socket.GetConnection

	function Socket:Connected()
		return v3.SocketConnected(self.socket_handle)
	end
	
	function Socket:HasData()
		return v3.SocketHasData(self.socket_handle)
	end
	
	-- This does not work. And is bad. And not needed. Why keep it?
	function Socket:ByteCount()
		return v3.SocketByteCount(self.socket_handle)
	end
	
	function Socket:GetInt()
		return v3.SocketGetInt(self.socket_handle)
	end
	
	function Socket:SendInt(value)
		v3.SocketSendInt(self.socket_handle, value)
	end
	
	function Socket:GetString()
		return v3.SocketGetString(self.socket_handle)
	end
	
	function Socket:SendString(value)
		v3.SocketSendString(self.socket_handle, value)
	end
	
	function Socket:GetRaw(length)
		return v3.SocketGetRaw(self.socket_handle, length)
	end
	
	function Socket:SendRaw(value)
		v3.SocketSendRaw(self.socket_handle, value)
	end
	
	-- verge's file support is not included in vx, so there's little point in supporting verge's get/send file functions
	--[[
	function Socket:GetFile()
	end
	function Socket:SendFile()
	end
	--]]
	
	-- floats? wtf? it's in vc_builtins.cpp though. I want answers
	--[[
	function Socket:GetFloat()
	end
	function Socket:SendFloat()
	end
	--]]
	
	function Socket:__tostring()
		return "socket " .. ObjectAttributesToString(self)
	end
