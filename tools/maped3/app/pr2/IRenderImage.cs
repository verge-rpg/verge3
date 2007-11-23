using System;
namespace winmaped2.pr2 {
    public interface IRenderImage : IDisposable {
        void Clear(int color);
        int[] GetArray();
        System.Drawing.Bitmap ConvertToBitmap();
        int GetPixel(int x, int y);
        int Height { get; }
        int Pitch { get; }
        unsafe int* Buffer { get; }
        int Stride { get; }
        int Width { get; }
    }
}
