#pragma once

namespace pr2
{
	__gc class BiosFont
	{
	public:
		static const byte xx = 1, zz = 0;
		static byte Number(int num) __gc[]
		{
			switch (num)
			{
				case 0: return s0; 
				case 1: return s1; 
				case 2: return s2; 
				case 3: return s3; 
				case 4: return s4; 
				case 5: return s5; 
				case 6: return s6; 
				case 7: return s7; 
				case 8: return s8; 
				case 9: return s9;
				default : return 0;
			}
		}

		/** everything below this line is (c) vecna **/
		static byte s1 __gc[] =
		{
					3,
			zz,xx,zz,
			xx,xx,zz,
			zz,xx,zz,
			zz,xx,zz,
			zz,xx,zz,
			zz,xx,zz,
			xx,xx,xx
		};

		static byte s2 __gc[] =
		{
			4,
			zz,xx,xx,zz,
			xx,zz,zz,xx,
			zz,zz,zz,xx,
			zz,zz,zz,xx,
			zz,zz,xx,zz,
			zz,xx,zz,zz,
			xx,xx,xx,xx
		};

		static byte s3 __gc[] =
		{
				4,
			xx,xx,xx,xx,
			zz,zz,zz,xx,
			zz,zz,zz,xx,
			zz,xx,xx,xx,
			zz,zz,zz,xx,
			zz,zz,zz,xx,
			xx,xx,xx,xx
		};

		static byte s4 __gc[] =
		{
				4,
			xx,zz,xx,zz,
			xx,zz,xx,zz,
			xx,zz,xx,zz,
			xx,xx,xx,xx,
			zz,zz,xx,zz,
			zz,zz,xx,zz,
			zz,zz,xx,zz
		};

		static byte s5 __gc[] =
		{
				4,
			xx,xx,xx,xx,
			xx,zz,zz,zz,
			xx,zz,zz,zz,
			xx,xx,xx,zz,
			zz,zz,zz,xx,
			zz,zz,zz,xx,
			xx,xx,xx,zz
		};

		static byte s6 __gc[] =
		{
				4,
			zz,xx,xx,zz,
			xx,zz,zz,xx,
			xx,zz,zz,zz,
			xx,xx,xx,zz,
			xx,zz,zz,xx,
			xx,zz,zz,xx,
			zz,xx,xx,zz
		};

		static byte s7 __gc[] =
		{
				3,
			xx,xx,xx,
			zz,zz,xx,
			zz,zz,xx,
			zz,xx,zz,
			zz,xx,zz,
			zz,xx,zz,
			zz,xx,zz
		};

		static byte s8 __gc[] =
		{
				4,
			zz,xx,xx,zz,
			xx,zz,zz,xx,
			xx,zz,zz,xx,
			zz,xx,xx,zz,
			xx,zz,zz,xx,
			xx,zz,zz,xx,
			zz,xx,xx,zz
		};

		static byte s9 __gc[] = 
		{
				3,
			xx,xx,xx,
			xx,zz,xx,
			xx,zz,xx,
			xx,xx,xx,
			zz,zz,xx,
			zz,zz,xx,
			xx,xx,xx
		};

		static byte s0 __gc[] =
		{
				3,
			xx,xx,xx,
			xx,zz,xx,
			xx,zz,xx,
			xx,zz,xx,
			xx,zz,xx,
			xx,zz,xx,
			xx,xx,xx
		};

	};
}
