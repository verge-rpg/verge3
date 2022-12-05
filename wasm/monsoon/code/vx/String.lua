-- Returns whether or not the string begins with the passed substring
function string:startsWith(substring)
	local match, end_match = self:find(substring)
	if match == 1 then
		return true
	end
	return false
end

-- Returns whether or not the string ends with the passed substring
function string:endsWith(substring)
	local match, end_match = self:find(substring)
	if end_match == #self then
		return true
	end
	return false
end

function string:TokenCount(delim)
	return v3.TokenCount(self, delim)
end

function string:GetToken(delim, index)
	return v3.GetToken(self, delim, index)
end
