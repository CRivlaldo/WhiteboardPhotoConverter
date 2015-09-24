#pragma once

namespace WPC
{
	struct Point
	{
		long x;
		long y;

		Point(void) {};	
		Point(long x, long y);
		Point(double x, double y);
		Point(int x, int y);

		static const Point Zero() { return Point(0, 0); }
		static const Point One() { return Point(1, 1); }

		const Point operator+(const Point& point) const;
		const Point operator-(const Point& point) const;	
		const Point operator*(const long scale) const;
		const Point operator/(const long divisor) const;	
		const Point operator/(const double divisor) const;		
		const bool operator==(const Point& point) const;	
		const bool operator!=(const Point& point) const;
		const bool operator<(const Point& point) const;
	};
}



