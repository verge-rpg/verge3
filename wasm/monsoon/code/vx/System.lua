-- Takes an argument list of any length and type, and logs them as strings.
-- Overrides Lua's default print.
function print(...)
	local t = {...}
	local tab = false
	local s = ""
	for i, v in ipairs(t) do
		if tab then
			s = s .. "    "
		end
		s = s .. tostring(v)
		tab = true
	end
	v3.log(s)
end

vx.Log = print

-- Takes an argument list of any length and type, and logs them as strings.
function vx.Exit(...)
	local t = {...}
	local s = ""
	local tab = false
	
	for i, v in ipairs(t) do
		if tab then
			s = s .. "    "
		end
		s = s .. tostring(v)
		tab = true
	end
	v3.Exit(s)
end

-- Takes an argument list of any length and type, and logs them as strings.
function vx.MessageBox(...)
	local t = {...}
	local s = ""
	local tab = false
	
	for i, v in ipairs(t) do
		if tab then
			s = s .. "    "
		end
		s = s .. tostring(v)
		tab = true
	end
	v3.MessageBox(s)
end

function vx.SetAppName(title)
	v3.SetAppName(tostring(title))
end

function vx.SetResolution(xres, yres)
	v3.SetResolution(num(xres), num(yres))
end