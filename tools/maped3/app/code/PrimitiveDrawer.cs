using System;
using System.Collections;
using System.Text;

namespace winmaped2 {
    class PrimitiveDrawer {
        public delegate void Callback(int x, int y, object tag);
        public delegate bool CheckCallback(int x, int y, object tag);

        public static void DrawEllipse(int x0, int y0, int x1, int y1, Callback cb, object tag) {
            int ymin = Math.Min(y0, y1);
            int ymax = Math.Max(y0, y1);
            int xmin = Math.Min(x0, x1);
            int xmax = Math.Max(x0, x1);

            int xc = xmin + (xmax - xmin) / 2;
            int yc = ymin + (ymax - ymin) / 2;

            int a = (xmax - xmin) / 2;
            int b = (ymax - ymin) / 2;
            int a2 = a * a;
            int b2 = b * b;
            int fa2 = 4 * a2;

            if (a == 0 || b == 0)
                return;

            int x, y;
            int sigma;

            for (x = 0, y = b, sigma = 2 * b2 + a2 * (1 - 2 * b); b2 * x <= a2 * y; x++) {
                cb(xc + x, yc + y, tag);
                cb(xc - x, yc + y, tag);
                cb(xc + x, yc - y, tag);
                cb(xc - x, yc - y, tag);

                if (sigma >= 0) {
                    sigma += fa2 * (1 - y);
                    y--;
                }
                sigma += b2 * (4 * x + 6);
            }

            int fb2 = 4 * b2;

            for (x = a, y = 0, sigma = 2 * a2 + b2 * (1 - 2 * a); a2 * y <= b2 * x; y++) {
                cb(xc + x, yc + y, 1);
                cb(xc - x, yc + y, 1);
                cb(xc + x, yc - y, 1);
                cb(xc - x, yc - y, 1);

                if (sigma >= 0) {
                    sigma += fb2 * (1 - x);
                    x--;
                }
                sigma += a2 * (4 * y + 6);
            }
        }

        public static void DrawRectangle(int x0, int y0, int x1, int y1, Callback cb, object tag) {
            int ymin = Math.Min(y0, y1);
            int ymax = Math.Max(y0, y1);
            int xmin = Math.Min(x0, x1);
            int xmax = Math.Max(x0, x1);
            for (int y = ymin; y <= ymax; y++) {
                cb(xmin, y, tag);
                if (xmax - xmin > 0) {
                    cb(xmax, y, tag);
                }
            }
            for (int x = xmin + 1; x <= xmax - 1; x++) {
                cb(x, ymin, tag);
                if (ymax - ymin > 0) {
                    cb(x, ymax, tag);
                }
            }
        }
        public static void DrawFilledRectangle(int x0, int y0, int x1, int y1, Callback cb, object tag) {
            int ymin = Math.Min(y0, y1);
            int ymax = Math.Max(y0, y1);
            int xmin = Math.Min(x0, x1);
            int xmax = Math.Max(x0, x1);
            for (int y = ymin; y <= ymax; y++) {
                for (int x = xmin; x <= xmax; x++) {
                    cb(x, y, tag);
                }
            }
        }

        public static void DrawLine(int Ax, int Ay, int Bx, int By, Callback cb, object tag) {
            //------------------------------------------------------------------------
            // INITIALIZE THE COMPONENTS OF THE ALGORITHM THAT ARE NOT AFFECTED BY THE
            // SLOPE OR DIRECTION OF THE LINE
            //------------------------------------------------------------------------
            int dX = Math.Abs(Bx - Ax);	// store the change in X and Y of the line endpoints
            int dY = Math.Abs(By - Ay);

            //------------------------------------------------------------------------
            // DETERMINE "DIRECTIONS" TO INCREMENT X AND Y (REGARDLESS OF DECISION)
            //------------------------------------------------------------------------
            int Xincr, Yincr;
            if (Ax > Bx) { Xincr = -1; } else { Xincr = 1; }	// which direction in X?
            if (Ay > By) { Yincr = -1; } else { Yincr = 1; }	// which direction in Y?

            //------------------------------------------------------------------------
            // DETERMINE INDEPENDENT VARIABLE (ONE THAT ALWAYS INCREMENTS BY 1 (OR -1) )
            // AND INITIATE APPROPRIATE LINE DRAWING ROUTINE (BASED ON FIRST OCTANT
            // ALWAYS). THE X AND Y'S MAY BE FLIPPED IF Y IS THE INDEPENDENT VARIABLE.
            //------------------------------------------------------------------------
            if (dX >= dY) {                     // if X is the independent variable
                int dPr = dY << 1;              // amount to increment decision if right is chosen (always)
                int dPru = dPr - (dX << 1);     // amount to increment decision if up is chosen
                int P = dPr - dX;               // decision variable start value

                for (; dX >= 0; dX--) {         // process each point in the line one at a time (just use dX)
                    cb(Ax, Ay, tag);            // plot the pixel
                    if (P > 0) {                // is the pixel going right AND up?
                        Ax += Xincr;	        // increment independent variable
                        Ay += Yincr;            // increment dependent variable
                        P += dPru;              // increment decision (for up)
                    } else {                    // is the pixel just going right?
                        Ax += Xincr;            // increment independent variable
                        P += dPr;               // increment decision (for right)
                    }
                }
            } else {                            // if Y is the independent variable
                int dPr = dX << 1;              // amount to increment decision if right is chosen (always)
                int dPru = dPr - (dY << 1);     // amount to increment decision if up is chosen
                int P = dPr - dY;               // decision variable start value

                for (; dY >= 0; dY--) {         // process each point in the line one at a time (just use dY)
                    cb(Ax, Ay, tag);            // plot the pixel
                    if (P > 0) {                // is the pixel going up AND right?
                        Ax += Xincr;            // increment dependent variable
                        Ay += Yincr;            // increment independent variable
                        P += dPru;              // increment decision (for up)
                    } else {                    // is the pixel just going up?
                        Ay += Yincr;            // increment independent variable
                        P += dPr;               // increment decision (for right)
                    }
                }
            }
        }

        /*
         * FIXME: this algorithm shouldn't be recursive. -- andy 8 November 2007
         */
        static void FloodFill(Hashtable hash, int x, int y, CheckCallback ccb, Callback cb, object tag) {
            ArrayList al = hash[x] as ArrayList;
            if (al == null) {
                al = new ArrayList();
                al.Add(y);
                hash[x] = al;
            } else {
                if (al.Contains(y)) {
                    return;
                } else {
                    al.Add(y);
                }
            }

            if (ccb(x, y, tag)) {
                cb(x, y, tag);
                FloodFill(hash, x + 1, y, ccb, cb, tag);
                FloodFill(hash, x - 1, y, ccb, cb, tag);
                FloodFill(hash, x, y + 1, ccb, cb, tag);
                FloodFill(hash, x, y - 1, ccb, cb, tag);
            }
        }

        public static void FloodFill(int x, int y, CheckCallback ccb, Callback cb, object tag) {
            Hashtable hash = new Hashtable();
            FloodFill(hash, x, y, ccb, cb, tag);
        }
    }
}
