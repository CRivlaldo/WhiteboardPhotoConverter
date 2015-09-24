#include "allheaders.h"
#include "baseapi.h"
#include "strngs.h"

#include "Prerequisites.h"
#include "TextRecognizer.h"

using namespace WPC;

void FilterByMedian(long size, long* pValues, int radius, int throws)
{
	long* pBuffer = (long*)malloc(size * sizeof(long));
	memcpy(pBuffer, pValues, sizeof(long) * size);

	for(int k = 0; k < throws; k++)
	{
		for(long i = 0; i < size; i++)
		{
			std::vector<long> frequences;
			for(long j = i - radius; j <= i + radius; j++)
			{
				if(j < 0 || j > size)
					continue;

				frequences.push_back(pValues[j]);
			}

			std::sort(frequences.begin(), frequences.end());
			long median;
			if(frequences.size() % 2 == 0)
			{
				median = (frequences[frequences.size() / 2 - 1] + 
					frequences[frequences.size() / 2]) / 2;
			}
			else
				median = frequences[frequences.size() / 2];

			pBuffer[i] = median;
		}
		memcpy(pValues, pBuffer, sizeof(long) * size);
	}

	free(pBuffer);
}

void FindLevels(long size, long* pValues, long level, std::list<std::pair<long, long>>& outLanes)
{
	long laneStart = -1;
	for(long i = 0; i < size; i++)
	{
		if(laneStart == -1)
		{
			if(pValues[i] >= level)
			{
				laneStart = i;
			}
		}
		else
		{
			if(pValues[i] < level)
			{
				outLanes.push_back(std::pair<long, long>(laneStart, i));
				laneStart = -1;
			}
		}
	}

	if(laneStart != -1)
	{
		outLanes.push_back(std::pair<long, long>(laneStart, size - 1));
	}
}

void PushLevelToList(long start, long end, long minGap, std::list<std::pair<long, long>>& outLanes)
{
	long gap = -1;
	if(!outLanes.empty())
	{
		gap = start - outLanes.rbegin()->second - 1;
	}

	if(gap >= 0 && gap < minGap)
	{
		outLanes.rbegin()->second = end;
		//std::pair<long, long> lastLevel = *outLanes.rbegin();
		//lastLevel.second = last;
					
		//outLanes.pop_back();
		//outLanes.push_back(lastLevel);
	}
	else
		outLanes.push_back(std::pair<long, long>(start, end));
}

void FindLevels(long size, long* pValues, long level, long minGap, 
				std::list<std::pair<long, long>>& outLanes)
{
	long laneStart = -1;
	for(long i = 0; i < size; i++)
	{
		if(laneStart == -1)
		{
			if(pValues[i] >= level)
			{
				laneStart = i;
			}
		}
		else
		{
			if(pValues[i] < level)
			{
				PushLevelToList(laneStart, i, minGap, outLanes);
				laneStart = -1;
			}
		}
	}

	if(laneStart != -1)
	{
		PushLevelToList(laneStart, size - 1, minGap, outLanes);
	}
}

void DetectEdge(BinaryBitmap* pBitmap)
{
	BinaryBitmap* pBitmapCopy = new BinaryBitmap(pBitmap->GetWidth(), pBitmap->GetHeight());
	
	for(long x = 0; x < pBitmap->GetWidth(); x++)
	{
		for(long y = 0; y < pBitmap->GetHeight(); y++)
		{
			if(!pBitmap->GetPixel(x, y))
				continue;

			bool detected = false;

			for(long dx = x - 1; dx <= x+1 && !detected; dx++)
			{
				if(dx < 0 || dx >= pBitmap->GetWidth())
					continue;
								
				for(long dy = y - 1; dy <= y+1 && !detected; dy++)
				{
					if(dy < 0 || dy >= pBitmap->GetHeight())
						continue;

					if(dx == x && dy == y)
						continue;

					if(!pBitmap->GetPixel(dx, dy))
						detected = true;
				}
			}

			pBitmapCopy->SetPixel(x, y, detected);
		}
	}

	*pBitmap = *pBitmapCopy;//!!!do we need copying???
	delete pBitmapCopy;
}

//bool IsPointInDiamond(long x, long y, long width, long height)
//{
//	double kx = abs((x - width / 2.0) / width * 2.0);
//	double ky = abs((y - height / 2.0) / height * 2.0);
//	return kx + ky <= 1;
//}

void Dilation(BinaryBitmap* pBitmap, long elementWidth, long elementHeight)
{
	//!!!!E-quantor
	BinaryBitmap* pBitmapCopy = new BinaryBitmap(pBitmap->GetWidth(), pBitmap->GetHeight());
	
	for(long x = 0; x < pBitmap->GetWidth(); x++)
	{
		for(long y = 0; y < pBitmap->GetHeight(); y++)
		{
			long sum = 0;
			/*for(long ex = 0; ex < elementWidth; ex++)
			{
				long dx = x - elementWidth / 2 + ex;
				if(dx < 0 || dx >= pBitmap->GetWidth())
					continue;

				for(long ey = 0; ey < elementHeight; ey++)
				{
					long dy = y - elementHeight / 2 + ey;
					if(dy < 0 || dy >= pBitmap->GetHeight())
						continue;

					if(!IsPointInDiamond(ex, ey, elementWidth, elementHeight))
						continue;

					if(pBitmap->GetPixel(dx, dy))
						sum++;
				}
			}*/

			for(long ex = 0; ex < elementWidth; ex++)
			{
				long dx = x - elementWidth / 2 + ex;
				if(dx < 0 || dx >= pBitmap->GetWidth())
					continue;

				if(pBitmap->GetPixel(dx, y))
					sum++;
			}

			for(long ey = 0; ey < elementHeight; ey++)
			{
				long dy = y - elementHeight / 2 + ey;
				if(dy < 0 || dy >= pBitmap->GetHeight())
					continue;
				
				if(pBitmap->GetPixel(x, dy))
					sum++;
			}

			if(sum > 0)
				pBitmapCopy->SetPixel(x, y);
		}
	}

	*pBitmap = *pBitmapCopy;//!!!do we need copying???
	delete pBitmapCopy;
}

void Erasion(BinaryBitmap* pBitmap, long elementWidth, long elementHeight)
{
	//!!!!E-quantor
	BinaryBitmap* pBitmapCopy = new BinaryBitmap(pBitmap->GetWidth(), pBitmap->GetHeight());
	
	for(long x = 0; x < pBitmap->GetWidth(); x++)
	{
		for(long y = 0; y < pBitmap->GetHeight(); y++)
		{
			long sum = 0;
			/*for(long ex = 0; ex < elementWidth; ex++)
			{
				long dx = x - elementWidth / 2 + ex;
				if(dx < 0 || dx >= pBitmap->GetWidth())
					continue;

				for(long ey = 0; ey < elementHeight; ey++)
				{
					long dy = y - elementHeight / 2 + ey;
					if(dy < 0 || dy >= pBitmap->GetHeight())
						continue;

					if(!IsPointInDiamond(ex, ey, elementWidth, elementHeight))
						continue;

					if(!pBitmap->GetPixel(dx, dy))
						sum++;
				}
			}*/

			for(long ex = 0; ex < elementWidth; ex++)
			{
				long dx = x - elementWidth / 2 + ex;
				if(dx < 0 || dx >= pBitmap->GetWidth())
					continue;

				if(!pBitmap->GetPixel(dx, y))
					sum++;
			}

			for(long ey = 0; ey < elementHeight; ey++)
			{
				long dy = y - elementHeight / 2 + ey;
				if(dy < 0 || dy >= pBitmap->GetHeight())
					continue;
				
				if(!pBitmap->GetPixel(x, dy))
					sum++;
			}

			if(sum == 0)
				pBitmapCopy->SetPixel(x, y);
		}
	}

	*pBitmap = *pBitmapCopy;//!!!do we need copying???
	delete pBitmapCopy;
}

void Opening(BinaryBitmap* pBitmap, long elementWidth, long elementHeight)
{
	Erasion(pBitmap, elementWidth, elementHeight);
	Dilation(pBitmap, elementWidth, elementHeight);
}

bool CutRim(TextGlyph& text)
{
	//!!!maybe we should recognize text until rim isn't cuted????
	
	//text.text = StringUtils::Trim(text.text);
	std::string trimChars = " \n\r";
	long cutWidth = text.rect.height;// / 2;
	std::string rimChars = "\\/|[]()";
	
	while(text.text.length() > 0)
	{
		bool found = false;
		for(size_t i = 0; i < trimChars.length(); i++)
		{
			if(text.text[0] == trimChars[i])
			{
				text.text.erase(0, 1);

				if(text.text[0] == ' ')
				{
					text.rect.origin.x += cutWidth;
					text.rect.width -= cutWidth;
				}

				found = true;
			}
		}

		if(!found)
			break;
	}

	while(text.text.length() > 0)
	{
		bool found = false;
		for(size_t i = 0; i < trimChars.length(); i++)
		{
			size_t lastCharIndex = text.text.length() - 1;
			if(text.text[lastCharIndex] == trimChars[i])
			{
				text.text.erase(lastCharIndex, 1);

				if(text.text[lastCharIndex] == ' ')
					text.rect.width -= cutWidth;

				found = true;
			}
		}

		if(!found)
			break;
	}
	
	if(text.text.length() > 0)
	{
		for(size_t i = 0; i < rimChars.length(); i++)
		{
			if(text.text[0] == rimChars[i])
			{
				text.text.erase(0, 1);

				text.rect.origin.x += cutWidth;
				text.rect.width -= cutWidth;

				break;
			}
		}
	}
	
	if(text.text.length() > 0)
	{
		size_t lastCharIndex = text.text.length() - 1;
		for(size_t i = 0; i < rimChars.length(); i++)
		{
			if(text.text[lastCharIndex] == rimChars[i])
			{
				text.text.erase(lastCharIndex, 1);
				text.rect.width -= cutWidth;
				break;
			}
		}
	}
	
	while(text.text.length() > 0)
	{
		bool found = false;
		for(size_t i = 0; i < trimChars.length(); i++)
		{
			if(text.text[0] == trimChars[i])
			{
				text.text.erase(0, 1);

				if(text.text[0] == ' ')
				{
					text.rect.origin.x += cutWidth;
					text.rect.width -= cutWidth;
				}

				found = true;
			}
		}

		if(!found)
			break;
	}

	while(text.text.length() > 0)
	{
		bool found = false;
		for(size_t i = 0; i < trimChars.length(); i++)
		{
			size_t lastCharIndex = text.text.length() - 1;
			if(text.text[lastCharIndex] == trimChars[i])
			{
				text.text.erase(lastCharIndex, 1);

				if(text.text[lastCharIndex] == ' ')
					text.rect.width -= cutWidth;

				found = true;
			}
		}

		if(!found)
			break;
	}

	//text.text = StringUtils::Trim(text.text);

	return (text.text.length() > 0) && (text.rect.width > 0);
}

//bool CutRimFromText(std::string& text)
//{
//	std::string trimChars = " \n\r";
//	std::string rimChars = "\\/|[]()";
//	
//	while(text.length() > 0)
//	{
//		bool found = false;
//		for(size_t i = 0; i < trimChars.length(); i++)
//		{
//			if(text[0] == trimChars[i])
//			{
//				text.erase(0, 1);
//				found = true;
//			}
//		}
//
//		if(!found)
//			break;
//	}
//
//	while(text.length() > 0)
//	{
//		bool found = false;
//		for(size_t i = 0; i < trimChars.length(); i++)
//		{
//			size_t lastCharIndex = text.length() - 1;
//			if(text[lastCharIndex] == trimChars[i])
//			{
//				text.erase(lastCharIndex, 1);
//				found = true;
//			}
//		}
//
//		if(!found)
//			break;
//	}
//	
//	if(text.length() > 0)
//	{
//		for(size_t i = 0; i < rimChars.length(); i++)
//		{
//			if(text[0] == rimChars[i])
//			{
//				text.erase(0, 1);
//				break;
//			}
//		}
//	}
//	
//	if(text.length() > 0)
//	{
//		size_t lastCharIndex = text.length() - 1;
//		for(size_t i = 0; i < rimChars.length(); i++)
//		{
//			if(text[lastCharIndex] == rimChars[i])
//			{
//				text.erase(lastCharIndex, 1);
//				break;
//			}
//		}
//	}
//	
//	while(text.length() > 0)
//	{
//		bool found = false;
//		for(size_t i = 0; i < trimChars.length(); i++)
//		{
//			if(text[0] == trimChars[i])
//			{
//				text.erase(0, 1);
//				found = true;
//			}
//		}
//
//		if(!found)
//			break;
//	}
//
//	while(text.length() > 0)
//	{
//		bool found = false;
//		for(size_t i = 0; i < trimChars.length(); i++)
//		{
//			size_t lastCharIndex = text.length() - 1;
//			if(text[lastCharIndex] == trimChars[i])
//			{
//				text.erase(lastCharIndex, 1);
//				found = true;
//			}
//		}
//
//		if(!found)
//			break;
//	}
//
//	//text.text = StringUtils::Trim(text.text);
//
//	return text.length() > 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

TextRecognizer::TextRecognizer(BinaryBitmap* pBinaryBitmap,	Debugger* pDebugger)
{
	this->pBinaryBitmap = pBinaryBitmap;
	this->pDebugger = pDebugger;

	pAPI = new tesseract::TessBaseAPI();
}

TextRecognizer::~TextRecognizer(void)
{
}

//void TextRecognizer::FindTextAreas()
//{
//	textAreas.clear();
//
//	long width = pBinaryBitmap->GetDIBWidth();
//	long height = pBinaryBitmap->GetDIBHeight();
//
//	long horizontalLevel = 5;//width / 17;///!!!!magic constant
//	long minGap = 10;///!!!!magic constant
//
//	long* pHorizontalPixels = (long*)malloc(height * sizeof(long));
//	long* pVerticalPixels = (long*)malloc(width * sizeof(long));
//
//	ZeroMemory(pHorizontalPixels, sizeof(long) * height);
//
//	for(long x = 0; x < width; x++)
//	{
//		for(long y = 0; y < height; y++)
//		{
//			if(pBinaryBitmap->GetPixel(x, y))
//			{
//				pHorizontalPixels[y]++;
//			}
//		}
//	}
//	
//	FilterByMedian(height, pHorizontalPixels, 2, 4);
//
//	std::list<std::pair<long, long>> horizontalLanes;
//	FindLevels(height, pHorizontalPixels, horizontalLevel, horizontalLanes);
//
//	std::list<std::pair<long, long>>::const_iterator horizontalIt;
//	for(horizontalIt = horizontalLanes.begin(); horizontalIt != horizontalLanes.end(); horizontalIt++)
//	{
//		if(horizontalIt->second - horizontalIt->first < 5)
//			continue;
//		
//		//long verticalLevel = (long)((horizontalIt->second - horizontalIt->first) / 17.0 + .5);///!!!!magic constant
//		long verticalLevel = 1;///!!!!magic constant
//
//		ZeroMemory(pVerticalPixels, sizeof(long) * width);
//
//		long startY = std::max<long>(0, horizontalIt->first - 3);
//		long endY = std::min<long>(horizontalIt->second + 3, height - 1);
//		for(long x = 0; x < width; x++)
//		{
//			for(long y = startY; y <= endY; y++)
//			{
//				if(pBinaryBitmap->GetPixel(x, y))
//				{
//					pVerticalPixels[x]++;
//				}
//			}
//		}
//				
//		FilterByMedian(width, pVerticalPixels, 2, 4);
//
//		std::list<std::pair<long, long>> verticalLanes;
//		FindLevels(width, pVerticalPixels, verticalLevel, minGap, verticalLanes);
//
//		std::list<std::pair<long, long>>::const_iterator verticalIt;
//		for(verticalIt = verticalLanes.begin(); verticalIt != verticalLanes.end(); verticalIt++)
//		{
//			if(verticalIt->second - verticalIt->first < 5)
//				continue;
//			
//			/*Rect textAreaRect(Point(verticalIt->first, horizontalIt->first), 
//				Point(verticalIt->second, horizontalIt->second));*///!!!!
//			Rect textAreaRect(Point(verticalIt->first, horizontalIt->first) - Point(5, 5), 
//				Point(verticalIt->second, horizontalIt->second) + Point(5, 5));
//			textAreaRect.CropBy(pBinaryBitmap->GetRect());
//
//			textAreas.push_back(textAreaRect);
//		}
//	}
//
//	///!!!debug
//	{
//		FIBITMAP* pBitmap = FreeImage_Allocate(width, height, 24);	
//	
//		//for(LONG y = 0; y < height; y++)
//		//{
//		//	for(LONG x = 0; x < width; x++)
//		//	{
//		//		double frequencySum = pHorizontalPixels[y] / (double)width;
//
//		//		RGBQUAD color;
//		//		{
//		//			color.rgbRed = (BYTE)(frequencySum * 255);
//		//			color.rgbGreen = (BYTE)(frequencySum * 255);
//		//			color.rgbBlue = (BYTE)(frequencySum * 255);
//		//		}
//
//		//		FreeImage_SetPixelColor(pBitmap, x, y, &color);
//		//	}
//		//}
//
//		std::list<std::pair<long, long>>::const_iterator horizontalIt;
//		for(horizontalIt = horizontalLanes.begin(); horizontalIt != horizontalLanes.end(); horizontalIt++)
//		{
//			if(horizontalIt->second - horizontalIt->first < 5)
//				continue;
//
//			ZeroMemory(pVerticalPixels, sizeof(long) * width);
//
//			for(long x = 0; x < width; x++)
//			{
//				for(long y = horizontalIt->first; y <= horizontalIt->second; y++)
//				{
//					if(pBinaryBitmap->GetPixel(x, y))
//					{
//						pVerticalPixels[x]++;
//					}
//				}
//			}
//				
//			FilterByMedian(width, pVerticalPixels, 2, 4);
//			
//			for(long x = 0; x < width; x++)
//			{
//				for(long y = horizontalIt->first; y <= horizontalIt->second; y++)
//				{
//					double frequencySum = pVerticalPixels[x] / (double)(horizontalIt->second - horizontalIt->first + 1);
//
//					RGBQUAD color;
//					{
//						color.rgbRed = (BYTE)(frequencySum * 255);
//						color.rgbGreen = (BYTE)(frequencySum * 255);
//						color.rgbBlue = (BYTE)(frequencySum * 255);
//					}
//
//					FreeImage_SetPixelColor(pBitmap, x, y, &color);
//				}
//			}
//				
//			//FilterByMedian(width, pVerticalPixels, 8, 8);
//
//		}
//
//		FreeImage_Save(FIF_BMP, pBitmap, "D:\\dev\\WhiteboardPhotoConverter\\Release\\freqV.bmp");
//		FreeImage_Unload(pBitmap);
//	}
//
//	free(pHorizontalPixels);
//	free(pVerticalPixels);
//}

void TextRecognizer::ConnectedComponentAnalisys(BinaryBitmap* pBitmap, long lowerThreshold, 
												long higherThreshold, double factor)
{
	///!!!move component recognizer somewhere
	QuickUnion connectedComponents(pBitmap->GetWidth() * pBitmap->GetHeight() + 1);
	long backgroundIndex = pBitmap->GetWidth() * pBitmap->GetHeight();

	for(long y = 0; y < pBitmap->GetHeight(); y++)
	{
		if(pBitmap->GetWidth() == 0)
			break;
			
		if(!pBitmap->GetPixel(0, y))
			connectedComponents.Union(pBitmap->GetPixelIndex(0, y), backgroundIndex);
		
		for(long x = 1; x < pBitmap->GetWidth(); x++)
		{
			long pixelIndex = pBitmap->GetPixelIndex(x, y);

			if(pBitmap->GetPixel(x, y))
			{
				//if(y > 0) //????this one already checked
				//{
				//	if(pBitmap->GetPixel(x - 1, y - 1))
				//		connectedComponents.Union(pBitmap->GetPixelIndex(x - 1, y - 1), pixelIndex);
				//}
				
				if(pBitmap->GetPixel(x - 1, y))
					connectedComponents.Union(pBitmap->GetPixelIndex(x - 1, y), pixelIndex);

		/*		if(y < pBitmap->GetHeight() - 1)
				{
					if(pBitmap->GetPixel(x - 1, y + 1))
						connectedComponents.Union(pBitmap->GetPixelIndex(x - 1, y + 1), pixelIndex);
				}*/
			}
			else
				connectedComponents.Union(pixelIndex, backgroundIndex);
		}
	}

	for(long x = 0; x < pBitmap->GetWidth(); x++)
	{
		if(pBitmap->GetHeight() == 0)
			break;

		for(long y = 1; y < pBitmap->GetHeight(); y++)
		{
			long pixelIndex = pBitmap->GetPixelIndex(x, y);

			if(pBitmap->GetPixel(x, y))
			{
	/*			if(x > 0)
				{
					if(pBitmap->GetPixel(x - 1, y - 1))
						connectedComponents.Union(pBitmap->GetPixelIndex(x - 1, y - 1), pixelIndex);
				}*/
				
				if(pBitmap->GetPixel(x, y - 1))
					connectedComponents.Union(pBitmap->GetPixelIndex(x, y - 1), pixelIndex);

		/*		if(x < pBitmap->GetWidth() - 1)
				{
					if(pBitmap->GetPixel(x + 1, y - 1))
						connectedComponents.Union(pBitmap->GetPixelIndex(x + 1, y - 1), pixelIndex);
				}*/
			}
		}
	}

	printf("Total text components: %d\n", connectedComponents.GetUnionCount());//!!!!debug
	for(long y = 0; y < pBitmap->GetHeight(); y++)
	{
		for(long x = 0; x < pBitmap->GetWidth(); x++)
		{
			long pixelIndex = pBitmap->GetPixelIndex(x, y);
			if(connectedComponents.HasConnection(pixelIndex, backgroundIndex))
				continue;

			Chunk chunk = Chunk::CreateChunkFromUnion(pBitmap, connectedComponents, pixelIndex);

			if(chunk.GetHeight() >= lowerThreshold && chunk.GetHeight() <= higherThreshold && 
				chunk.GetWidth() * 1.0 / chunk.GetHeight() > 1.5)
			{
				Rect rect = chunk.GetRegion();
				textAreas.push_back(Rect(rect.GetMin() / factor, rect.GetMax() / factor));
			}

			connectedComponents.Union(pixelIndex, backgroundIndex);
		}
	}
}

void TextRecognizer::FindTextAreas()
{
	double factor = 1;
	while(true)
	{
		if(pBinaryBitmap->GetWidth() * factor < 12 || pBinaryBitmap->GetHeight() * factor < 12)
			break;

		BinaryBitmap bitmap = pBinaryBitmap->Scale(factor);

		DetectEdge(&bitmap);
		//if(pDebugger)
		//{
		//	pDebugger->ShowBinaryImage(&bitmap);
		//	pDebugger->WaitForContinue();
		//}

		Dilation(&bitmap, 21, 5);
		//if(pDebugger)
		//{
		//	pDebugger->ShowBinaryImage(&bitmap);
		//	pDebugger->WaitForContinue();
		//}

		Opening(&bitmap, 45, 11);
	/*	if(pDebugger)
		{
			pDebugger->ShowBinaryImage(&bitmap);
			pDebugger->WaitForContinue();
		}*/

		ConnectedComponentAnalisys(&bitmap, 12, 48, factor);

		factor /= 2.0;
	}
}

PIX* TextRecognizer::CreateImageByBinaryBitmap()
{
	Pix* pImage = pixCreate(pBinaryBitmap->GetWidth(), pBinaryBitmap->GetHeight(), 1);

	unsigned int *pData = pixGetData(pImage);
	int wpl = pixGetWpl(pImage);
    int bpl = (1 * pBinaryBitmap->GetWidth() + 7) / 8;
    unsigned int *pLine;
    unsigned char val8;

    for(int y = 0; y < pBinaryBitmap->GetHeight(); y++) 
	{
		pLine = pData + y * wpl;
		for(int dx = 0; dx < bpl; dx++)
		{
			val8 = 0;
			for(int shift = 0; shift < 8; shift++)
			{
				int x = dx * 8 + shift;
				val8 *= 2;
				if(x < pBinaryBitmap->GetWidth())
				{
					if(pBinaryBitmap->GetPixel(x, pBinaryBitmap->GetHeight() - y - 1))
						val8 += 1;
				}
			}
			SET_DATA_BYTE(pLine, dx, val8);
		}
    }

	return pImage;
}

//PIX* TextRecognizer::CreateImageByBinaryBitmap(const Rect& rect)///!!!debug
//{
//	Pix* pImage = pixCreate(pBinaryBitmap->GetWidth(), pBinaryBitmap->GetHeight(), 1);
//
//	unsigned int *pData = pixGetData(pImage);
//	int wpl = pixGetWpl(pImage);
//	int bpl = (1 * rect.width + 7) / 8;
//    unsigned int *pLine;
//    unsigned char val8;
//
//	for(int y = 0; y < rect.height; y++) 
//	{
//		pLine = pData + y * wpl;
//		for(int dx = 0; dx < bpl; dx++)
//		{
//			val8 = 0;
//			for(int shift = 0; shift < 8; shift++)
//			{
//				int x = dx * 8 + shift;
//				val8 *= 2;
//				if(x < rect.width)
//				{
//					if(pBinaryBitmap->GetPixel(x + rect.origin.x, rect.origin.y + rect.height - 1 - y))
//					{
//						val8 += 1;
//					}
//				}
//			}
//			SET_DATA_BYTE(pLine, dx, val8);
//		}
//    }
//
//	return pImage;
//}

bool TextRecognizer::ReconginzeTextBlocks(std::string& errorText)
{
	std::list<Rect>::const_iterator textAreasIterator;
	for(textAreasIterator = textAreas.begin(); textAreasIterator != textAreas.end(); textAreasIterator++)
	{
		//pAPI->SetRectangle(textAreasIterator->origin.x, pBinaryBitmap->GetHeight() - 
		//	textAreasIterator->origin.y - textAreasIterator->height, textAreasIterator->width, 
		//	textAreasIterator->height);
		//char* ocrResult = pAPI->GetUTF8Text();
		//int confidence = pAPI->MeanTextConf();
		////printf("Rect: x=%d, y=%d, w=%d, h=%d, confidence: %d, text: %s\n",
		////	textAreasIterator->origin.x, pBinaryBitmap->GetHeight() - textAreasIterator->origin.y -
		////	textAreasIterator->height, textAreasIterator->width, textAreasIterator->height, 
		////	confidence, ocrResult);//!!!!debug

		//if(!ocrResult)
		//	continue;

		int confidence;
		std::string recognizedText = RecognizeTextFromRect(*textAreasIterator, confidence);
		if(recognizedText == "")
			continue;

		textGlyphs.push_back(TextGlyph(*textAreasIterator, recognizedText, confidence));
		//delete[] ocrResult;
	}

	return true;
}
		
std::string TextRecognizer::RecognizeTextFromRect(const Rect& rect, int& confidence)
{
	pAPI->SetRectangle(rect.origin.x, pBinaryBitmap->GetHeight() - rect.origin.y - rect.height, 
		rect.width, rect.height);
	char* ocrResult = pAPI->GetUTF8Text();
	confidence = pAPI->MeanTextConf();

	if(!ocrResult)
		return std::string("");

	std::string result(ocrResult);
	//delete[] ocrResult;

	return result;
}

std::string TextRecognizer::RecognizeTextFromRect(const Rect& rect)
{
	pAPI->SetRectangle(rect.origin.x, pBinaryBitmap->GetHeight() - rect.origin.y - rect.height, 
		rect.width, rect.height);
	char* ocrResult = pAPI->GetUTF8Text();

	if(!ocrResult)
		return std::string("");

	std::string result(ocrResult);
	//delete[] ocrResult;

	return result;
}

//bool TextRecognizer::CutRim(TextGlyph& text)
//{
//	if(!CutRimFromText(text.text))
//		return false;
//
//	///!!!дихотомически бы
//	while(true)
//	{		
//		Rect candidateRect(text.rect.GetMin(), text.rect.GetMax() - Point(2, 0));
//		std::string recognizedText = RecognizeTextFromRect(candidateRect);
//
//		if(!CutRimFromText(recognizedText))
//			break;
//
//		if(recognizedText == text.text)
//			text.rect.width -= 2;
//		else
//			break;
//	}
//
//	while(true)
//	{		
//		Rect candidateRect(text.rect.GetMin() + Point(2, 0), text.rect.GetMax());
//		std::string recognizedText = RecognizeTextFromRect(candidateRect);
//		
//		if(!CutRimFromText(recognizedText))
//			break;
//
//		if(recognizedText == text.text)
//		{
//			text.rect.origin.x += 2;
//			text.rect.width -= 2;
//		}
//		else
//			break;
//	}
//
//	while(true)
//	{		
//		Rect candidateRect(text.rect.GetMin(), text.rect.GetMax() - Point(0, 2));
//		std::string recognizedText = RecognizeTextFromRect(candidateRect);
//		
//		if(!CutRimFromText(recognizedText))
//			break;
//
//		if(recognizedText == text.text)
//			text.rect.height -= 2;
//		else
//			break;
//	}
//
//	while(true)
//	{		
//		Rect candidateRect(text.rect.GetMin() + Point(0, 2), text.rect.GetMax());
//		std::string recognizedText = RecognizeTextFromRect(candidateRect);
//
//		if(!CutRimFromText(recognizedText))
//			break;
//
//		if(recognizedText == text.text)
//		{
//			text.rect.origin.y += 2;
//			text.rect.height -= 2;
//		}
//		else
//			break;
//	}
//
//	return (text.rect.width > 0);
//}
		
void TextRecognizer::RemoveTextIntersections(std::list<TextGlyph>& outTextGlyphs)
{	
	textGlyphs.sort(&CompareConfidences);

	while(!textGlyphs.empty())
	{
		TextGlyph textGlyph = textGlyphs.back();
		textGlyphs.pop_back();

		long textSquare = textGlyph.rect.width * textGlyph.rect.height;

		std::list<TextGlyph>::iterator textIterator = textGlyphs.begin();
		while(textIterator != textGlyphs.end())
		{
			std::list<TextGlyph>::iterator current = textIterator;
			textIterator++;

			Rect intersection;
			if(textGlyph.rect.GetIntersection(current->rect, intersection))
			{				
				long candidateSquare = current->rect.width * current->rect.height;
				long intersectionSquare = intersection.width * intersection.height;
				double ratio = 1.0 * intersectionSquare / textSquare;
				double candidateRatio = 1.0 * intersectionSquare / candidateSquare;
				if(ratio > 0.5 || candidateRatio > 0.5)
					textGlyphs.erase(current);
			}
		}

		if(!CutRim(textGlyph))
			continue;

		outTextGlyphs.push_back(textGlyph);
				
		printf("Rect: x=%d, y=%d, w=%d, h=%d, confidence: %d, text: %s\n",
			textGlyph.rect.origin.x, pBinaryBitmap->GetHeight() - textGlyph.rect.origin.y -
			textGlyph.rect.height, textGlyph.rect.width, textGlyph.rect.height, 
			textGlyph.confidence, textGlyph.text.c_str());//!!!!debug
	}
}
		
void TextRecognizer::RemoveTextFromBitmap(std::list<TextGlyph>& textGlyphs)
{
	std::list<TextGlyph>::const_iterator textIterator;
	//int index = 0;//!!!debug
	for(textIterator = textGlyphs.begin(); textIterator != textGlyphs.end(); textIterator++)
	{
		//!!!we shouldn't remove connected to outside components
		Rect rect = textIterator->rect;
		{
			long shiftX = rect.width / 10;
			long shiftY = rect.height / 10;
			rect.origin = rect.origin + Point(shiftX / 2, shiftY / 2);
			rect.width -= shiftX;
			rect.height -= shiftY;
		}

		pBinaryBitmap->SetRect(rect, false);
	}
}

bool TextRecognizer::RecognizeTextInternal(std::list<TextGlyph>& outTextGlyphs, std::string& errorText)
{
	FindTextAreas();

	if(pDebugger)
	{
		pDebugger->ShowBinaryImageWithRects(pBinaryBitmap, textAreas);
		pDebugger->WaitForContinue();
	}

    // Initialize tesseract-ocr with English, without specifying tessdata path
    if (pAPI->Init(NULL, "eng")) {
        errorText = "Could not initialize tesseract";
		return false;
    }

	Pix* pImage = CreateImageByBinaryBitmap();
	//pixWrite("D:\\dev\\WhiteboardPhotoConverter\\Release\\pixImage.bmp", pImage, IFF_BMP);//!!!debug
	pAPI->SetImage(pImage);

	bool result = ReconginzeTextBlocks(errorText);

	pixDestroy(&pImage);

	if(!result)
		return false;

	RemoveTextIntersections(outTextGlyphs);
	if(pDebugger)
	{
		textAreas.clear();

		std::list<TextGlyph>::const_iterator textIterator;
		for(textIterator = outTextGlyphs.begin(); textIterator != outTextGlyphs.end(); textIterator++)
			textAreas.push_back(textIterator->rect);

		pDebugger->ShowBinaryImageWithRects(pBinaryBitmap, textAreas);
		pDebugger->WaitForContinue();
	}

	RemoveTextFromBitmap(outTextGlyphs);

	pAPI->End();

	return true;
}

bool TextRecognizer::RecognizeText(BinaryBitmap* pBinaryBitmap, std::list<TextGlyph>& textGlyphs, 
								   std::string& errorText, Debugger* pDebugger)
{
	TextRecognizer textRecognizer(pBinaryBitmap, pDebugger);
	return textRecognizer.RecognizeTextInternal(textGlyphs, errorText);
}
