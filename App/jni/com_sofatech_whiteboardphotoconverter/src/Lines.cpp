#include "Prerequisites.h"

using namespace WPC;

//!!!!move to computational geometry module
bool ArePointsOnTheSameLine(const Point& point0, const Point& point1, const Point& point2)
{
	if(point0.x == point1.x)
		return point0.x == point2.x;
	
	if(point0.y == point1.y)
		return point0.y == point2.y;

	return ((point2.x  - point0.x) * (point1.y - point0.y)) == 
		((point2.y - point0.y) * (point1.x - point0.x));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

Line::Line(std::list<Point> controlPoints)
{
	this->controlPoints = controlPoints;
}
		
Point Line::GetBeginPoint() const
{ 
	if(controlPoints.empty())
		return Point(-1, -1);

	return controlPoints.front(); 
}
		
Point Line::GetEndPoint() const
{ 
	if(controlPoints.empty())
		return Point(-1, -1);

	return controlPoints.back(); 
}

void Line::AddControlPoint(const Point& point, bool toBegin)
{
	if(toBegin)
		controlPoints.push_front(point);
	else
		controlPoints.push_back(point);
}

void Line::RemoveControlPoint(bool fromBegin)
{
	if(fromBegin)
		controlPoints.pop_front();
	else
		controlPoints.pop_back();
}

std::list<Point>::const_iterator Line::GetBegin() const 
{ 
	return controlPoints.begin(); 
}

std::list<Point>::const_iterator Line::GetEnd() const
{
	return controlPoints.end();
}
		
std::list<Point>::const_reverse_iterator Line::GetReversedBegin() const
{
	return controlPoints.rbegin();
}
		
std::list<Point>::const_reverse_iterator Line::GetReversedEnd() const
{
	return controlPoints.rend();
}

void Line::Extract(long blockSize, Line& outLine) const
{
	std::list<Point>::const_iterator it;
	for(it = GetBegin(); it != GetEnd(); it++)
	{
		Point point = *it;
		outLine.AddControlPoint(point * blockSize);
	}
}

void Line::RemovePointsOnTheSameLine()
{
	if(controlPoints.size() <= 2)
		return;

	std::vector<Point> newControlPoints;

	//newControlPoints.push_back(controlPoints.front());
	//controlPoints.pop_front();

	while(!controlPoints.empty())
	{
		Point currentPoint = controlPoints.front();
		controlPoints.pop_front();

		if(newControlPoints.size() < 2)
			newControlPoints.push_back(currentPoint);
		else
		{
			if(ArePointsOnTheSameLine(newControlPoints[newControlPoints.size() - 2], 
				newControlPoints[newControlPoints.size() - 1], currentPoint))
			{
				newControlPoints[newControlPoints.size() - 1] = currentPoint;
			}
			else
				newControlPoints.push_back(currentPoint);
		}
	}

	controlPoints.assign(newControlPoints.begin(), newControlPoints.end());
}