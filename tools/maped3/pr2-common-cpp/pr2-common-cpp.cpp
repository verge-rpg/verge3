// pr2-common-cpp.h

#pragma once
#using <mscorlib.dll>

using namespace System;

namespace pr2
{	
	public __gc class Primitives
	{
		public:
		__delegate void Callback(int x, int y, Object __gc* tag);

		__gc class FloodFill
		{
		public:
			__delegate bool CheckCallback(int x, int y, Object __gc* tag);

			static void _drawSafe(System::Collections::Hashtable __gc* hash,int x, int y, CheckCallback __gc* ccb, Callback __gc* cb, Object __gc* tag)
			{
				System::Collections::ArrayList __gc* al = __try_cast<System::Collections::ArrayList __gc*>(hash->get_Item(__box(x)));
				if(al == 0)
				{
					al = __gc new System::Collections::ArrayList();
					al->Add(__box(y));
					hash->set_Item(__box(x),al);
				}
				else
				{
					if(al->Contains(__box(y)))
						return;
					else
						al->Add(__box(y));
				}

				if(ccb(x,y,tag))
				{

					cb(x,y,tag);
					_drawSafe(hash,x+1,y,ccb,cb,tag);
					_drawSafe(hash,x-1,y,ccb,cb,tag);
					_drawSafe(hash,x,y+1,ccb,cb,tag);
					_drawSafe(hash,x,y-1,ccb,cb,tag);
				}
			}
		
			static void drawSafe(int x, int y, CheckCallback __gc* ccb, Callback __gc* cb, Object __gc* tag)
			{
				System::Collections::Hashtable __gc* hash = __gc new System::Collections::Hashtable();
				_drawSafe(hash,x,y,ccb,cb,tag);

			}

			static void draw(int x, int y, CheckCallback __gc* ccb, Callback __gc* cb, Object __gc* tag)
			{
				if(ccb(x,y,tag))
				{
					cb(x,y,tag);
					draw(x+1,y,ccb,cb,tag);
					draw(x-1,y,ccb,cb,tag);
					draw(x,y+1,ccb,cb,tag);
					draw(x,y-1,ccb,cb,tag);
				}
			}
		};

		__gc class Ellipse
		{
		public:
			static void draw(int x0, int y0, int x1, int y1, Callback __gc* cb, Object __gc* tag)
			{

				int ymin = Math::Min(y0,y1);
				int ymax = Math::Max(y0,y1);
				int xmin = Math::Min(x0,x1);
				int xmax = Math::Max(x0,x1);

				int xc = xmin+(xmax-xmin)/2;
				int yc = ymin+(ymax-ymin)/2;

				int a = (xmax-xmin)/2;
				int b = (ymax-ymin)/2;
				int a2 = a*a;
				int b2 = b*b;
				int fa2 = 4*a2;

				if(a==0 || b==0)
					return;

				int x, y;
				int sigma;

				for(x=0, y=b, sigma=2*b2+a2*(1-2*b); b2*x <= a2*y; x++)
				{
					cb(xc+x,yc+y,tag);
					cb(xc-x,yc+y,tag);
					cb(xc+x,yc-y,tag);
					cb(xc-x,yc-y,tag);

					if(sigma>=0)
					{
						sigma += fa2*(1-y);
						y--;
					}
					sigma += b2*(4*x+6);
				}

				int fb2 = 4*b2;

				for(x=a, y=0, sigma=2*a2+b2*(1-2*a); a2*y <= b2*x; y++)
				{
					cb(xc+x,yc+y,__box(1));
					cb(xc-x,yc+y,__box(1));
					cb(xc+x,yc-y,__box(1));
					cb(xc-x,yc-y,__box(1));

					if(sigma>=0)
					{
						sigma += fb2*(1-x);
						x--;
					}
					sigma += a2*(4*y+6);
				}


			}
		};

		__gc class RectFill
		{
		public:
			static void draw(int x0, int y0, int x1, int y1, Callback __gc* cb, Object __gc* tag)
			{
				int ymin = Math::Min(y0,y1);
				int ymax = Math::Max(y0,y1);
				int xmin = Math::Min(x0,x1);
				int xmax = Math::Max(x0,x1);
				for(int y=ymin;y<=ymax;y++)
					for(int x=xmin;x<=xmax;x++)
						cb(x,y,tag);
			}
		};

	}; //primitives


} //namespace
