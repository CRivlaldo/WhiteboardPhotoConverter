#include "Prerequisites.h"

using namespace WPC;

Shape::Shape(Shape::ShapeType shapeType, const Point& origin, long blockSize)
{
	this->shapeType = shapeType;
	this->origin = origin;
	this->blockSize = blockSize;
}
		
double Shape::GetMatch(BinaryBitmap* pBinaryBitmap) const
{
	BinaryBitmap patternBB(pBinaryBitmap->GetWidth(), pBinaryBitmap->GetHeight());
	FillBinaryBitmap(patternBB);
	
	return pBinaryBitmap->GetMatch(patternBB);
}

bool Shape::IsShapeForMatching(ShapeType shapeType)
{
	if(shapeType == ShapeType_FreeText)
		return false;

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

RectShape::RectShape(const Point& origin, long width, long height, long blockSize) 
	: Shape(Shape::ShapeType_Rect, origin, blockSize)
{
	this->width = width;
	this->height = height;
}
		
void RectShape::FillBinaryBitmap(BinaryBitmap &binaryBitmap) const
{
	for(long x = 0; x < width; x++)
	{
		binaryBitmap.SetPixel(x, 0);
		binaryBitmap.SetPixel(x, height - 1);
		//binaryBitmap.SetPixel(x, 1);///!!!???
		//binaryBitmap.SetPixel(x, height - 2);///!!!???
	}
	
	for(long y = 1; y < height - 1; y++)
	{
		binaryBitmap.SetPixel(0, y);
		binaryBitmap.SetPixel(width - 1, y);
		//binaryBitmap.SetPixel(1, y);///!!!???
		//binaryBitmap.SetPixel(width - 2, y);///!!!???
	}
}
		
Rect RectShape::GetRegion() const
{
	return Rect(origin, width, height);
}
		
bool RectShape::FindNearestPoint(const Point& fromPoint, Point& intersectionPoint) const 
{
	Point center = origin + Point(width / 2, height / 2);

	long dx = std::max<long>(std::abs(fromPoint.x - center.x) - width / 2, 0);
	long dy = std::max<long>(std::abs(fromPoint.y - center.y) - height / 2, 0);

	intersectionPoint = fromPoint + Point(dx, dy);

	return true;
}

bool RectShape::HasIntersection(const Point& point) const
{
	return GetRegion().Contains(point) && Rect(origin + Point(1, 1), width - 2, height - 2).Contains(point);
}
		
bool RectShape::Contains(const Point& point) const
{
	return GetRegion().Contains(point);
}
		
Shape* RectShape::Extract() const
{
	return new RectShape(origin * blockSize, width * blockSize, height * blockSize, 1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
		
FreeText::FreeText(const Point& origin, long width, long height, long blockSize, const std::string& text) :
	RectShape(origin, width, height, blockSize)
{
	this->text = text;
}

FreeText::FreeText(const TextGlyph& textGlyph) :
	RectShape(textGlyph.rect.origin, textGlyph.rect.width, textGlyph.rect.height, 1)
{
	this->text = textGlyph.text;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

EllipseShape::EllipseShape(const Point& origin, double radiusX, double radiusY, long blockSize) 
	: Shape(Shape::ShapeType_Ellipse, origin, blockSize)
{
	this->radiusX = radiusX;
	this->radiusY = radiusY;
}
		
void EllipseShape::FillBinaryBitmap(BinaryBitmap &binaryBitmap) const
{
	///!!!!need to calc step
	for(double x = -radiusX; x <= radiusX; x += .1f)
	{
		double y = radiusY * sqrt(1 - x * x / (radiusX * radiusX));

		long _x = (long)(radiusX + x);
		long _y1 = (long)(radiusY + y - 0.5);
		long _y2 = (long)(radiusY - y + 0.5);

		if(_x < 0 || _x >= binaryBitmap.GetWidth())
			continue;

		if(_y1 >= 0 && _y1 < binaryBitmap.GetHeight())
			binaryBitmap.SetPixel(_x, _y1);
		//else
		//	printf("(%d, %d)\n", _x, _y1);///!!!!vladimir debug
		
		if(_y2 >= 0 && _y2 < binaryBitmap.GetHeight())
			binaryBitmap.SetPixel(_x, _y2);
		//else
		//	printf("(%d, %d)\n", _x, _y2);///!!!!vladimir debug
	}

	///!!!!clean
	//for(double x = -radiusX + 1; x <= radiusX - 1; x += .1f)
	//{
	//	double y = radiusY * sqrt(1 - x * x / ((radiusX - 1) * (radiusX - 1)));

	//	long _x = (long)(radiusX + x);
	//	long _y1 = (long)(radiusY + y - 0.5);
	//	long _y2 = (long)(radiusY - y + 0.5);

	//	if(_x >= 0 && _x < binaryBitmap.GetWidth() && _y1 >= 0 && _y1 < binaryBitmap.GetHeight())
	//		binaryBitmap.SetPixel(_x, _y1);
	//	//else
	//	//	printf("(%d, %d)\n", _x, _y1);///!!!!vladimir debug
	//	
	//	if(_x >= 0 && _x < binaryBitmap.GetWidth() && _y2 >= 0 && _y2 < binaryBitmap.GetHeight())
	//		binaryBitmap.SetPixel(_x, _y2);
	//	//else
	//	//	printf("(%d, %d)\n", _x, _y2);///!!!!vladimir debug

	//}
}
		
Rect EllipseShape::GetRegion() const
{
	return Rect(origin - Point((long)(radiusX - .5), (long)(radiusY - .5)), 
		(long)(2 * radiusX), (long)(2 * radiusY));
}

void EllipseShape::UpdateMinLength(const Point& fromPoint, const Point& localPoint, 
								   long& minLength, Point& intersectionPoint) const
{
	Point point = origin + localPoint;
	Point difference = point - fromPoint;
	long lengthSqr = difference.x * difference.x + difference.y * difference.y;

	if(lengthSqr < minLength || minLength == std::numeric_limits<long>::infinity())
	{
		minLength = lengthSqr;
		intersectionPoint = point;
	}
}
		
bool EllipseShape::FindNearestPoint(const Point& fromPoint, Point& intersectionPoint) const 
{
	//!!!!not optimized
	//first of all, we need to detect closest sector

	long width = (long)(2 * radiusX);
	long height = (long)(2 * radiusY);
	long minLength = std::numeric_limits<long>::infinity();

	//!!!need to calc step
	for(double x = -radiusX; x <= radiusX; x += .05f)
	{
		double y = radiusY * sqrt(1 - x * x / (radiusX * radiusX));//!!!!we should pass double_y

		long _x = (long)(x + .5);
		long _y1 = (long)(-y - 0.5);
		long _y2 = (long)(y + 0.5);

		//if(_x < 0 || _x >= width)
		//	continue;

		//if(_y1 >= 0 && _y1 < height)
			UpdateMinLength(fromPoint, Point(_x, _y1), minLength, intersectionPoint);
		
		//if(_y2 >= 0 && _y2 < height)
			UpdateMinLength(fromPoint, Point(_x, _y2), minLength, intersectionPoint);
	}

	return true;
}
		
//bool EllipseShape::FindIntersection(const std::vector<double>& curveEquationCoefficients,
//					  const std::pair<long, long>& endPoint, std::pair<long, long>& intersectionPoint) const
//{
//	///.....
//
//	return true;
//}

bool EllipseShape::HasIntersection(const Point& point) const
{
	double localX = point.x - origin.x;
	double localY = radiusY * sqrt(1 - localX * localX / (radiusX * radiusX));

	long y1 = (long)(origin.y + localY + 0.5);
	long y2 = (long)(origin.y - localY - 0.5);

	printf("Ellipse tests (%d %d) ~ (%d %d), (%d %d)\n", point.x, point.y, point.x, y1, point.x, y2);///!!!!debug

	return point.y == y1 || point.y == y2;
}
		
bool EllipseShape::Contains(const Point& point) const
{
	double localX = point.x - origin.x;
	double localY = point.y - origin.y;

	return localX * localX / (radiusX * radiusX) + localY * localY / (radiusY * radiusY) <= 1;
}
		
Shape* EllipseShape::Extract() const
{
	return new EllipseShape(origin * blockSize, (long)(radiusX * blockSize), (long)(radiusY * blockSize), 1);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

TextGlyph::TextGlyph(const Rect& rect, const std::string& text, const double confidence)
{
	this->rect = rect;
	this->text = text;
	this->confidence = confidence;
}