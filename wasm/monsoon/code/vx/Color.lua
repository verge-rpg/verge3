function vx.RGB(r, g, b)
	return v3.RGB(num(r), num(g), num(b))
end

vx.MakeColor = vx.RGB
vx.MakeColour = vx.RGB

function vx.MixColor(c1, c2, blend)
	return v3.MixColor(num(c1), num(c2), num(blend))
end

vx.MixColour = vx.MixColor

function vx.GetR(c)
	return v3.GetR(num(c))
end

function vx.GetG(c)
	return v3.GetG(num(c))
end

function vx.GetB(c)
	return v3.GetB(num(c))
end

function vx.HSV(h, s, v)
	return v3.HSV(num(h), num(s), num(v))
end

function vx.GetH(c)
	return v3.GetH(num(c))
end

function vx.GetS(c)
	return v3.GetS(num(c))
end

function vx.GetV(c)
	return v3.GetV(num(c))
end