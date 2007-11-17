
namespace winmaped2 {
    static class BiosFont {
        public static byte[] Number(int num) {
            switch (num) {
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
                default: return null;
            }
        }

        const byte xx = 1;
        const byte __ = 0;

        /** everything below this line is (c) vecna **/
        static byte[] s1 =
        {
                    3,
            __,xx,__,
            xx,xx,__,
            __,xx,__,
            __,xx,__,
            __,xx,__,
            __,xx,__,
            xx,xx,xx
        };

        static byte[] s2 =
        {
            4,
            __,xx,xx,__,
            xx,__,__,xx,
            __,__,__,xx,
            __,__,__,xx,
            __,__,xx,__,
            __,xx,__,__,
            xx,xx,xx,xx
        };

        static byte[] s3 =
        {
                4,
            xx,xx,xx,xx,
            __,__,__,xx,
            __,__,__,xx,
            __,xx,xx,xx,
            __,__,__,xx,
            __,__,__,xx,
            xx,xx,xx,xx
        };

        static byte[] s4 =
        {
                4,
            xx,__,xx,__,
            xx,__,xx,__,
            xx,__,xx,__,
            xx,xx,xx,xx,
            __,__,xx,__,
            __,__,xx,__,
            __,__,xx,__
        };

        static byte[] s5 =
        {
                4,
            xx,xx,xx,xx,
            xx,__,__,__,
            xx,__,__,__,
            xx,xx,xx,__,
            __,__,__,xx,
            __,__,__,xx,
            xx,xx,xx,__
        };

        static byte[] s6 =
        {
                4,
            __,xx,xx,__,
            xx,__,__,xx,
            xx,__,__,__,
            xx,xx,xx,__,
            xx,__,__,xx,
            xx,__,__,xx,
            __,xx,xx,__
        };

        static byte[] s7 =
        {
                3,
            xx,xx,xx,
            __,__,xx,
            __,__,xx,
            __,xx,__,
            __,xx,__,
            __,xx,__,
            __,xx,__
        };

        static byte[] s8 =
        {
                4,
            __,xx,xx,__,
            xx,__,__,xx,
            xx,__,__,xx,
            __,xx,xx,__,
            xx,__,__,xx,
            xx,__,__,xx,
            __,xx,xx,__
        };

        static byte[] s9 = 
        {
                3,
            xx,xx,xx,
            xx,__,xx,
            xx,__,xx,
            xx,xx,xx,
            __,__,xx,
            __,__,xx,
            xx,xx,xx
        };

        static byte[] s0 =
        {
                3,
            xx,xx,xx,
            xx,__,xx,
            xx,__,xx,
            xx,__,xx,
            xx,__,xx,
            xx,__,xx,
            xx,xx,xx
        };

    }

}
