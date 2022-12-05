-- Prints out public attributes and their current values.
--
-- The object passed can be a userdata with ._attr field
-- initialized like { key = defaultvalue, key2 = default2, ... }
-- and an actual field with each of these names. So if you have
-- "name" declared in your _attr, it needs to be declared on your
-- object as well.
--
-- Alternatively, it can be passed a table and it will return all
-- its key/value pairs as a string.
--
-- Failing that, it returns "".
function ObjectAttributesToString(o)
	local s = "(" .. type(o) .. ")"
	if type(o) == "userdata" or type(o) == "table" then
		s = s .. " { "
		if o._attr then
			for k, v in pairs(o._attr) do
				s = s .. k .. " = ";
				if type(o[k]) == "string" then
					s = s .. "\"" .. o[k] .. "\""
				else
					s = s .. tostring(o[k])
				end
				s = s .. "; "
			end
		elseif type(o) == "table" then
			for k, v in pairs(o) do
				s = s .. k .. " = ";
				if type(v) == "string" then
					s = s .. "\"" .. v .. "\""
				else
					s = s .. tostring(v)
				end
				s = s .. "; "
			end		
		else
			s = s .. "[userdata does not have an ._attr list] "
		end
		s = s .. "}"
	end
	return s
end

function ObjectAttributesToLog(o, indent)
	local t = ""
	indent = indent or 0
	
	local function GetIndent()
		if indent > 0 and t == "" then
			for i = 1, indent do
				t = t .. "\t"
			end
			return t
		end
		return t
	end
	
	v3.log(GetIndent() .. "(" .. type(o) .. ")")
	if type(o) == "table" then
		v3.log(GetIndent() .. "{")
		if o._attr then
			for k, v in pairs(o._attr) do
				v3.log(GetIndent() .. "\t" .. k .. ":")
				if type(o[k]) == "string" then
					v3.log(GetIndent() .. "\t\t\"" .. o[k] .. "\"")
				elseif type(o[k]) == "table" or type(o[k]) == "userdata" then
					ObjectAttributesToLog(o[k], indent + 2)
				else
					v3.log(GetIndent() .. "\t\t" .. tostring(o[k]))
				end
			end
		elseif type(o) == "table" then
			for k, v in pairs(o) do
				v3.log(GetIndent() .. "\t" .. k .. ":");
				if type(v) == "string" then
					v3.log(GetIndent() .. "\t\t\"" .. v .. "\"")
				elseif type(v) == "table" or type(v) == "userdata" then
					ObjectAttributesToLog(v, indent + 2)
				else
					v3.log(GetIndent() .. "\t\t" .. tostring(v))
				end
			end		
		else
			v3.log(GetIndent() + "[userdata does not have an ._attr list] ")
		end
		v3.log(GetIndent() .. "}")
	end
end

-- Takes a 1-based integer array table and returns an ordered list of all the items.
-- Returns "" if the argument passed is not a table.
function IndexedValuesToString(t)
	local comma = false
	local s = ""
	if type(t) == "table" then
		for i, v in ipairs(t) do
			if comma then
				s = s .. ", "
			end
			s = s .. tostring(v)
			comma = true
		end
	end
	return s
end

-- Takes a 1-based integer array table and returns an ordered list of the types of each item
-- Returns "" if the argument passed is not a table.
function IndexedTypesToString(t)
	local comma = false
	local s = ""
	if type(t) == "table" then
		for i, v in ipairs(t) do
			if comma then
				s = s .. ", "
			end
			s = s .. type(v)
			comma = true
		end
	end
	return s
end