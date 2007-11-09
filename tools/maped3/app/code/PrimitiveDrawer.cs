using System;
using System.Collections.Generic;
using System.Text;

namespace winmaped2 {
    class PrimitiveDrawer {
        public static void DrawRectangle(int x0, int y0, int x1, int y1, pr2.Primitives.Callback cb, object tag) {
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

        public static void DrawLine(int Ax, int Ay, int Bx, int By, pr2.Primitives.Callback cb, object tag) {
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
                        Ay += Yincr;         // increment independent variable
                        P += dPr;            // increment decision (for right)
                    }
                }
            }
        }

    }
}
