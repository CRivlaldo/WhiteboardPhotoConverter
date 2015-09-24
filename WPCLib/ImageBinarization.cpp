#include "Prerequisites.h"

using namespace WPC;

bool DoColorPassLevel(double level, double color)
{
	return color <= level;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

ImageBinarization::ImageBinarization(const std::string& inputFileName, FIBITMAP* pFIBitmap, PBITMAPINFO pFIBitmapInfo)
{
	this->inputFileName = std::string(inputFileName);
	this->pFIBitmap = pFIBitmap;
	this->pFIBitmapInfo = pFIBitmapInfo;
	
	/*bitmapWidth = pFIBitmapInfo->bmiHeader.biHeight;
	bitmapHeight = pFIBitmapInfo->bmiHeader.biHeight;*/
}


ImageBinarization::~ImageBinarization(void)
{
	FreeImageUtils::UnloadBitmap(pFIBitmap);
}

bool ImageBinarization::BinarizeAndSaveToBMP(const std::string& outputFileName)
{
	BinaryBitmap* pBinaryBitmap = new BinaryBitmap(pFIBitmapInfo->bmiHeader.biWidth,
		pFIBitmapInfo->bmiHeader.biHeight);
	
	if(!Binarize(pBinaryBitmap))
	{
		delete pBinaryBitmap;
		return false;
	}

	pBinaryBitmap->SaveToBMP(outputFileName);
	delete pBinaryBitmap;

	return true;
}

bool ImageBinarization::Binarize(BinaryBitmap* pOutBinaryBitmap)
{
	double* brightnessMap = CreateBrightnessMap(pFIBitmap);

	RGBQUAD blackColor, whiteColor;

	blackColor.rgbRed = 0;
	blackColor.rgbGreen = 0;
	blackColor.rgbBlue = 0;

	whiteColor.rgbRed = 255;
	whiteColor.rgbGreen = 255;
	whiteColor.rgbBlue = 255;

	const int radius = 16;
	const double standardCoef = -0.1;

	for(LONG y = 0; y < pFIBitmapInfo->bmiHeader.biHeight; y++)
	{
		for(LONG x = 0; x < pFIBitmapInfo->bmiHeader.biWidth; x++)
		{
			double localLevel = GetLocalLevel(brightnessMap, x, y, radius, standardCoef);

			if(DoColorPassLevel(localLevel, brightnessMap[y * pFIBitmapInfo->bmiHeader.biWidth + x]))
				pOutBinaryBitmap->SetPixel(x, y);
		}
	}

	free(brightnessMap);

	return true;
}

double ImageBinarization::GetMedium(const double* brightnessMap, LONG x, LONG y, int radius, int& outPixelCount)
{
	double colorSum = 0;
	outPixelCount = 0;

	for(int dx = -radius; dx <= radius; dx++)
	{
		for(int dy = -radius; dy <= radius; dy++)
		{
			if(abs(dx) + abs(dy) > radius)
				continue;

			if(x + dx < 0 || y + dy < 0)
				continue;
			if(x + dx >= pFIBitmapInfo->bmiHeader.biWidth || y + dy >= pFIBitmapInfo->bmiHeader.biHeight)
				continue;
			
			colorSum += brightnessMap[(y + dy) * pFIBitmapInfo->bmiHeader.biWidth + x + dx];
			outPixelCount++;
		}
	}

	return colorSum / outPixelCount;
}

double ImageBinarization::GetMediumDeviation(const double* brightnessMap, LONG x, LONG y, int radius, int n, double medium)
{
	double mediumDevSum = 0;

	for(int dx = -radius; dx <= radius; dx++)
	{
		for(int dy = -radius; dy <= radius; dy++)
		{
			if(abs(dx) + abs(dy) > radius)
				continue;

			if(x + dx < 0 || y + dy < 0)
				continue;
			if(x + dx >= pFIBitmapInfo->bmiHeader.biWidth || y + dy >= pFIBitmapInfo->bmiHeader.biHeight)
				continue;

			double deviation = brightnessMap[(y + dy) * pFIBitmapInfo->bmiHeader.biWidth + x + dx] - medium;
			mediumDevSum += deviation * deviation;
		}
	}

	return sqrt(mediumDevSum / n);
}

double ImageBinarization::GetStandardDeviation(int n, double mediumDeviation)
{
	return sqrt(n * mediumDeviation * mediumDeviation / (n - 1));
}
		
/*
	Niblack method: 
	B(x, y)=mediumDev(x, y)+k*standradDev(x, y)
*/
double ImageBinarization::GetLocalLevel(const double* brightnessMap, LONG x, LONG y, int radius, double standardCoef)
{
	int n = 2 * radius * (radius + 1) + 1;
	double medium = GetMedium(brightnessMap, x, y, radius, n);
	double mediumDev = GetMediumDeviation(brightnessMap, x, y, radius, n, medium);
	double standardDev = GetStandardDeviation(n, mediumDev);
	//return medium + standardCoef * standardDev;
	return medium + standardCoef * sqrt(standardDev + medium * medium);
}

double ColorToBrighteness(const RGBQUAD& color)
{
	return (color.rgbRed + color.rgbGreen + color.rgbBlue) / (3.0 * 255);
}

double* ImageBinarization::CreateBrightnessMap(FIBITMAP* pFIB)
{
	double* brightnessMap = (double*) malloc(sizeof(double) * 
		pFIBitmapInfo->bmiHeader.biHeight * pFIBitmapInfo->bmiHeader.biWidth);

	for(LONG y = 0; y < pFIBitmapInfo->bmiHeader.biHeight; y++)
	{
		for(LONG x = 0; x < pFIBitmapInfo->bmiHeader.biWidth; x++)
		{
			RGBQUAD color;
			FreeImage_GetPixelColor(pFIB, x, y, &color);
			brightnessMap[y * pFIBitmapInfo->bmiHeader.biWidth + x] = ColorToBrighteness(color);
		}
	}

	return brightnessMap;
}
		
bool ImageBinarization::BinarizeImageAndSaveToBMP(const std::string& inputFileName, FIBITMAP* pFIBitmap, 
												  PBITMAPINFO pFIBitmapInfo, const std::string& outputFileName)
{
	ImageBinarization imageToBinarizedBMP(inputFileName, pFIBitmap, pFIBitmapInfo);
	return imageToBinarizedBMP.BinarizeAndSaveToBMP(outputFileName);
}
		
bool ImageBinarization::BinarizeImage(const std::string& inputFileName, FIBITMAP* pFIBitmap, 
									  PBITMAPINFO pFIBitmapInfo, BinaryBitmap* pOutBinaryBitmap)
{
	ImageBinarization imageToBinarizedBMP(inputFileName, pFIBitmap, pFIBitmapInfo);
	return imageToBinarizedBMP.Binarize(pOutBinaryBitmap);
}

//void ImageBinarization::Binarize(FIBITMAP* pBinarizedFIBitmap)
//{
//	double* brightnessMap = CreateBrightnessMap(pBinarizedFIBitmap);
//	
//	RGBQUAD blackColor, whiteColor;
//
//	blackColor.rgbRed = 0;
//	blackColor.rgbGreen = 0;
//	blackColor.rgbBlue = 0;
//
//	whiteColor.rgbRed = 255;
//	whiteColor.rgbGreen = 255;
//	whiteColor.rgbBlue = 255;
//
//	const int radius = 16;
//	const double standardCoef = -0.1;
//
//	for(LONG y = 0; y < pFIBitmapInfo->bmiHeader.biHeight; y++)
//	{
//		for(LONG x = 0; x < pFIBitmapInfo->bmiHeader.biWidth; x++)
//		{
//			double localLevel = GetLocalLevel(brightnessMap, x, y, radius, standardCoef);
//
//			if(IsColorPassLevel(localLevel, brightnessMap[y * pFIBitmapInfo->bmiHeader.biWidth + x]))
//				FreeImage_SetPixelColor(pBinarizedFIBitmap, x, y, &whiteColor);
//			else
//				FreeImage_SetPixelColor(pBinarizedFIBitmap, x, y, &blackColor);
//		}
//	}
//
//	free(brightnessMap);
//}