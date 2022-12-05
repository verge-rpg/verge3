-- Color filter enumeration
vx.FilterEffect = {
	None = v3.CF_NONE, 
	Greyscale = v3.CF_GREY,
	GreyscaleInvert = v3.CF_GREYINV,
	Invert = v3.CF_INV,
	Red = v3.CF_RED,
	Green = v3.CF_GREEN,
	Blue = v3.CF_BLUE,
	Custom = v3.CF_CUSTOM
}

vx.custom_color_filter_color_1 = vx.RGB(0,0,0)
vx.custom_color_filter_color_2 = vx.RGB(0,0,0)

function vx.SetCustomColorFilter(color1, color2)
	vx.custom_color_filter_color_1 = num(color1)
	vx.custom_color_filter_color_2 = num(color2)
	vx.RestoreCustomColorFilter();
end

function vx.RestoreCustomColorFilter()
	v3.SetCustomColorFilter(vx.custom_color_filter_color_1, vx.custom_color_filter_color_2)
end
