-- File modes
vx.FileMode = {
	Read = v3.FILE_READ,
	Write = v3.FILE_WRITE,
	Append = v3.FILE_WRITE_APPEND,
	WriteAppend = v3.FILE_WRITE_APPEND
}

-- File seeking
vx.SeekMode = {
	Set = v3.SEEK_SET,
	Current = v3.SEEK_CUR,
	End = v3.SEEK_END
}

vergeclass 'File' do
	function File:__init(...)
		t = { ... }
		if tostring(t[1]) and tonumber(t[2]) then
			filename, mode = unpack(t)
			self.filename = filename
			self.mode = mode
			self.file_handle = v3.FileOpen(filename, mode)
		else
			error('Constructor File(' .. IndexedTypesToString(t) .. ') is not defined.', 2)
		end
	end
	
	File._property('opened', function(self) return self.file_handle ~= 0 end)
	File._property('eof', function(self) return self.opened and v3.FileEOF(self.file_handle) end)
	
	function File:Close()
		if self.opened then
			v3.FileClose(self.file_handle)
			self.file_handle = 0
		end
	end
	
	function File:ReadByte()
		return self.opened and v3.FileReadByte(self.file_handle)
	end
	
	function File:ReadLine()
		return self.opened and v3.FileReadLn(self.file_handle)
	end

	function File:ReadWord()
		return self.opened and v3.FileReadWord(self.file_handle)
	end
	
	function File:ReadQuad()
		return self.opened and v3.FileReadQuad(self.file_handle)
	end
	
	function File:ReadToken()
		return self.opened and v3.FileReadToken(self.file_handle)
	end
	
	function File:ReadString()
		return self.opened and v3.FileReadString(self.file_handle)
	end
	
	function File:SeekLine(line_number)
		if self.opened then
			v3.SeekLine(self.file_handle, line_number)
		end
	end

	function File:GetPosition()
		if self.opened then
			v3.FileCurrentPos(self.file_handle)
		end
	end
	
	function File:Seek(position, seek_mode)
		if self.opened then
			v3.SeekPos(self.file_handle, position, seek_mode)
		end
	end
	
	function File:Write(s)
		if self.opened then
			v3.FileWrite(self.file_handle, s)
		end
	end
	
	function File:WriteByte(n)
		if self.opened then
			v3.FileWriteByte(self.file_handle, n)
		end
	end
	
	function File:WriteWord(n)
		if self.opened then
			v3.FileWriteWord(self.file_handle, n)
		end
	end
	
	function File:WriteQuad(n)
		if self.opened then
			v3.FileWriteQuad(self.file_handle, n)
		end
	end
	
	function File:WriteLine(s)
		if self.opened then
			v3.FileWriteLn(self.file_handle, s)
		end
	end	
	
	function File:WriteString(s)
		if self.opened then
			v3.FileWriteString(self.file_handle, s)
		end
	end	
	
	function File:__tostring()
		return "file " .. ObjectAttributesToString(self)
	end
end
