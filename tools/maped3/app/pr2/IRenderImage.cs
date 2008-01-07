using System;
namespace winmaped2.pr2 {
    public unsafe interface IRenderImage : IDisposable {
        void Clear(int color);
        IRenderImage Clone();
        void UpdatePixels(int[] newPixels);
        int[] GetArray();
        System.Drawing.Bitmap ConvertToBitmap();
        int GetPixel(int x, int y);
        void SetPixel(int x, int y, int color);
        int Height { get; }
        int Pitch { get; }
        int* Buffer { get; }
        int Stride { get; }
        int Width { get; }
    }
}
