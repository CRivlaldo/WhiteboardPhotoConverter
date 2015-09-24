#include "Prerequisites.h"

using namespace WPC;

Rect::Rect(long x, long y, long width, long height)
{
	origin = Point(x, y);
	this->width = width;
	this->height = height;
}

Rect::Rect(const Point& origin, long width, long height)
{
	this->origin = origin;
	this->width = width;
	this->height = height;
}

Rect::Rect(const Point& minPoint, const Point& maxPoint)
{
	this->origin = minPoint;
	this->width = maxPoint.x - minPoint.x + 1;
	this->height = maxPoint.y - minPoint.y + 1;
}

bool Rect::Contains(const Point& point) const
{
	return origin.x <= point.x && point.x < origin.x + width && 
		origin.y <= point.y && point.y < origin.y + height;
}

double Rect::GetDistanceSquareFromPoint(const Point& point) const
{
	Point center = origin + Point(width, height) / 2.0;

	long dx = std::max<long>(std::abs(point.x - center.x) - width / 2, 0);
	long dy = std::max<long>(std::abs(point.y - center.y) - height / 2, 0);

	return dx * dx + dy * dy;
}

void Rect::CropBy(const Rect& rect)
{
	Point maxPoint = GetMax();
	Point rectMaxPoint = rect.GetMax();

	origin = Point(std::max<long>(origin.x, rect.origin.x), std::max<long>(origin.y, rect.origin.y));
	maxPoint = Point(std::min<long>(maxPoint.x, rectMaxPoint.x), std::min<long>(maxPoint.y, rectMaxPoint.y));

	this->width = maxPoint.x - origin.x + 1;
	this->height = maxPoint.y - origin.y + 1;
}

bool Rect::GetIntersection(const Rect& rect, Rect& outRect) const
{
	outRect = *this;
	outRect.CropBy(rect);

	return (outRect.width > 0) && (outRect.height > 0);
}
		
bool Rect::HasIntersection(const Rect& rect) const
{
	Rect crop;
	crop.CropBy(rect);

	return (crop.width > 0) && (crop.height > 0);
}