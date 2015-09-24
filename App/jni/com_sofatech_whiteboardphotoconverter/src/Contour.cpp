#include "Prerequisites.h"

using namespace WPC;

Contour::ContourBypasser::ContourBypasserPixel::ContourBypasserPixel() :
	Point()
{
	//this->x = 0;
	//this->y = 0;
	this->direction = Up;
}

Contour::ContourBypasser::ContourBypasserPixel::ContourBypasserPixel(const Point& pixel, 
																	 ContourPixelDirection direction)
																	 : Point(pixel)
{
	//this->x = x;
	//this->y = y;
	this->direction = direction;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

Contour::ContourBypasser::ContourBypasser(Contour* pContour, BinaryBitmap* pBinaryBitmap, const Point& start)
{
	this->pContour = pContour;
	this->pBinaryBitmap = pBinaryBitmap;
	this->startPixel = ContourBypasserPixel(start, Up);

	currentPixel = startPixel;
}

Contour::ContourBypasser::ContourPixelDirection Contour::ContourBypasser::GetNextContourPixelDirection(
	Contour::ContourBypasser::ContourPixelDirection direction)
{
	return (Contour::ContourBypasser::ContourPixelDirection) 
		(((int)direction + 1) % CONTOUR_PIXEL_DIRECTION_COUNT);
}

Contour::ContourBypasser::ContourPixelDirection Contour::ContourBypasser::GetPreviousContourPixelDirection(
	Contour::ContourBypasser::ContourPixelDirection direction)
{
	return (Contour::ContourBypasser::ContourPixelDirection) 
		(((int)direction + CONTOUR_PIXEL_DIRECTION_COUNT - 1) % CONTOUR_PIXEL_DIRECTION_COUNT);
}

bool Contour::ContourBypasser::EndOfContour() const
{
	return (Point)currentPixel == (Point)startPixel;
}

void Contour::ContourBypasser::Next()
{
	AddAroundPixels();
	GoToNextPixel(1);
	//printf("(%d, %d)\n", currentPoint.x, currentPoint.y);///!!!!vladimir delete
}

void Contour::ContourBypasser::AddAroundPixels()
{
	for(long x = currentPixel.x - 1; x <= currentPixel.x + 1; x++)
	{
		if(x < 0 || x >= pBinaryBitmap->GetWidth())
			continue;

		for(long y = currentPixel.y - 1; y <= currentPixel.y + 1; y++)
		{
			if(y < 0 || y >= pBinaryBitmap->GetHeight())
				continue;

			if(pBinaryBitmap->GetPixel(x, y))
				pContour->AddPixel(x, y);
		}
	}
}

void Contour::ContourBypasser::GoToNextPixel(int attempt)
{
	if(attempt > 4)
		return;

	switch(GetPreviousContourPixelDirection(currentPixel.direction))
	{
	case Left:
		{
			if(CanMoveToPixel(currentPixel - Point(1, 0)))
			{
				currentPixel.x--;
				currentPixel.direction = Left;
				return;
			}
		}
		break;
	case Up:
		{
			if(CanMoveToPixel(currentPixel - Point(0, 1)))
			{
				currentPixel.y--;
				currentPixel.direction = Up;
				return;
			}
		}
		break;
	case Right:
		{
			if(CanMoveToPixel(currentPixel + Point(1, 0)))
			{
				currentPixel.x++;
				currentPixel.direction = Right;
				return;
			}
		}
		break;
	case Down:
		{
			if(CanMoveToPixel(currentPixel + Point(0, 1)))
			{
				currentPixel.y++;
				currentPixel.direction = Down;
				return;
			}
		}
		break;
	}
				
	currentPixel.direction = GetNextContourPixelDirection(currentPixel.direction);				
	GoToNextPixel(attempt + 1);
}

bool Contour::ContourBypasser::CanMoveToPixel(const Point& pixel) const
{
	if(pBinaryBitmap->GetRect().Contains(pixel))
	{
		return !pBinaryBitmap->GetPixel(pixel.x, pixel.y);
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

BinaryBitmap Contour::ToBinaryBitmap() const
{
	BinaryBitmap binaryBitmap(GetWidth(), GetHeight(), pBitmap->GetBlockSize());

	std::vector<Point>::const_iterator it;
	for(it = GetBegin(); it != GetEnd(); it++)
	{
		Point pixel = *it - minPixel;
		binaryBitmap.SetPixel(pixel.x, pixel.y);
	}

	return binaryBitmap;
}

const Contour Contour::CreateContourOfRegion(BinaryBitmap* pBinaryBitmap, const Point& start)
{
	Contour contour(pBinaryBitmap);
	ContourBypasser contourBypasser(&contour, pBinaryBitmap, start);

	do
	{
		contourBypasser.Next();
	} 
	while(!contourBypasser.EndOfContour());

	return contour;
}

bool Contour::IsPixelOutsideContour(const Point& pixel) const
{
	return pBitmap->GetRect().Contains(pixel) && pBitmap->GetPixel(pixel.x, pixel.y) && !Contains(pixel);
}

void Contour::EraseFromBitmap() const
{
	std::vector<Point>::const_iterator it;
	for(it = GetBegin(); it != GetEnd(); it++)
	{
		Point pixel = *it;

		if(IsPixelOutsideContour(pixel - Point(1, 0)) || IsPixelOutsideContour(pixel - Point(0, 1)) ||
			IsPixelOutsideContour(pixel + Point(1, 0)) || IsPixelOutsideContour(pixel + Point(0, 1)))
		{
			continue;
		}

		pBitmap->SetPixel(pixel.x, pixel.y, false);
	}
}