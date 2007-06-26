using System;
using System.Drawing;

namespace winmaped2
{
	public class HSBColor
	{
		public float H,S,B;
	}
	public class PaintFunctions
	{
		public static void	PaintFrame(Graphics g, int x1, int y1, int w, int h)
		{
			g.DrawLine(SystemPens.ControlDark, x1,y1, x1+w-1, y1);
			g.DrawLine(SystemPens.ControlDark, x1,y1, x1, y1+h-1);
			g.DrawLine(SystemPens.ControlLightLight, x1,y1+h-1, x1+w-1,y1+h-1);
			g.DrawLine(SystemPens.ControlLightLight, x1+w-1,y1,x1+w-1,y1+h-1);
			g.DrawRectangle(Pens.Black,x1+1,y1+1,w-3,h-3);
		}
		public static void	PaintLine(Graphics gfx, int r, int g, int b, int x1, int y1, int x2, int y2)
		{
			gfx.DrawLine(new Pen(Color.FromArgb(r,g,b)),x1,y1,x2,y2);
			
		}

		// credit to foley/van dam gfx book
		public static HSBColor	ColorToHsb(Color c)
		{
			float minval,maxval,delta;
			float r=c.R/255.0f,g=c.G/255.0f,b=c.B/255.0f;
			HSBColor hsbc = new HSBColor();

			maxval = Math.Max( r, Math.Max(g,b) );
			minval = Math.Min( r, Math.Min(g,b) );

			hsbc.B = (maxval + minval) / 2.0f;

			if(maxval==minval)
			{
				// grayscale
				hsbc.S = 0.0f;
				hsbc.H = 0.0f;
				return hsbc;
			}
			if(hsbc.B < 0.5f)
				hsbc.S = (maxval - minval) / (maxval + minval);
			else
				hsbc.S = (maxval - minval) / (2.0f - maxval - minval);
			delta = maxval-minval;

			if(r==maxval)
				hsbc.H = (g-b) / delta;
			else if (g==maxval)
				hsbc.H = 2.0f + (b-r) / delta;
			else if (b==maxval)
				hsbc.H = 4.0f + (r-g) / delta;

			hsbc.H = hsbc.H * 60.0f;
			while(hsbc.H<0.0f) hsbc.H += 360.0f;
			while(hsbc.H>360.0f) hsbc.H -= 360.0f;
			return hsbc;
		}

	    // ===================================================//
		// HSB -> RGB STUFF RIPPED FROM:
		// http://www.gotdotnet.com/Community/UserSamples/Details.aspx?SampleGuid=6ab7a73c-7dbd-45ea-aa60-63119050ddec

		///<summary>Returns a Color structure specified by HSB values.</summary>
		///<param name='h'>The hue of the color.</param>
		///<param name='s'>The saturation of the color.</param>
		///<param name='b'>The brightness of the color.</param>
		///<returns>A Color structure specifed by the HSB values.</returns>
		public static Color HsbToColor(float h, float s, float b)
		{
			/*if(h < 0 || h >= 360)
			{
				throw new ArgumentOutOfRangeException("h", "h must be non-negative and less than 360.");
			}

			if(s < 0 || s > 1)
			{
				throw new ArgumentOutOfRangeException("s", "s must be non-negative and less than or equal to 1");
			}

			if(b < 0 || b > 1)
			{
				throw new ArgumentOutOfRangeException("b", "b must be non-negative and less than or equal to 1");
			}*/

			float m1;
			float m2;

			h = h/360;
			m2 = (b<=0.5) ? b*(s+1) : b+s-b*s;
			m1 = b*2-m2;

			return Color.FromArgb(
				HsbIntermediateValuesToRgbComponent(m1,m2,h + 1F/3),
				HsbIntermediateValuesToRgbComponent(m1,m2,h),
				HsbIntermediateValuesToRgbComponent(m1,m2,h - 1F/3)
				);
		}

		///<summary>Returns an RGB component for the intermediate values of the HSB to RGB convertion algorithm.</summary>
		///<param name='m1'>The first intermediate value.</param>
		///<param name='m2'>The second intermediate value.</param>
		///<param name='h'>The normalized hue value.</param>
		///<returns>An RGB component.</returns>
		private static int HsbIntermediateValuesToRgbComponent(float m1, float m2, float h)
		{
			if(h<0)
			{
				h++;
			}
			else if(h>1)
			{
				h--;
			}

			if(h*6<1) return (int)(255*(m1+(m2-m1)*h*6));
			if(h*2<1) return (int)(255*m2);
			if(h*3<2) return (int)(255*(m1+(m2-m1)*(2F/3-h)*6));
			return (int)(255*m1);
		}
	}
}
