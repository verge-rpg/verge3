using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Drawing.Imaging;

namespace winmaped2.pr2 {
    public class LockedBitmap : RenderImage {
        public unsafe LockedBitmap(Bitmap bmp) {
            ImageLockMode imageLockMode = ImageLockMode.ReadWrite;
            this.bmp = bmp;
            bitmapData = bmp.LockBits(new Rectangle(0, 0, bmp.Width, bmp.Height), imageLockMode, PixelFormat.Format32bppArgb);

            buf = (int*)bitmapData.Scan0.ToPointer();
            width = bitmapData.Width;
            height = bitmapData.Height;
            stride = bitmapData.Stride;
            pitch = stride / 4;
        }

        public override void Dispose() {
            if (!isDisposed) {
                isDisposed = true;
                bmp.UnlockBits(bitmapData);
            }
        }

        private BitmapData bitmapData;
        private Bitmap bmp;
        private bool isDisposed = false;
    }
}
