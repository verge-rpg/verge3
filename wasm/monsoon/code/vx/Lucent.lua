vx.lucent = 0
function vx.SetLucent(lucent)
	vx.lucent = lucent
	v3.SetLucent(num(lucent))
end
function vx.GetLucent()
	return vx.lucent
end
function vx.SetOpacity(opacity)
	vx.SetLucent(100 - opacity)
end
function vx.GetOpacity()
	return 100 - vx.lucent
end