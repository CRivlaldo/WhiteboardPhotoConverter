namespace WPC
{	
	class Line
	{
	/*private:
		class CubicSpline
		{
		};*/

	private:
		//std::list<CubicSpline> splines;
		//long originX;
		//long originY;
		std::list<Point> controlPoints;
		
	public:
		void AddControlPoint(const Point& pont, bool toBegin = false);
		void RemoveControlPoint(bool fromBegin = false);
		void RemovePointsOnTheSameLine();

		std::list<Point>::const_iterator GetBegin() const;
		std::list<Point>::const_iterator GetEnd() const;
		std::list<Point>::const_reverse_iterator GetReversedBegin() const;
		std::list<Point>::const_reverse_iterator GetReversedEnd() const;
		long GetPointCount() const { return controlPoints.size(); }

		Point GetBeginPoint() const;
		Point GetEndPoint() const;

		void Extract(long blockSize, Line& outLine) const;

		Line() {};
		Line(std::list<Point> controlPoints);
	};
}