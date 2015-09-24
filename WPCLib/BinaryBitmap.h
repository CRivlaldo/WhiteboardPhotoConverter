#pragma once

namespace WPC
{
	class BinaryBitmap
	{
	private:
		long width;
		long height;
		long blockSize;
		bool* pBits;
		unsigned char* pDIBits;

		void InitInternal(long width, long height, long blockSize);
		void ZeroBits();
		long GetMass(long x, long y, long dw, long dh) const;
		static void Copy(const BinaryBitmap& srcBitmap, BinaryBitmap& dstBitmap);
		void FillBlock(FIBITMAP* pBitmap, long x, long y, RGBQUAD& color);
		void FillDIBlock(long x, long y, unsigned char color);
	public:
		BinaryBitmap(long width, long height);
		BinaryBitmap(const BinaryBitmap& srcBitmap);
		BinaryBitmap(long width, long height, long blockSize);
		~BinaryBitmap(void);

		long GetWidth() const { return width; }
		long GetHeight() const { return height; }
		long GetDIBWidth() const { return width * blockSize; }
		long GetDIBHeight() const { return height * blockSize; }
		long GetBlockSize() const { return blockSize; }
		const void* BinaryBitmap::GetDIBBits() const { return (const void*)pDIBits; }
		void SetPixel(long x, long y) { SetPixel(x, y, true); }
		void SetPixel(long x, long y, bool light);
		void SetRect(const Rect& rect, bool light);
		bool GetPixel(long x, long y) const { return pBits[y * width + x]; }
		long GetPixelIndex(long x, long y) const { return y * width + x; }
		const BinaryBitmap Reduce(long blockSize, long criticalMass) const;
		const double GetMatch(const BinaryBitmap& binaryBitmap) const;
		int GetWeight(long x, long y) const;
		Rect GetRect() const { return Rect(0, 0, width, height); }
		BinaryBitmap Scale(double factor) const;

		void SaveToBMP(const std::string& fileName);

		BinaryBitmap& operator=(const BinaryBitmap& srcBitmap);
	};
}