#include "Prerequisites.h"

using namespace WPC;

Point::Point(long x, long y)
{
	this->x = x;
	this->y = y;
}

Point::Point(double x, double y)
{
	this->x = (long)(x + .5);
	this->y = (long)(y + .5);
}
	
Point::Point(int x, int y)
{
	this->x = x;
	this->y = y;
}

const Point Point::operator+(const Point& point) const
{
	return Point(x + point.x, y + point.y);
}

const Point Point::operator-(const Point& point) const
{
	return Point(x - point.x, y - point.y);
}
	
const Point Point::operator*(const long scale) const
{
	return Point(x * scale, y * scale);
}
	
const Point Point::operator/(const long divisor) const
{
	return Point(x / divisor, y / divisor);
}
	
const Point Point::operator/(const double divisor) const
{
	return Point(x / divisor, y / divisor);
}
	
const bool Point::operator==(const Point& point) const
{
	return x == point.x && y == point.y;
}
	
const bool Point::operator!=(const Point& point) const
{
	return x != point.x || y != point.y;
}
	
const bool Point::operator<(const Point& point) const
{
	return y < point.y || (y == point.y && x < point.x);
}