#include "Prerequisites.h"

using namespace WPC;

ShapeRecognizer::ShapeRecognizer(BinaryBitmap* pBinaryBitmap, double matchLevel, 
								 std::list<Shape*>* pShapes, std::list<Line*>* pLines) :
	regionUnion(pBinaryBitmap->GetWidth() * pBinaryBitmap->GetHeight() + 2),
	chunkUnion(pBinaryBitmap->GetWidth() * pBinaryBitmap->GetHeight() + 1)
{
	this->pShapes = pShapes;
	this->pLines = pLines;

	InitInternal(pBinaryBitmap, matchLevel, NULL);///!!!!we don't need 2 constructors
}

ShapeRecognizer::ShapeRecognizer(BinaryBitmap* pBinaryBitmap, double matchLevel, 
								 std::list<Shape*>* pShapes, std::list<Line*>* pLines, 
								 Debugger* pDebugger) :
	regionUnion(pBinaryBitmap->GetWidth() * pBinaryBitmap->GetHeight() + 2),
	chunkUnion(pBinaryBitmap->GetWidth() * pBinaryBitmap->GetHeight() + 1)
{
	this->pShapes = pShapes;
	this->pLines = pLines;

	InitInternal(pBinaryBitmap, matchLevel, pDebugger);///!!!!we just need to set debugger as property
}

ShapeRecognizer::~ShapeRecognizer(void)
{
	//std::list<Shape*>::iterator shapeIterator;
	//for(shapeIterator = shapes.begin(); shapeIterator != shapes.end(); shapeIterator++)
	//{
	//	delete *shapeIterator;
	//}

	//std::list<Line*>::iterator lineIterator;
	//for(lineIterator = lines.begin(); lineIterator != lines.end(); lineIterator++)
	//{
	//	delete *lineIterator;
	//}
}

void ShapeRecognizer::InitInternal(BinaryBitmap* pBinaryBitmap, double matchLevel, Debugger* pDebugger)
{
	this->pBinaryBitmap = pBinaryBitmap;
	this->pDebugger = pDebugger;
	this->matchLevel = matchLevel;

	lightPixelIndex = pBinaryBitmap->GetWidth() * pBinaryBitmap->GetHeight();
	outSurfaceIndex = lightPixelIndex + 1;

	backgroundPixelIndex = pBinaryBitmap->GetWidth() * pBinaryBitmap->GetHeight();
}

bool ShapeRecognizer::RecognizeShapes(BinaryBitmap* pBinaryBitmap, double matchLevel, 
									  std::list<Shape*>& shapes, std::list<Line*>& lines, 
									  Debugger* pDebugger = NULL)
{
	ShapeRecognizer shapeRecognizer(pBinaryBitmap, matchLevel, &shapes, &lines, pDebugger);
	return shapeRecognizer.RecognizeShapesAndLines();
}

void ShapeRecognizer::InitRegionUnion()
{
	for(long y = 0; y < pBinaryBitmap->GetHeight(); y++)
	{
		if(pBinaryBitmap->GetWidth() == 0)
			break;

		bool previousLight = pBinaryBitmap->GetPixel(0, y);

		for(long x = 1; x < pBinaryBitmap->GetWidth(); x++)
		{
			bool light = pBinaryBitmap->GetPixel(x, y);
			if(light)
			{
				regionUnion.Union(y * pBinaryBitmap->GetWidth() + x, lightPixelIndex);
			}
			else if(!previousLight)
			{
				regionUnion.Union(y * pBinaryBitmap->GetWidth() + x - 1, y * pBinaryBitmap->GetWidth() + x);
			}
			previousLight = light;
		}

		if(!pBinaryBitmap->GetPixel(0, y))
			regionUnion.Union(y * pBinaryBitmap->GetWidth(), outSurfaceIndex);
		if(!pBinaryBitmap->GetPixel(pBinaryBitmap->GetWidth() - 1, y))
			regionUnion.Union((y + 1) * pBinaryBitmap->GetWidth() - 1, outSurfaceIndex);
	}

	for(long x = 0; x < pBinaryBitmap->GetWidth(); x++)
	{
		if(pBinaryBitmap->GetHeight() == 0)
			break;

		bool previousLight = pBinaryBitmap->GetPixel(x, 0);

		for(long y = 1; y < pBinaryBitmap->GetHeight(); y++)
		{
			bool light = pBinaryBitmap->GetPixel(x, y);
			if(!previousLight && !light)
			{
				regionUnion.Union((y - 1) * pBinaryBitmap->GetWidth() + x, y * pBinaryBitmap->GetWidth() + x);
			}
			previousLight = light;
		}

		if(!pBinaryBitmap->GetPixel(x, 0))
			regionUnion.Union(x, outSurfaceIndex);
		if(!pBinaryBitmap->GetPixel(x, pBinaryBitmap->GetHeight() - 1))
			regionUnion.Union((pBinaryBitmap->GetHeight() - 1) * pBinaryBitmap->GetWidth() + x, outSurfaceIndex);
	}
}

void ShapeRecognizer::InitChunkUnion()
{
	for(long y = 0; y < pBinaryBitmap->GetHeight(); y++)
	{
		if(pBinaryBitmap->GetWidth() == 0)
			break;

		if(!pBinaryBitmap->GetPixel(0, y))
			chunkUnion.Union(pBinaryBitmap->GetPixelIndex(0, y), backgroundPixelIndex);

		for(long x = 1; x < pBinaryBitmap->GetWidth(); x++)
		{
			long pixelIndex = pBinaryBitmap->GetPixelIndex(x, y);

			if(pBinaryBitmap->GetPixel(x, y))
			{
				if(y > 0) //????this one already checked
				{
					if(pBinaryBitmap->GetPixel(x - 1, y - 1))
						chunkUnion.Union(pBinaryBitmap->GetPixelIndex(x - 1, y - 1), pixelIndex);
				}
				
				if(pBinaryBitmap->GetPixel(x - 1, y))
					chunkUnion.Union(pBinaryBitmap->GetPixelIndex(x - 1, y), pixelIndex);

				if(y < pBinaryBitmap->GetHeight() - 1)
				{
					if(pBinaryBitmap->GetPixel(x - 1, y + 1))
						chunkUnion.Union(pBinaryBitmap->GetPixelIndex(x - 1, y + 1), pixelIndex);
				}
			}
			else
			{
				chunkUnion.Union(pixelIndex, backgroundPixelIndex);
			}
		}
	}

	for(long x = 0; x < pBinaryBitmap->GetWidth(); x++)
	{
		if(pBinaryBitmap->GetHeight() == 0)
			break;

		if(!pBinaryBitmap->GetPixel(x, 0))
			chunkUnion.Union(pBinaryBitmap->GetPixelIndex(x, 0), backgroundPixelIndex);

		for(long y = 1; y < pBinaryBitmap->GetHeight(); y++)
		{
			long pixelIndex = pBinaryBitmap->GetPixelIndex(x, y);

			if(pBinaryBitmap->GetPixel(x, y))
			{
				if(x > 0)
				{
					if(pBinaryBitmap->GetPixel(x - 1, y - 1))
						chunkUnion.Union(pBinaryBitmap->GetPixelIndex(x - 1, y - 1), pixelIndex);
				}
				
				if(pBinaryBitmap->GetPixel(x, y - 1))
					chunkUnion.Union(pBinaryBitmap->GetPixelIndex(x, y - 1), pixelIndex);

				if(x < pBinaryBitmap->GetWidth() - 1)
				{
					if(pBinaryBitmap->GetPixel(x + 1, y - 1))
						chunkUnion.Union(pBinaryBitmap->GetPixelIndex(x + 1, y - 1), pixelIndex);
				}
			}
		}
	}
}

void ShapeRecognizer::ExpandRegion(Rect& region)
{
	////!!!!delete
	{
		//if(region.origin.x > 0)
		//{
		//	region.origin.x--;
		//	region.width++;
		//}
		//if(region.origin.y > 0)
		//{
		//	region.origin.y--;
		//	region.height++;
		//}
		//		
		//if(region.GetMax().x < pBinaryBitmap->GetWidth() - 1)
		//	region.width++;

		//if(region.GetMax().y < pBinaryBitmap->GetHeight() - 1)
		//	region.height++;
	}

	region.origin.x--;
	region.origin.y--;

	region.width += 2;
	region.height += 2;

	region.CropBy(pBinaryBitmap->GetRect());
}

bool ShapeRecognizer::RecognizeShapesAndLines()
{	
	if(!RecognizeShapes())
		return false;

	if(!RecognizeLines())
		return false;

	return true;
}

bool ShapeRecognizer::RecognizeShapes()
{
	InitRegionUnion();
	FillHoles();
		
	long startY = 1;
	std::list<long> pointsFromNotRecognizedRegions;

	while(true)
	{
		if(pDebugger)
		{//!!!!here we can show bitmap and shapes
			pDebugger->ShowBinaryImageWithRegionsAndShapes(pBinaryBitmap, &regionUnion, *pShapes, *pLines);
			pDebugger->WaitForContinue();
		}

		if(!FindAndRecognizeShape(startY, pointsFromNotRecognizedRegions))
			break;

		Rect region = pShapes->back()->GetRegion();
		ExpandRegion(region);

		RecreateRegionUnion(region);
	}

	return true;
}

bool ShapeRecognizer::RecognizeLines()
{
	InitChunkUnion();

	long startY = 1;
	std::list<long> pointsFromNotRecognizedChunks;

	while(true)
	{
		if(pDebugger)
		{//!!!!here we can show bitmap and shapes
			pDebugger->ShowBinaryImageWithRegionsAndShapes(pBinaryBitmap, &chunkUnion, *pShapes, *pLines);
			pDebugger->WaitForContinue();
		}

		long chunkIndex;
		if(!FindAndRecognizeLine(startY, pointsFromNotRecognizedChunks, chunkIndex))
			break;
		
		chunkUnion.Union(chunkIndex, backgroundPixelIndex);
	}

	return true;
}

bool ShapeRecognizer::IsPixelConnectedToNotRecognizedRegion(long x, long y, 
															std::list<long>& pointsFromNotRecognizedRegions)
{
	long pointIndex = y * pBinaryBitmap->GetWidth() + x;

	std::list<long>::iterator it;
	for(it = pointsFromNotRecognizedRegions.begin(); it != pointsFromNotRecognizedRegions.end(); it++)
	{
		if(regionUnion.HasConnection(*it, pointIndex))
			return true;
	}

	return false;
}

bool ShapeRecognizer::FindAndRecognizeShape(long& startY, std::list<long>& pointsFromNotRecognizedRegions)
{
	///!!!maybe we need list of contours

	for(long y = startY; y < pBinaryBitmap->GetHeight() - 1; y++)
	{
		for(long x = 1; x < pBinaryBitmap->GetWidth() - 1; x++)
		{
			if(!regionUnion.HasConnection(y * pBinaryBitmap->GetWidth() + x, lightPixelIndex) &&
				!regionUnion.HasConnection(y * pBinaryBitmap->GetWidth() + x, outSurfaceIndex))
			{
				if(IsPixelConnectedToNotRecognizedRegion(x, y, pointsFromNotRecognizedRegions))
					continue;

				Contour contour = Contour::CreateContourOfRegion(pBinaryBitmap, Point(x, y));

				if(pDebugger)
				{
					pDebugger->ShowBinaryImage(&contour.ToBinaryBitmap());
					pDebugger->WaitForContinue();
				}

				Shape* pShape;
				double match;
				ShapeMatcher::TryToMatchShape(&contour.ToBinaryBitmap(), contour.GetOrigin(), 
					pShape, match, pDebugger);

				if(match >= matchLevel)
				{
					pShapes->push_back(pShape);
					contour.EraseFromBitmap();
					startY = y;			

					return true;					
				}
				
				pointsFromNotRecognizedRegions.push_back(y * pBinaryBitmap->GetWidth() + x);
			}
		}
	}

	return false;
}

bool ShapeRecognizer::FindAndRecognizeLine(long& startY, std::list<long>& pointsFromNotRecognizedChunks,
											long& chunkIndex)
{
	//we can start just from first changed pixel

	///!!!maybe we need list of lines

	for(long y = startY; y < pBinaryBitmap->GetHeight() - 1; y++)
	{
		for(long x = 0; x < pBinaryBitmap->GetWidth(); x++)
		{
			if(pBinaryBitmap->GetPixel(x, y))
			{
				chunkIndex = pBinaryBitmap->GetPixelIndex(x, y);
				Chunk chunk = Chunk::CreateChunkFromUnion(pBinaryBitmap, chunkUnion, chunkIndex);

				Line* pLine;

				if(LineRecognizer::TryToRecognizeLineFromChunk(pBinaryBitmap, chunk, pLine))
				{
					pLines->push_back(pLine);
				}

				chunk.EraseFromBitmap();

				startY = y;
				return true;
			}
		}
	}

	return false;
}

void ShapeRecognizer::RecreateRegionUnion(const Rect& region)
{
	for(long y = region.GetMin().y; y <= region.GetMax().y; y++)
	{
		if(pBinaryBitmap->GetWidth() == 0)
			break;

		bool previousLight = pBinaryBitmap->GetPixel(region.GetMin().x, y);
		
		if(!previousLight && regionUnion.HasConnection(y * pBinaryBitmap->GetWidth() + region.GetMin().x, 
			lightPixelIndex))
		{
			regionUnion.Disconnect(y * pBinaryBitmap->GetWidth() + region.GetMin().x);
		}

		for(long x = region.GetMin().x + 1; x <= region.GetMax().x; x++)
		{
			bool light = pBinaryBitmap->GetPixel(x, y);

			if(!light && regionUnion.HasConnection(y * pBinaryBitmap->GetWidth() + x, lightPixelIndex))
				regionUnion.Disconnect(y * pBinaryBitmap->GetWidth() + x);

			if(!light && !previousLight)
			{
				regionUnion.Union(y * pBinaryBitmap->GetWidth() + x - 1, y * pBinaryBitmap->GetWidth() + x);
			}
			previousLight = light;
		}

		/*if(!pBinaryBitmap->GetPixel(0, y))
			regionUnion.Union(y * pBinaryBitmap->GetWidth(), outSurfaceIndex);
		if(!pBinaryBitmap->GetPixel(pBinaryBitmap->GetWidth() - 1, y))
			regionUnion.Union((y + 1) * pBinaryBitmap->GetWidth() - 1, outSurfaceIndex);*/
	}

	for(long x = region.GetMin().x; x <= region.GetMax().x; x++)
	{
		if(pBinaryBitmap->GetHeight() == 0)
			break;

		bool previousLight = pBinaryBitmap->GetPixel(x, region.GetMin().y);

		for(long y = region.GetMin().y + 1; y <= region.GetMax().y; y++)
		{
			bool light = pBinaryBitmap->GetPixel(x, y);
			if(!previousLight && !light)
			{
				regionUnion.Union((y - 1) * pBinaryBitmap->GetWidth() + x, y * pBinaryBitmap->GetWidth() + x);
			}
			previousLight = light;
		}

		/*if(!pBinaryBitmap->GetPixel(x, 0))
			regionUnion.Union(x, outSurfaceIndex);
		if(!pBinaryBitmap->GetPixel(x, pBinaryBitmap->GetHeight() - 1))
			regionUnion.Union((pBinaryBitmap->GetHeight() - 1) * pBinaryBitmap->GetWidth() + x, outSurfaceIndex);*/
	}
}

///!!!method isn't finished
//void ShapeRecognizer::RecreateChunkUnion(const Rect& region)
//{
//	for(long y = region.GetMinY(); y <= region.GetMaxY(); y++)
//	{
//		if(!pBinaryBitmap->GetPixel(0, y))
//			chunkUnion.Union(pBinaryBitmap->GetPixelIndex(0, y), backgroundPixelIndex);
//
//		for(long x = region.GetMinX() + 1; x < pBinaryBitmap->GetWidth(); x++)
//		{
//			long pixelIndex = pBinaryBitmap->GetPixelIndex(x, y);
//
//			if(pBinaryBitmap->GetPixel(x, y))
//			{
//				if(y > 0) //????this one already checked
//				{
//					if(pBinaryBitmap->GetPixel(x - 1, y - 1))
//						chunkUnion.Union(pBinaryBitmap->GetPixelIndex(x - 1, y - 1), pixelIndex);
//				}
//				
//				if(pBinaryBitmap->GetPixel(x - 1, y))
//					chunkUnion.Union(pBinaryBitmap->GetPixelIndex(x - 1, y), pixelIndex);
//
//				if(y < pBinaryBitmap->GetHeight() - 1)
//				{
//					if(pBinaryBitmap->GetPixel(x - 1, y + 1))
//						chunkUnion.Union(pBinaryBitmap->GetPixelIndex(x - 1, y + 1), pixelIndex);
//				}
//			}
//			else
//			{
//				chunkUnion.Union(pixelIndex, backgroundPixelIndex);
//			}
//		}
//	}
//
//	for(long x = 0; x < pBinaryBitmap->GetWidth(); x++)
//	{
//		if(pBinaryBitmap->GetHeight() == 0)
//			break;
//
//		if(!pBinaryBitmap->GetPixel(x, 0))
//			chunkUnion.Union(pBinaryBitmap->GetPixelIndex(x, 0), backgroundPixelIndex);
//
//		for(long y = 1; y < pBinaryBitmap->GetHeight(); y++)
//		{
//			long pixelIndex = pBinaryBitmap->GetPixelIndex(x, y);
//
//			if(pBinaryBitmap->GetPixel(x, y))
//			{
//				if(x > 0)
//				{
//					if(pBinaryBitmap->GetPixel(x - 1, y - 1))
//						chunkUnion.Union(pBinaryBitmap->GetPixelIndex(x - 1, y - 1), pixelIndex);
//				}
//				
//				if(pBinaryBitmap->GetPixel(x, y - 1))
//					chunkUnion.Union(pBinaryBitmap->GetPixelIndex(x, y - 1), pixelIndex);
//
//				if(x < pBinaryBitmap->GetWidth() - 1)
//				{
//					if(pBinaryBitmap->GetPixel(x + 1, y - 1))
//						chunkUnion.Union(pBinaryBitmap->GetPixelIndex(x + 1, y - 1), pixelIndex);
//				}
//			}
//		}
//	}
//}

void ShapeRecognizer::FillHoles()
{
	//long processedPixelsIndex = pBinaryBitmap->GetWidth() * pBinaryBitmap->GetHeight();
	//QuickUnion processedPixels = regionUnion;

	for(long y = 0; y < pBinaryBitmap->GetHeight(); y++)
	{
		for(long x = 0; x < pBinaryBitmap->GetWidth(); x++)
		{
			if(pBinaryBitmap->GetPixel(x, y))
				continue;

			long pixelIndex = pBinaryBitmap->GetPixelIndex(x, y);
			//if(processedPixels.HasConnection(processedPixelsIndex, pixelIndex))
			//	continue;

			if(regionUnion.HasConnection(pixelIndex, outSurfaceIndex))
				continue;

			Chunk chunk = Chunk::CreateChunkFromUnion(pBinaryBitmap, regionUnion, pixelIndex);
			if(chunk.GetCount() <= 3)
			{
				std::vector<Point>::const_iterator chunkIterator;
				for(chunkIterator = chunk.GetBegin(); chunkIterator != chunk.GetEnd(); 
					chunkIterator++)
				{
					pBinaryBitmap->SetPixel(chunkIterator->x, chunkIterator->y);
				}
					
				regionUnion.Union(pixelIndex, lightPixelIndex);
			}

			//processedPixels.Union(processedPixelsIndex, pixelIndex);
		}
	}
}