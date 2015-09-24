#include "Prerequisites.h"

using namespace WPC;

BinaryBitmap::BinaryBitmap(long width, long height)
{
	InitInternal(width, height, 1);
}

BinaryBitmap::BinaryBitmap(long width, long height, long blockSize)
{
	InitInternal(width, height, blockSize);
}

void BinaryBitmap::InitInternal(long width, long height, long blockSize)
{
	this->width = width;
	this->height = height;
	this->blockSize = blockSize;

	pBits = (bool*) malloc(width * height * sizeof(bool));
	pDIBits = (unsigned char*) malloc(3 * width * height * blockSize * blockSize * sizeof(unsigned char));

	ZeroBits();
}
	
BinaryBitmap::BinaryBitmap(const BinaryBitmap& srcBitmap)
{
	pBits = NULL;
	pDIBits = NULL;
	Copy(srcBitmap, *this);
}

BinaryBitmap::~BinaryBitmap(void)
{
	free(pDIBits);
	free(pBits);

	pDIBits = NULL;
	pBits = NULL;
}

void BinaryBitmap::ZeroBits()
{
	for(long i = 0; i < width * height; i++)
		pBits[i] = false;

	for(long i = 0; i < width * height * blockSize * blockSize; i++)
	{
		pDIBits[i * 3 + 0] = 0;
		pDIBits[i * 3 + 1] = 0;
		pDIBits[i * 3 + 2] = 0;
	}
}

void BinaryBitmap::FillBlock(FIBITMAP* pBitmap, long x, long y, RGBQUAD& color)
{
	for(long iY = y * blockSize; iY < (y + 1) * blockSize; iY++)
	{
		for(long iX = x * blockSize; iX < (x + 1) * blockSize; iX++)
		{
			FreeImage_SetPixelColor(pBitmap, x, y, &color);
		}
	}
}
	
void BinaryBitmap::FillDIBlock(long x, long y, unsigned char color)
{
	for(long iY = y * blockSize; iY < (y + 1) * blockSize; iY++)
	{
		for(long iX = x * blockSize; iX < (x + 1) * blockSize; iX++)
		{
			long pixelIndex = iY * width * blockSize + iX;
			pDIBits[pixelIndex * 3 + 0] = color;
			pDIBits[pixelIndex * 3 + 1] = color;
			pDIBits[pixelIndex * 3 + 2] = color;
		}
	}
}

void BinaryBitmap::SaveToBMP(const std::string& fileName)
{
	FIBITMAP* pBitmap = FreeImage_Allocate(width * blockSize, height * blockSize, 24);	
	
	RGBQUAD blackColor, whiteColor;

	blackColor.rgbRed = 0;
	blackColor.rgbGreen = 0;
	blackColor.rgbBlue = 0;

	whiteColor.rgbRed = 255;
	whiteColor.rgbGreen = 255;
	whiteColor.rgbBlue = 255;

	for(LONG y = 0; y < height; y++)
	{
		for(LONG x = 0; x < width; x++)
		{
			if(pBits[y * width + x])
				FillBlock(pBitmap, x, y, whiteColor);
			else
				FillBlock(pBitmap, x, y, blackColor);
		}
	}

	FreeImage_Save(FIF_BMP, pBitmap, fileName.c_str());
	FreeImage_Unload(pBitmap);
}
	
void BinaryBitmap::SetPixel(long x, long y, bool light) 
{ 
	long pixelIndex = width * y + x;
	unsigned char color = light ? 255 : 0;

	pBits[pixelIndex] = light; 
	FillDIBlock(x, y, color);
}

long BinaryBitmap::GetMass(long x, long y, long dw, long dh) const
{
	long mass = 0;

	for(long iY = y; iY < y + dh && iY < height; iY++)
	{
		for(long iX = x; iX < x + dw && iX < width; iX++)
		{
			long pixelIndex = iY * width + iX;
			if(pBits[pixelIndex])
				mass++;
		}
	}

	return mass;
}
	
const BinaryBitmap BinaryBitmap::Reduce(long reducedBlockSize, long criticalMass) const
{
	long reducedWidth = (width + reducedBlockSize - 1) / reducedBlockSize;
	long reducedHeight = (height + reducedBlockSize - 1) / reducedBlockSize;
	BinaryBitmap result(reducedWidth, reducedHeight, reducedBlockSize);

	for(long y = 0; y < reducedHeight; y++)
	{
		for(long x = 0; x < reducedWidth; x++)
		{
			long mass = GetMass(x * reducedBlockSize, y * reducedBlockSize, 
				reducedBlockSize, reducedBlockSize);

			if(mass >= criticalMass)
				result.SetPixel(x, y);
		}
	}

	return BinaryBitmap(result);
}

void BinaryBitmap::Copy(const BinaryBitmap& srcBitmap, BinaryBitmap& dstBitmap)
{
	dstBitmap.width = srcBitmap.width;
	dstBitmap.height = srcBitmap.height;
	dstBitmap.blockSize = srcBitmap.blockSize;

	if(dstBitmap.pBits)
		free(dstBitmap.pBits);

	if(dstBitmap.pDIBits)
		free(dstBitmap.pDIBits);

	long totalBits = srcBitmap.width * srcBitmap.height;
	long bitsSize = totalBits * sizeof(bool);
	long diBitsSize = totalBits * sizeof(unsigned char) * 3 * srcBitmap.blockSize * srcBitmap.blockSize;
	dstBitmap.pBits = (bool*)malloc(bitsSize);
	dstBitmap.pDIBits = (unsigned char*)malloc(diBitsSize);
	memcpy(dstBitmap.pBits, srcBitmap.pBits, bitsSize);
	memcpy(dstBitmap.pDIBits, srcBitmap.pDIBits, diBitsSize);
	//memcpy_s(dstBitmap.pBits, bitsSize, srcBitmap.pBits, sizeof(srcBitmap.pBits));
	//memcpy_s(dstBitmap.pDIBits, diBitsSize, srcBitmap.pDIBits, sizeof(srcBitmap.pDIBits));
}

BinaryBitmap& BinaryBitmap::operator=(const BinaryBitmap& srcBitmap)
{
	Copy(srcBitmap, *this);
	return *this;
}
		
const double BinaryBitmap::GetMatch(const BinaryBitmap& binaryBitmap) const
{
	if(binaryBitmap.width != width || binaryBitmap.height != height)
		throw new std::exception("Bitmap sizes are not equal");

	double coef = 0;
	//long lightPixelCount = 0;

	for(long x = 0; x < width; x++)
	{
		for(long y = 0; y < height; y++)
		{
			long pixelIndex = y * width + x;
			coef += (pBits[pixelIndex] == binaryBitmap.pBits[pixelIndex]) ? 1 : 0;
			//if(pBits[pixelIndex])
			//	lightPixelCount++;
		}
	}

	//return coef / lightPixelCount;
	return coef / (width * height);
}

int BinaryBitmap::GetWeight(long pointX, long pointY) const
{
	int weight = 0;

	for(long x = pointX - 1; x <= pointX + 1; x++)
	{
		if(x < 0 || x >= width)
			continue;

		for(long y = pointY - 1; y <= pointY + 1; y++)
		{
			if(y < 0 || y >= height)
				continue;

			if(x == pointX && y == pointY)
				continue;

			if(GetPixel(x, y))
			{
				weight += (x == pointX || y == pointY) ? 2 : 1;
			}
		}
	}

	return weight;
}

BinaryBitmap BinaryBitmap::Scale(double factor) const
{
	long newWidth = (long)(factor * width);
	long newHeight = (long)(factor * height);
	BinaryBitmap bitmap(newWidth, newHeight);

	for(long x = 0; x < newWidth; x++)
	{
		for(long y = 0; y < newHeight; y++)
		{
			long srcX = (long)(x / factor);
			long srcY = (long)(y / factor);

			//long srcX2 = srcX1 < width - 1 ? srcX1 + 1 : srcX1;
			//long srcY2 = srcY1 < height - 1 ? srcY1 + 1 : srcY1;

			int colorSum = 0;
			int colorCount = 0;
			for(long iSrcX = srcX; (iSrcX <= srcX + 1) && (iSrcX < width); iSrcX++)
			{
				for(long iSrcY = srcY; (iSrcY <= srcY + 1) && (iSrcY < height); iSrcY++)
				{
					if(GetPixel(iSrcX, iSrcY))
						colorSum++;

					colorCount++;
				}
			}

			if(colorSum * 2 / colorCount >= 1)
				bitmap.SetPixel(x, y);
		}
	}

	return bitmap;
}

void BinaryBitmap::SetRect(const Rect& rect, bool light)
{
	for(long x = rect.origin.x; x < rect.origin.x + rect.width; x++)
	{
		for(long y = rect.origin.y; y < rect.origin.y + rect.height; y++)
			SetPixel(x, y, false);
	}
}