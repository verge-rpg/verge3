-- Mungo is magical data format that easily chomps up silly datafiles
-- with the ferocity of a mongoose (mungos mungo). Then through parsing,
-- it can be excreted into tabular/associative/structured form!
-- [] are comments,
-- "", '', `` are string quotes
-- *-+|~>=# are the default list delimiters
--  (define more delimiters if you think you need to)
-- : is the mapping delimiter, used when associating keys with string values
local LIST_DELIMITERS = "*-+|~>=#"
local MAPPING_DELIMITER = ':'
local WHITESPACE = " \t"
local QUOTE = "'`\""
local COMMENT_OPEN = '['
local COMMENT_CLOSE = ']'
-- Returns whether or not a file exists.
local function FileExists(filename)
	local f = io.open(filename, "r")
	if f == nil then
		return false
	end
	f:close()
	return true
end

-- Convert Mungo format into Lua table
function MungoReadTable(filename)
	local result = {}
	if FileExists(filename) then
		local buffer
		do
			local tmp = {}
			for line in io.lines(filename) do
				table.insert(tmp, line)
			end
			table.insert(tmp, "")
			buffer = table.concat(tmp, "\n")
		end

		local i = 1
		local lists = {}
		local delimiters = {}
		local delimiter_current = nil
		local comment = false
		local quote = false
		local t = nil
		local k = ""
		local v = ""
		local line_index = 1
		while i <= #buffer do
			local c = buffer:sub(i, i)
			-- Next line
			if c == '\r' then
				if quote then
					v = v .. c
				end
			elseif c == '\n' then
				if quote then
					v = v .. c
				end
				line_index = line_index + 1
			elseif not comment and not quote then
				-- List delimiter
				if LIST_DELIMITERS:find(c, 1, true) then
					-- Map current key to a string value and create a new list item
					if t and delimiter_current == c then
						if #v == 0 then
							return nil, "Expected a value identifier but got list delimiter '" .. c .. "' on line " .. tostring(line_index)
						end
						t[k] = v
					-- Map current key to a list
					elseif not lists[c] then
						delimiter_current = c
						if #k > 0 and #v > 0 then
							return nil, "Unexpected list delimiter '" .. c .. "' on line " .. tostring(line_index)
						end
						if t and #v == 0 then
							return nil, "Expected a key identifier but got list delimiter '" .. c .. "' on line " .. tostring(line_index)
						end
						if not t then
							t = result 
						else
							t[v] = {}
							t = t[v]
						end
						lists[c] = t
						table.insert(delimiters, c)
					-- Map current key to a string value and return to a list on an upper level
					else
						delimiter_current = c
						if #v == 0 then
							return nil, "Expected a value identifier but got list delimiter '" .. c .. "' on line " .. tostring(line_index)
						end
						t[k] = v
						t = lists[c]
						local delimiter_index = nil
						for idx, delimiter in ipairs(delimiters) do
							if delimiter == c then
								delimiter_index = idx
								break
							end
						end

						if delimiter_index then
							while delimiter_index < #delimiters do
								lists[table.remove(delimiters)] = nil
							end
						end
					end
					k = ""
					v = ""
				-- Mapping delimiter, make current indentifier into the key, make new indentifier a value
				elseif c == MAPPING_DELIMITER then
					if not t then
						return nil, "Attempt to define mapping before a list was started on line " .. tostring(line_index)
					end
					if #v == 0 then
						return nil, "Expected a key identifier but got mapping delimiter '" .. c .. "' on line " .. tostring(line_index)
					end
					k = v
					v = ""
				elseif c == COMMENT_OPEN then
					comment = true
				elseif QUOTE:find(c, 1, true) then
					if not t then
						return nil, c .. " quote encountered before a list was started on line " .. tostring(line_index)
					end
					quote = c
				elseif not WHITESPACE:find(c, 1, true) then
					if not t then
						return nil, "Indentifier character '" .. c .. "' encountered before a list was started on line " .. tostring(line_index)
					end
					v = v .. c
				end
			elseif quote then
				if c == quote then
					quote = false
				else
					v = v .. c
				end
			elseif comment and c == COMMENT_CLOSE then
				comment = false				
			end
			i = i + 1
		end
		-- If we reached EOF mid-string, whine
		if #v == 0 or (#v > 0 and #k == 0) then
			return nil, "Expected a value identifier but got EOF on line " .. tostring(line_index)
		end
		if #k > 0 then
			t[k] = v
		end
	else
		return nil, "Failed to open '" .. filename .. "'"
	end

	-- Finally, return our resolved table.
	return result
end

local function _sort_keys(t)
	local keys = {}
	for key in pairs(t) do
		table.insert(keys, key)
	end
	table.sort(keys)
	return keys
end

local function sorted_pairs(t)
	local keys = _sort_keys(t)
	local i = 0
	local function iter(t)
		i = i + 1
		local k = keys[i]
		if k then
			return k, t[k]
		end
	end
	return iter, t
end

local function EscapeRegex(str)
	return str:gsub("[%^%$%(%)%%%.%[%]%*%+%-%?]", "%%%1")
end

-- For 'complex' key/value identifiers that contain Mungo special characters
local complex_regex = "[" .. EscapeRegex(LIST_DELIMITERS .. MAPPING_DELIMITER .. WHITESPACE .. QUOTE .. COMMENT_OPEN .. COMMENT_CLOSE) .. "]"

local function MungoWriteHuman(t, f, indent)
	local tab = string.rep("    ", indent - 1)
	local delim = LIST_DELIMITERS:sub(indent, indent)
	for k, v in sorted_pairs(t) do
		k = tostring(k)
	
		f:write(tab, delim, " ")
		if k:find(complex_regex) then			
			k = k:gsub("\"", "\" '\"' \"")
			f:write("\"", k, "\"")
		else
			f:write(k)			
		end
		
		if type(v) ~= "table" then
			v = tostring(v)
			f:write(MAPPING_DELIMITER, " ")
			if v:find(complex_regex) then
				v = v:gsub("\"", "\" '\"' \"")
				f:write("\"", v, "\"\n")
			else
				f:write(v, "\n")	
			end
		else
			f:write("\n")
			MungoWriteHuman(v, f, indent + 1)
		end
	end
end

function MungoWriteDirty(t, f, indent)
	local delim = LIST_DELIMITERS:sub(indent, indent)
	for k, v in pairs(t) do
		k = tostring(k)
		f:write(delim, "\"", k:gsub("\"", "\" '\"' \""), "\"")
		
		if type(v) ~= "table" then
			v = tostring(v)
			f:write(MAPPING_DELIMITER, "\"", v:gsub("\"", "\" '\"' \""), "\"")
		else
			MungoWriteDirty(v, f, indent + 1)
		end
	end
end

function MungoWriteTable(table, filename, write_method)
	write_method = write_method or MungoWriteHuman
	
	f = io.open(filename, "w")
	if not f then
		return "Failed to open '" .. filename .. "' for writing"
	end
	write_method(table, f, 1)
	f:close()
end
