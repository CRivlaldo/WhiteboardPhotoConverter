namespace WPC
{
	struct Rect
	{
		Point origin;
		long width, height;
		
		Rect() {}
		Rect(long x, long y, long width, long height);
		Rect(const Point& origin, long width, long height);	
		Rect(const Point& minPoint, const Point& maxPoint);			

		Point GetMin() const { return origin; }
		Point GetMax() const { return origin + Point(width - 1, height - 1); }

		bool Contains(const Point& point) const;
		double GetDistanceSquareFromPoint(const Point& point) const;	
		void CropBy(const Rect& rect);
		bool GetIntersection(const Rect& rect, Rect& outRect) const;
		bool HasIntersection(const Rect& rect) const;
		long GetSquare() const { return width * height; }
	};
}