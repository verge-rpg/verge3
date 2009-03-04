//#define BPP 32

#if BPP==16
#define ALPHA_CHANNEL
#define IFAC(X) X
#else
#define IFAC(X)
#endif


void bpperr() { err("blitter operation unsupported in your bpp"); }

FORCEINLINE int _MakeColor(int r, int g, int b) {
	return T_MakeColor<BPP>(r,g,b);
}

FORCEINLINE void _GetColor(int c, int &r, int &g, int &b) {
	T_GetColor<BPP>(c,r,g,b);
}

FORCEINLINE void BLEND_PIXELS(PT* dst, PT* src, byte* sa, const int num) {
	for(int i=0;i<num;i++) {
		if(BPP==32) {
			const Color& sc = ((Color*)src)[i];
			Color& dc = ((Color*)dst)[i];

			if(sc.a == 0) continue;
			if (sc.a == 255)
			{
				dc = sc;
				continue;
			}

			int pa = sc.a;
			int ipa = 255-pa;

			dc.r = ((sc.r*pa)+(dc.r*ipa))>>8;
			dc.g = ((sc.g*pa)+(dc.g*ipa))>>8;
			dc.b = ((sc.b*pa)+(dc.b*ipa))>>8;
		} else if(BPP==16) {
			//todo - use lookup table
			byte pa = sa[i];
			byte ipa = 255-pa;
			int sr,sg,sb,dr,dg,db;
			_GetColor(src[i],sr,sg,sb);
			_GetColor(dst[i],dr,dg,db);
			dr = ((sr*pa)+(dr*ipa))>>8;
			dg = ((sg*pa)+(dg*ipa))>>8;
			db = ((sb*pa)+(db*ipa))>>8;
			dst[i] = _MakeColor(dr,dg,db);
		}
	}
}

inline void COPY_PIXELS_320_480(PT * dst, PT* src) {
	memcpy(dst,src,320*480*sizeof(PT));
}

inline void COPY_PIXELS_320(PT* dst, PT* src) {
	memcpy(dst,src,320*sizeof(PT));
}

inline void COPY_PIXELS(PT* dst, PT* src, const int num) {
	for(int i=0;i<num;i++)
		*dst++=*src++;
}


template<bool CLIP>
static void dd32_PutPixel(int x, int y, int color, image *dest)
{
	PT *ptr = (PT *)dest->data;
	if(CLIP)
		if (x<dest->cx1 || x>dest->cx2 || y<dest->cy1 || y>dest->cy2)
			return;
	ptr[(y * dest->pitch) + x] = color;
}

#if BPP==32
template<bool CLIP>
static void dd32_PutPixel_50lucent(int x, int y, int color, image *dest)
{
	quad s, c;
	int *d=(int *)dest->data;

	if(CLIP)
		if (x<dest->cx1 || x>dest->cx2 || y<dest->cy1 || y>dest->cy2)
			return;

	s=d[(y * dest->pitch) + x];
	c=(s & tmask) + (color & tmask);
	d[(y * dest->pitch) + x] = (int) (c >> 1);
}
#else
template<bool CLIP> static void dd32_PutPixel_50lucent(int x, int y, int color, image *dest) {}
#endif

#if BPP==32
template<bool CLIP>
static void dd32_PutPixel_lucent(int x, int y, int color, image *dest)
{
	byte *d, *c;

	if(CLIP)
		if (x<dest->cx1 || x>dest->cx2 || y<dest->cy1 || y>dest->cy2)
			return;

	c = (byte *) &color;
	d = (byte *) dest->data;
	d += ((y*dest->pitch)+x)<<2;
	//MBG TODO - THIS LOOKS LIKE A HACK THAT WILL BREAK ALPHA CHANNEL
#ifdef __BIG_ENDIAN__
	d++;
	c++;
#endif
	*d = ((*d * alpha) + (*c * ialpha)) / 100; d++; c++;
	*d = ((*d * alpha) + (*c * ialpha)) / 100; d++; c++;
	*d = ((*d * alpha) + (*c * ialpha)) / 100;
}
#else
template<bool CLIP> static void dd32_PutPixel_lucent(int x, int y, int color, image *dest) {}
#endif


template<LUCENT_TYPE LT, bool CLIP> 
static void T_PutPixel(int x, int y, int color, image *dest)
{
	switch(LT) {
		case NONE: dd32_PutPixel<CLIP>(x,y,color,dest); break;
		case HALF: dd32_PutPixel_50lucent<CLIP>(x,y,color,dest); break;
		case ANY:  dd32_PutPixel_lucent<CLIP>(x,y,color,dest); break;
	}
}

template<LUCENT_TYPE LT>
void T_Line(int x, int y, int xe, int ye, int color, image *dest)
{
	if (x == xe) { T_VLine<LT>(x,y,ye,color,dest); return; }
	if (y == ye) { T_HLine<LT,true>(x,y,xe,color,dest); return; }

	// Bresenham's Line Drawing Algorithm

	int cx, cy, dx, dy;
	int xaccum, yaccum, xincre, yincre, xreset, yreset, xchange, ychange;
	int finished = 0;

	dx = abs(xe-x);
	dy = abs(ye-y);
	cx = x; cy = y;
	
	if(xe == x) xchange = 0; else if(xe < x) xchange = 0-1; else xchange = 1;
	if(ye == y) ychange = 0; else if(ye < y) ychange = 0-1; else ychange = 1;
	if(dx > dy) {
		xaccum = 0; xreset = 0; xincre = 0;
		yaccum = dy*2 - dx;
		yincre = dy*2;
		yreset = (dy-dx)*2;
	} else {
		yaccum = 0; yreset = 0; yincre = 0;
		xaccum = dx*2 - dy;
		xincre = dx*2;
		xreset = (dx-dy)*2;
	}
	
	while(!finished) {
		if(xaccum < 0) {
			xaccum += xincre;
		} else {
			cx += xchange;
			xaccum += xreset;
		}
		
		if(yaccum < 0) {
			yaccum += yincre;
		} else {
			cy += ychange;
			yaccum += yreset;
		}
		
		T_PutPixel<LT,true>(cx,cy,color,dest);
		
		if(xreset == 0 && cx == xe) finished = 1;
		if(yreset == 0 && cy == ye) finished = 1;
	}
	
	/*
	int dx = xe - x, dy = ye - y,
		xg = sgn(dx), yg = sgn(dy),
		i = 0;
	float slope = 0;

	if (abs(dx) >= abs(dy))
	{
		slope = (float) dy / (float) dx;
		for (i=0; i!=dx; i+=xg)
			PutPixel(x+i, y+(int)(slope*i), color, dest);
	}
	else
	{
		slope = (float) dx / (float) dy;
		for (i=0; i!=dy; i+=yg)
			PutPixel(x+(int)(slope*i), y+i, color, dest);
	}
	PutPixel(xe, ye, color, dest);
	*/
}



template<LUCENT_TYPE LT, bool CLIP>
void T_HLine(int x, int y, int xe, int color, image *dest)
{
	PT *d = (PT *) dest->data;
	
	if(CLIP) {
		int cx1=0, cy1=0, cx2=0, cy2=0;
		if (xe<x) SWAP(x,xe);
		dest->GetClip(cx1, cy1, cx2, cy2);
		if (x>cx2 || y>cy2 || xe<cx1 || y<cy1)
			return;
		if (xe>cx2) xe=cx2;
		if (x<cx1)  x =cx1;
	}

	d += (y * dest->pitch) + x;
	for (; x<=xe; x++) {
		switch(LT) {
			case NONE: *d++ = color; break;
			case HALF: {
				if(BPP==32) {
					int s=*d;
					s=(s & tmask) + (color & tmask);
					*d++ = (s >> 1);
				} else bpperr();
				break;
			}
			case ANY:
				T_PutPixel<ANY,false>(x, y, color, dest);
				break;
		}
	}
}

template<LUCENT_TYPE LT>
void T_Box(int x, int y, int x2, int y2, int color, image *dest)
{
	if (x2<x) SWAP(x,x2);
	if (y2<y) SWAP(y,y2);
	T_HLine<LT,true>(x, y, x2, color, dest);
	T_HLine<LT,true>(x, y2, x2, color, dest);
	T_VLine<LT>(x, y+1, y2-1, color, dest);
	T_VLine<LT>(x2, y+1, y2-1, color, dest);
}

template<LUCENT_TYPE LT>
static void T_Rect(int x, int y, int x2, int y2, int color, image *dest)
{
	int cx1, cy1, cx2, cy2;
	if (x2<x) SWAP(x,x2);
	if (y2<y) SWAP(y,y2);
	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x2<cx1 || y2<cy1)
		return;
	if (x2>cx2) x2=cx2;
	if (x<cx1)  x =cx1;
	if (y2>cy2) y2=cy2;
	if (y<cy1)  y =cy1;

	//speed optimization for entire-buffer clearing
	if(y==0&&y2==dest->height-1
		&&x==0&&x2==dest->width-1
		&&dest->pitch == dest->width) {
			int todo = dest->width*dest->height;
			PT* d = (PT*)dest->data;
			for(int i=0;i<todo;i++)
				*d++ = color;
			return;
	}

	for (; y<=y2; y++)
		T_HLine<LT,false>(x, y, x2, color, dest);
}

template<LUCENT_TYPE LT>
void T_VLine(int x, int y, int ye, int color, image *dest)
{
	PT *d = (PT *) dest->data;
	int cx1=0, cy1=0, cx2=0, cy2=0;
	if (ye<y) SWAP(y,ye);
	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x<cx1 || ye<cy1)
		return;
	if (ye>cy2) ye=cy2;
	if (y<cy1)  y =cy1;

	d += (y * dest->pitch) + x;
	for (; y<=ye; y++, d+=dest->pitch) {
		switch(LT) {
			case NONE: *d = color; break;
			case HALF: {
				if(BPP==32) {
					quad s=*d;
					s=(s & tmask) + (color & tmask);
					*d = (s >> 1);
				} else bpperr();
				break;
			}
			case ANY:
				T_PutPixel<ANY,false>(x, y, color, dest);
				break;
		}
	}
}

template<LUCENT_TYPE LT>
void T_Triangle(int x1, int y1, int x2, int y2, int x3, int y3, int color, image *dest)
{
	int xstep,xstep2;
	int xval,xval2;
	int yon;
	int swaptemp;

	if (y1 > y3)
	{
		swaptemp=x1; x1=x3; x3=swaptemp;
		swaptemp=y1; y1=y3; y3=swaptemp;
	}
	if (y2 > y3)
	{
		swaptemp=x2; x2=x3; x3=swaptemp;
		swaptemp=y2; y2=y3; y3=swaptemp;
	}
	if (y1 > y2)
	{
		swaptemp=x1; x1=x2; x2=swaptemp;
		swaptemp=y1; y1=y2; y2=swaptemp;
	}

	xstep2=((x3-x1) << 16) / ((y3-y1) ? (y3-y1) : 1);
	xval2=x1 << 16;

	if (y1 != y2)
	{
		xstep = ((x2-x1) << 16) / (y2-y1);
		xval = x1 << 16;
		for (yon=y1; yon < y2; yon++)
		{
			if (yon >= dest->cy1 && yon <= dest->cy2)
				T_HLine<LT,true>(xval>>16, yon, xval2>>16, color, dest);
			xval+=xstep;
			xval2+=xstep2;
		}
	}
	if (y2 != y3)
	{
		xstep = ((x3-x2) << 16) / ((y3-y2) ? (y3-y2) : 1);
		xval = x2 << 16;
		for (yon=y2;yon < y3; yon++)
		{
			if (yon >= dest->cy1 && yon <= dest->cy2)
				T_HLine<LT,true>(xval>>16, yon, xval2>>16, color, dest);
			xval+=xstep;
			xval2+=xstep2;
		}
	}
}

template<LUCENT_TYPE LT>
void T_Oval(int x, int y, int xe, int ye, int color, int Fill, image *dest)
{
	int m=xe-x, n=ye-y,
		//mi=m/2,  //mbg 9/5/05 this variable is not being used. why? probably unnecessary
		ni=n/2,
		dx=4*m*m,
		dy=4*n*n,
		r=m*n*n,
		rx=2*r,
		ry=0,
		xx=m,
		lasty=9999;

	y+=ni;
	if (Fill)
		T_HLine<LT,true>(x, y, x+xx-1, color, dest);
	else {
		T_PutPixel<LT,true>(x, y, color, dest);
		T_PutPixel<LT,true>(x+xx, y, color, dest);
	}

	xe=x, ye=y;
	if (ni+ni==n)
	{
		ry=-2*m*m;
	}
	else
	{
		ry=2*m*m;
		ye++;

		if (Fill)
			T_HLine<LT,true>(xe, ye, xe+xx-1, color, dest);
		else {
			T_PutPixel<LT,true>(xe, ye, color, dest);
			T_PutPixel<LT,true>(xe+xx, ye, color, dest);
		}
	}

	while (xx>0)
	{
		if (r<=0)
		{
			xx-=2;
			x++, xe++;
			rx-=dy;
			r+=rx;
		}
		if (r>0)
		{
			y--, ye++;
			ry+=dx;
			r-=ry;
		}

		if (Fill && y != lasty)
		{
			T_HLine<LT,true>(x, y, x+xx-1, color, dest);
			T_HLine<LT,true>(xe, ye, xe+xx-1, color, dest);
		}
		else {
			T_PutPixel<LT,true>(x, y, color, dest);
			T_PutPixel<LT,true>(x+xx, y, color, dest);
			T_PutPixel<LT,true>(xe, ye, color, dest);
			T_PutPixel<LT,true>(xe+xx, ye, color, dest);
		}
		lasty = y;
	}
}

template<LUCENT_TYPE LT>
void T_Sphere(int x, int y, int xradius, int yradius, int color, image *dest)
{
	T_Oval<LT>(x-xradius, y-yradius, x+xradius-1, y+yradius-1, color, 1, dest);
}

template<LUCENT_TYPE LT>
void T_Circle(int x, int y, int xradius, int yradius, int color, image *dest)
{
	T_Oval<LT>(x-xradius, y-yradius, x+xradius-1, y+yradius-1, color, 0, dest);
}

template<LUCENT_TYPE LT, int FILTER>
static void _T_ColorFilter(image *img) {
	int rr, gg, bb, z, c;

	int x1,x2,y1,y2;
	img->GetClip(x1,y1,x2,y2);

	PT *ptr = (PT*)img->data;
	PT *data = (PT*)&ptr[(y1 * img->pitch) + x1];

	for (int y=y1; y<=y2; y++)
	{
		//int* data_end = data+x2+1;
		for(int x=x1;x<=x2;x++,data++) {
			int col = *data;
			if (col == transColor) continue; // Overkill (2006-07-27): Ignore trans pixels
			GetColor(col, rr, gg, bb);
			switch (FILTER)
			{
				case 0: 
				case 1: z = (rr+gg+bb)/3; c = _MakeColor(z,z,z); break;
				case 2: z = 255-((rr+gg+bb)/3); c = _MakeColor(z,z,z); break;
				case 3: c = _MakeColor(255-rr, 255-gg, 255-bb); break;
				case 4: z = (rr+gg+bb)/3; c = _MakeColor(z, 0, 0); break;
				case 5: z = (rr+gg+bb)/3; c = _MakeColor(0, z, 0); break;
				case 6: z = (rr+gg+bb)/3; c = _MakeColor(0, 0, z); break;
				case 7: z = (rr+gg+bb)/3; c = _MakeColor(cf_r1+((cf_rr*z)>>8), cf_g1+((cf_gr*z)>>8), cf_b1+((cf_br*z)>>8)); break;
			}
			
			T_PutPixel<LT,false>(x, y, c, img);

			*data = c;
		}
		data += img->pitch - (x2-x1+1);
	}
}

template<LUCENT_TYPE LT>
static void T_ColorFilter(int filter, image *img)
{
	switch (filter)
	{
		case 0: _T_ColorFilter<LT,0>(img); break;
		case 1: _T_ColorFilter<LT,1>(img); break;
		case 2: _T_ColorFilter<LT,2>(img); break;
		case 3: _T_ColorFilter<LT,3>(img); break;
		case 4: _T_ColorFilter<LT,4>(img); break;
		case 5: _T_ColorFilter<LT,5>(img); break;
		case 6: _T_ColorFilter<LT,6>(img); break;
		case 7: _T_ColorFilter<LT,7>(img); break;
		default: break;
	}
}

template<LUCENT_TYPE LT, bool TRANSPARENT>
void T_AddBlit(int x, int y, image *src, image *dest)
{
	bpperr();
}

template<LUCENT_TYPE LT, bool TRANSPARENT>
void T_SubtractBlit(int x, int y, image *src, image *dest)
{
	bpperr();
}


template<LUCENT_TYPE LT>
void T_Blit(int x, int y, image *src, image *dest)
{
	PT *s=(PT *)src->data,
		*d=(PT *)dest->data;
	byte* sa = src->alphaChannel;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1, cy1, cx2, cy2;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen>cx2) xlen = cx2-x+1;
	if (y+ylen>cy2) ylen = cy2-y+1;
	if (x<cx1) 	{
		s +=(cx1-x);
		sa+=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		sa+=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}

	d += (y * dpitch) + x;
	switch(LT) {
		case NONE: 
			if(src->alpha) {
				for (; ylen--; s+=spitch, sa+=src->width, d+=dpitch)
					BLEND_PIXELS(d,s,sa,xlen);
			}
			else {
				//iphone optimization
				if(xlen == 320) {
					if(ylen == 480 && dpitch == 320 && spitch == 320)
						COPY_PIXELS_320_480(d,s);
					else
						for (; ylen--; s+=spitch, d+=dpitch)
							COPY_PIXELS_320(d,s);
				}
				else
					for (; ylen--; s+=spitch, d+=dpitch)
						COPY_PIXELS(d,s,xlen);
			}
			break;
		case HALF: {
			if(BPP==32)
				for (; ylen; ylen--) {
					for (x=0; x<xlen; x++) {
						int sc=s[x];
						sc=(sc & tmask) + (d[x] & tmask);
						d[x] = (sc >> 1);
					}
					s+=spitch;
					d+=dpitch;
				}
			else bpperr();
			break;
		}
		case ANY: {
			if(BPP==32)
				for (; ylen; ylen--) {
					for (x=0; x<xlen; x++) {
						quad r1, g1, b1;
						quad r2, g2, b2;
						quad dp;

						int c = s[x];
						dp = d[x];

						r1 = (c >> 16) & 0xff,
						g1 = (c >> 8) & 0xff,
						b1 = (c & 0xff);

						r2 = (dp >> 16) & 0xff,
						g2 = (dp >> 8) & 0xff,
						b2 = (dp & 0xff);

						d[x] = ((((r1 * ialpha) + (r2 * alpha)) / 100) << 16) |
							   ((((g1 * ialpha) + (g2 * alpha)) / 100) << 8) |
							   ((((b1 * ialpha) + (b2 * alpha)) / 100));
					}
					s+=spitch;
					d+=dpitch;
				}
			else bpperr();
			break;
		}
	}
		
}

template<LUCENT_TYPE LT>
void T_FlipBlit(int x, int y, int fx, int fy, image *src, image *dest)
{
	err("this function probably doesnt work.");

	if(!fx && !fy) T_Blit<LT>(x,y,src,dest);

	PT *s=(PT *)src->data,
		*d=(PT *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen>cx2) xlen = cx2-x+1;
	if (y+ylen>cy2) ylen = cy2-y+1;
	if (x<cx1) 	{
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}

	d += (y * dpitch) + x;

	if(fx && !fy)
		for (; ylen--; s+=spitch, d+=dpitch)
			for(int i=0;i<xlen;i++)
				d[i]=s[xlen-i-1];

	else if(!fx && fy)
	{
		s+=spitch*(ylen-1);
		for (xlen *= 4; ylen--; s-=spitch, d+=dpitch)
			memcpy(d, s, xlen);
	}
	else if(fx && fy)
	{
		s+=spitch*(ylen-1);
		for (; ylen--; s-=spitch, d+=dpitch)
			for(int i=0;i<xlen;i++)
				d[i]=s[xlen-i-1];

	}
}

template<LUCENT_TYPE LT>
void T_BlitTile(int x, int y, char *src, image *dest)
{
	PT *s=(PT*)src,
		 *d=(PT*)dest->data;
	int spitch=16,
		dpitch=dest->pitch;
	int xlen=16,
		ylen=16;
	int cx1, cy1, cx2, cy2;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen > cx2) xlen=cx2-x+1;
	if (y+ylen > cy2) ylen=cy2-y+1;
	if (x<cx1) {
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}

	d+=(y*dest->pitch)+x;

	switch(LT) {
		case NONE:
		case HALF:
		case ANY:
			for (;ylen--; s+=spitch,d+=dpitch)
				COPY_PIXELS(d,s,xlen);
			break;
	}
}

template<LUCENT_TYPE LT>
void T_TBlitTile(int x, int y, char *src, image *dest)
{
	if(BPP != 32) bpperr();

	PT *s=(PT*)src,
		 *d=(PT*)dest->data;
	int spitch=16,
		dpitch=dest->pitch;
	int xlen=16,
		ylen=16;
	int cx1, cy1, cx2, cy2;
	PT c;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen > cx2) xlen=cx2-x+1;
	if (y+ylen > cy2) ylen=cy2-y+1;

	if (x<cx1) {
		s +=(cx1-x);
		xlen-=(cx1-x);
		x = cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y=cy1;
	}
	d+=y*dpitch+x;
	for (; ylen; ylen--)
	{
		for (x=0; x<xlen; x++)
		{
			switch(LT) {
			case NONE:
				c=s[x];
				if (c != (PT)transColor) d[x]=c;
				break;
			case ANY:
			case HALF: {
			#if BPP==32
				c=s[x];
				if (c == (PT)transColor) continue;

				quad r1, g1, b1;
				quad r2, g2, b2;
				quad dp;

				dp = d[x];

				r1 = (c >> 16) & 0xff,
				g1 = (c >> 8) & 0xff,
				b1 = (c & 0xff);

				r2 = (dp >> 16) & 0xff,
				g2 = (dp >> 8) & 0xff,
				b2 = (dp & 0xff);

				d[x] = ((((r1 * ialpha) + (r2 * alpha)) / 100) << 16) |
					   ((((g1 * ialpha) + (g2 * alpha)) / 100) << 8) |
					   ((((b1 * ialpha) + (b2 * alpha)) / 100));
				break;
				#endif
				}
			}
		}
		s+=spitch;
		d+=dpitch;
	}
}



template<LUCENT_TYPE LT>
void T_TBlit(int x, int y, image *src, image *dest)
{
	PT *s=(PT *)src->data,c,
		*d=(PT *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1, cy1, cx2, cy2;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen > cx2) xlen=cx2-x+1;
	if (y+ylen > cy2) ylen=cy2-y+1;
	if (x<cx1) {
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}
	d+=y*dpitch+x;
	for (; ylen; ylen--) {
		for (x=0; x<xlen; x++) {
			switch(LT) {
				case NONE:
					c=s[x];
					if (c != transColor) d[x]=c;
					break;
				case HALF: {
					if(BPP==32) {
						int sc=s[x]; if (sc == transColor) continue;
						sc=(sc & tmask) + (d[x] & tmask);
						d[x] = (sc >> 1);
					} else bpperr();
					break;
				}
				case ANY: {
					if(BPP==32) {
						quad r1, g1, b1;
						quad r2, g2, b2;
						quad dp;

						int c = s[x];
						if (c == transColor) continue;
						dp = d[x];

						r1 = (c >> 16) & 0xff,
						g1 = (c >> 8) & 0xff,
						b1 = (c & 0xff);

						r2 = (dp >> 16) & 0xff,
						g2 = (dp >> 8) & 0xff,
						b2 = (dp & 0xff);

						d[x] = ((((r1 * ialpha) + (r2 * alpha)) / 100) << 16) |
							   ((((g1 * ialpha) + (g2 * alpha)) / 100) << 8) |
							   ((((b1 * ialpha) + (b2 * alpha)) / 100));
					} else bpperr();
				}
				break;
			}
		}
		s+=spitch;
		d+=dpitch;
	}
}

template<LUCENT_TYPE LT, bool TRANSPARENT>
void T_ScaleBlit(int x, int y, int dw, int dh, image *src, image *dest)
{
	int i, j;
	int xerr, yerr;
	int xerr_start, yerr_start;
	int xadj, yadj;
	quad *d, *s;
	int xl, yl, xs, ys;
	int cx1, cy1, cx2, cy2;

	if (dw < 1 || dh < 1)
		return;

	xl = dw;
	yl = dh;
	xs = ys = 0;
	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x > cx2 || y > cy2
		|| x + xl < cx1 || y + yl < cy1)
			return;
	if (x + xl > cx2)
		xl = cx2 - x + 1;
	if (y + yl > cy2)
		yl = cy2 - y + 1;
	if (x < cx1) { xs = cx1 - x; xl -= xs; x = cx1; }
	if (y < cy1) { ys = cy1 - y; yl -= ys; y = cy1; }

	xadj = (src->width << 16)/dw;
	yadj = (src->height << 16)/dh;
	xerr_start = xadj * xs;
	yerr_start = yadj * ys;

	s = (quad *) src->data;
	s += ((yerr_start >> 16) * src->pitch);
	d = ((quad *) dest->data) + (y * dest->pitch) + x;
	yerr = yerr_start & 0xffff;

	for (i = 0; i < yl; i += 1) {
		xerr = xerr_start;
		for (j = 0; j < xl; j += 1) {
			int c = s[(xerr >> 16)];
			switch(LT) {
				case NONE:
					if (!TRANSPARENT || c != transColor)
						d[j] = c;
					break;
				case HALF:
					if(BPP==32) {
						if (!TRANSPARENT || c != transColor) {
							c = (c & tmask) + (d[j] & tmask);
							d[j] = (c >> 1);
						}
					} else bpperr();
					break;
				case ANY:
					if(BPP==32)
					if (!TRANSPARENT || c != transColor) {

						quad r1, g1, b1;
						quad r2, g2, b2;
						quad dp;

						dp = d[j];

						r1 = (c >> 16) & 0xff,
						g1 = (c >> 8) & 0xff,
						b1 = (c & 0xff);

						r2 = (dp >> 16) & 0xff,
						g2 = (dp >> 8) & 0xff,
						b2 = (dp & 0xff);

						d[j] = ((((r1 * ialpha) + (r2 * alpha)) / 100) << 16) |
							   ((((g1 * ialpha) + (g2 * alpha)) / 100) << 8) |
							   ((((b1 * ialpha) + (b2 * alpha)) / 100));
					}
					else bpperr();
			}
			xerr += xadj;
		}
		d    += dest->pitch;
		yerr += yadj;
		s    += (yerr>>16)*src->pitch;
		yerr &= 0xffff;
	}
}

template<LUCENT_TYPE LT, bool TRANSPARENT>
void T_WrapBlit(int x, int y, image *src, image *dst)
{
	if(LT != NONE) bpperr();
	if(TRANSPARENT) bpperr();

	int i;
	int cliph, clipw;
	int curx, cury;
	int spanx, spany;
	PT *source, *dest;

	cliph = dst->cy2 - dst->cy1 + 1;
	clipw = dst->cx2 - dst->cx1 + 1;
	y %= src->height;
	if (y<0) y = src->height+y;
	curx = 0;

	do
	{
		x %= curx ? 1 : src->width;
		if (x<0) x = src->width+x;
		spanx = src->width - x;
		if (curx + spanx >= clipw)
			spanx = clipw - curx;
		source = (PT *) src -> data + (y * src->pitch) + x;
		dest = (PT *) dst -> data + (dst->cy1 * dst->pitch) + dst->cx1 + curx;
		cury = 0;

		do
		{
			spany = src->height - (cury ? 0 : y);
			if (cury + spany >= cliph)
				spany = cliph - cury;

			for (i = 0; i < spany; i++, source += src->pitch, dest += dst->pitch)
				memcpy(dest, source, spanx*sizeof(PT));

			source = (PT *) src->data + x;
			cury += spany;
		} while (cury < cliph);
		curx +=	spanx;
	} while (curx < clipw);
}

template<LUCENT_TYPE LT, bool TRANSPARENT>
void T_RotScale(int posx, int posy, float angle, float scale, image* src, image *dest)
{
	if(LT != NONE) bpperr();

	int xs,ys,xl,yl;
	int srcx,srcy,x,y,tempx,tempy;

	PT *source = (PT*) src->data, *d;
	int width=src->width;
	int height=src->height;
	int clip_x = dest->cx1;
	int clip_y = dest->cy1;
	int clip_xend = dest->cx2;
	int clip_yend = dest->cy2;
	float ft=atan2((float)width,(float)height);
	int T_WIDTH_CENTER=width>>1;
	int T_HEIGHT_CENTER=height>>1;
	int W_WIDTH=(int)((float)width/scale*sin(ft) + (float)height/scale*cos(ft));
	int W_HEIGHT=W_WIDTH;
	int W_HEIGHT_CENTER=W_HEIGHT>>1;
	int W_WIDTH_CENTER=W_HEIGHT_CENTER;

	int sinas = (int)(sin(-angle)*65536*scale);
	int cosas = (int)(cos(-angle)*65536*scale);

	int xc = T_WIDTH_CENTER*65536 - (W_HEIGHT_CENTER*(cosas+sinas));
	int yc = T_HEIGHT_CENTER*65536 - (W_WIDTH_CENTER*(cosas-sinas));
	posx -= W_WIDTH_CENTER;
	posy -= W_HEIGHT_CENTER;

	// clipping
	if (W_WIDTH<2 || W_HEIGHT<2) return;
	xl=W_WIDTH;
	yl=W_HEIGHT;
	xs=ys=0;
	if (posx>clip_xend || posy>clip_yend || posx+xl<clip_x || posy+yl<clip_y)
		return;
	if (posx+xl > clip_xend) xl=clip_xend-posx+1;
	if (posy+yl > clip_yend) yl=clip_yend-posy+1;
	if (posx<clip_x)
	{
		xs=clip_x-posx;
		xl-=xs;
		posx=clip_x;

		xc+=cosas*xs;
		yc-=sinas*xs;
	}
	if (posy<clip_y)
	{
		ys=clip_y-posy;
		yl-=ys;
		posy=clip_y;

		xc+=sinas*ys;
		yc+=cosas*ys;
	}

	d=(quad*) dest->data+posx+posy*dest->pitch;
	for (y=0; y<yl; y++)
	{
		srcx=xc;
		srcy=yc;

		for (x=0; x<xl; x++)
		{
			tempx=(srcx>>16);
			tempy=(srcy>>16);

			if (tempx>=0 && tempx<width && tempy>=0 && tempy<height)
			{
				int sofs=tempx+tempy*src->pitch;
				if (!TRANSPARENT || source[sofs] != (quad)transColor)
					d[x]=source[sofs];
			}

			srcx+=cosas;
			srcy-=sinas;
		}
		d+=dest->pitch;
		xc+=sinas;
		yc+=cosas;
	}
}

template<LUCENT_TYPE LT, bool TRANSPARENT>
void T_BlitWrap(int x, int y, image *src, image *dest)
{
	if(LT != NONE) bpperr();

	PT *s=(PT *)src->data,c,
		 *d=(PT *)dest->data;
	int dpitch=dest->pitch;
	int dwidth=dest->width;
	int dheight=dest->height;

	for (int yy=0; yy < src->height; yy++)
	{
		for (int xx=0; xx < src->width; xx++)
		{
			c=s[(yy*src->pitch)+xx];
			if (!TRANSPARENT || c != (PT)transColor) 
				d[(((yy+y)%dheight)*dpitch)+((xx+x)%dwidth)]=c;
		}
	}
}

//TRANSPARENT is always true for silhouette. 
template<LUCENT_TYPE LT>
void T_Silhouette(int x, int y, int c, image *src, image *dest)
{
	if(LT != NONE) bpperr();

	int *s=(int *)src->data,
		*d=(int *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen > cx2) xlen=cx2-x+1;
	if (y+ylen > cy2) ylen=cy2-y+1;
	if (x<cx1) {
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}
	d+=y*dpitch+x;
	for (; ylen; ylen--)
	{
		for (x=0; x<xlen; x++)
			if (s[x] != transColor) 
				d[x]=c;
		s+=spitch;
		d+=dpitch;
	}
}

void T_Mosaic(int xf, int yf, image *src)
{
	if(BPP != 32) bpperr();

	int x,y,x2,y2,xc,yc,xr,yr;
	int x2max,y2max,xmax,ymax;
	int r,g,b;
	int i,j,c;
	int w,h;
	PT *data;
	PT color;
	int add,topleft;

	if(!(xf&&yf))
		return;

 	data=(PT *)src->data;
	w=src->cx2-src->cx1+1;
	h=src->cy2-src->cy1+1;
	xc=w/xf;
	yc=h/yf;
	xr=w%xf;
	yr=h%yf;
	add=src->pitch;

	if(xr)
		xmax=xc+1;
	else
		xmax=xc;
	if(yr)
		ymax=yc+1;
	else
		ymax=yc;

	for(x=0;x<xmax;x++)
	{
		topleft=x*xf+src->cx1+src->cy1*src->pitch;
		for(y=0;y<ymax;y++)
		{
			r=g=b=c=0;
			j=topleft;
			if(y<yc)
				y2max=yf;
			else
				y2max=yr;
			if(x<xc)
				x2max=xf;
			else
				x2max=xr;
			for(y2=0;y2<y2max;y2++)
			{
				i=j;
				for(x2=0;x2<x2max;x2++)
				{
  					b+=data[i]&0x000000FF;
					g+=(data[i]&0x0000FF00)>>8;
					r+=(data[i]&0x00FF0000)>>16;
					i++;
				}
				j+=add;
			}
			c=x2max*y2max;
			j=topleft;
			r/=c;
			g/=c;
			b/=c;
			color=_MakeColor(r,g,b);
			for(y2=0;y2<y2max;y2++)
			{
				i=j;
				for(x2=0;x2<x2max;x2++)
					data[i++]=color;
				j+=add;
			}
			topleft=j;
		}
	}
}

template<LUCENT_TYPE LT>
static void SetForLucentCommon()
{
	Blit            = T_Blit<LT>;
	TBlit           = T_TBlit<LT>;
	PutPixel		= T_PutPixel<LT,true>;
	Box				= T_Box<LT>;
	DrawRect        = T_Rect<LT>;
	Line			= T_Line<LT>;
	HLine			= T_HLine<LT,true>;
	VLine			= T_VLine<LT>;
	Triangle		= T_Triangle<LT>;
	Sphere          = T_Sphere<LT>;
	Circle          = T_Circle<LT>;
	ColorFilter     = T_ColorFilter<LT>;
	FlipBlit        = T_FlipBlit<LT>;
	BlitTile        = T_BlitTile<LT>;
	TBlitTile       = T_TBlitTile<LT>;
	ScaleBlit		= T_ScaleBlit<LT,false>;
	TScaleBlit      = T_ScaleBlit<LT,true>;
	WrapBlit		= T_WrapBlit<LT,false>;
	TWrapBlit		= T_WrapBlit<LT,true>;
	AdditiveBlit	= T_AddBlit<LT,false>;
	TAdditiveBlit	= T_AddBlit<LT,true>;
	SubtractiveBlit = T_SubtractBlit<LT,false>;
	TSubtractiveBlit= T_SubtractBlit<LT,true>;
	Silhouette		= T_Silhouette<LT>;
	BlitWrap		= T_BlitWrap<LT,false>;
	Mosaic			= T_Mosaic;
	MakeColor		= _MakeColor;
	GetColor		= _GetColor;
}

static void SetForLucent(int percent)
{
	if (percent == 0)
	{
		SetForLucentCommon<NONE>();
	}
	else if (percent == 50)
	{
		SetForLucentCommon<HALF>();
	}
	else
	{
		SetForLucentCommon<ANY>();
	}
}

#undef ALPHA_CHANNEL
#undef IFAC