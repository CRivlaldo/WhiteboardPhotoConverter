#pragma once

namespace WPC
{
	class ImageBinarization
	{
	private:
		std::string inputFileName;
		FIBITMAP* pFIBitmap;
		PBITMAPINFO pFIBitmapInfo;

		ImageBinarization(const std::string& inputFileName, FIBITMAP* pFIBitmap, PBITMAPINFO pFIBitmapInfo);
		~ImageBinarization(void);

		double* CreateBrightnessMap(FIBITMAP* pFIB);
		double GetLocalLevel(const double* brightnessMap, LONG x, LONG y, int radius, double standardCoef);
		double GetMediumDeviation(const double* brightnessMap, LONG x, LONG y, int radius, int n, double medium);
		double GetStandardDeviation(int n, double mediumDeviation);
		double GetMedium(const double* brightnessMap, LONG x, LONG y, int radius, int& n);

		bool BinarizeAndSaveToBMP(const std::string& outputFileName);
		bool Binarize(BinaryBitmap* pOutBinaryBitmap);

	public:
		static bool BinarizeImageAndSaveToBMP(const std::string& inputFileName, FIBITMAP* pFIBitmap, 
			PBITMAPINFO pFIBitmapInfo, const std::string& outputFileName);
		static bool BinarizeImage(const std::string& inputFileName, FIBITMAP* pFIBitmap, 
			PBITMAPINFO pFIBitmapInfo, BinaryBitmap* pOutBinaryBitmap);
	};
}

