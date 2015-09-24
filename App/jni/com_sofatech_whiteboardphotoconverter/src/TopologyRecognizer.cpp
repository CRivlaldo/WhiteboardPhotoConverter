#include "Prerequisites.h"

using namespace WPC;

void Get3LinePoints(Line* pLine, bool fromBegin, std::vector<Point>&  linePoints)
{
	if(fromBegin)
	{
		std::list<Point>::const_iterator it;
		for(it = pLine->GetBegin(); it != pLine->GetEnd(); it++)
		{
			linePoints.push_back(*it);
			if(linePoints.size() == 3)
				break;
		}
	}
	else
	{
		std::list<Point>::const_reverse_iterator it;
		for(it = pLine->GetReversedBegin(); it != pLine->GetReversedEnd(); it++)
		{
			linePoints.push_back(*it);
			if(linePoints.size() == 3)
				break;
		}
	}

	std::reverse(linePoints.begin(), linePoints.end());
}

//bool GetLineEquationCoefficients(const std::vector<std::pair<long, long>>&  linePoints,
//	std::vector<double>& curveEquationCoefficients)
//{
//	if(linePoints[0].first == linePoints[1].first)
//		return false;
//
//	double x0 = linePoints[0].first;
//	double y0 = linePoints[0].second;
//	double x1 = linePoints[1].first;
//	double y1 = linePoints[1].second;
//
//	double det = x0 - x1;
//	double a = (y0 - y1) / det;
//	double b = (-y0 * x1 + x0 * y1) / det;
//	
//	curveEquationCoefficients.push_back(0.0);
//	curveEquationCoefficients.push_back(a);
//	curveEquationCoefficients.push_back(b);
//
//	return true;
//}
//
//bool GetCurveEquationCoefficients(const std::vector<std::pair<long, long>>&  linePoints,
//	std::vector<double>& curveEquationCoefficients)
//{
//	if(linePoints.size() == 2)
//		return GetLineEquationCoefficients(linePoints, curveEquationCoefficients);
//	
//	double x0 = linePoints[0].first;
//	double y0 = linePoints[0].second;
//	double x1 = linePoints[1].first;
//	double y1 = linePoints[1].second;
//	double x2 = linePoints[2].first;
//	double y2 = linePoints[2].second;
//
//	double x0_2 = x0 * x0;
//	double x1_2 = x1 * x1;
//	double x2_2 = x2 * x2;
//
//	double det = x0_2 * (x1 - x2) + x1_2 * (x2 - x0) + x2_2 * (x0 - x1);
//
//	double a = y0 * (x1 - x2) + y1 * (x2 - x0) + y2 * (x0 - x1);
//	double b = y0 * (x2_2 - x1_2) + y1 * (x0_2 - x2_2) + y2 * (x1_2 - x0_2);
//	double c = y0 * x1 * x2 * (x1 - x2) + y1 * x0 * x2 * (x2 - x0) + y2 * x0 * x1 * (x0 - x1);
//
//	curveEquationCoefficients.push_back(a);
//	curveEquationCoefficients.push_back(b);
//	curveEquationCoefficients.push_back(c);
//
//	return true;
//}

bool ArePointsOnTheSameLine(const std::vector<Point>& linePoints)
{
	//!!!!
	return false;
}

//void GetPointOnBezierCurve(const std::vector<std::pair<long, long>>& controlPoints, const double t,
//	std::pair<long, long>& point)
//{
//	double k0 = 1 - t * t;
//	double k1 = 2 * t * (1 - t);
//	double k2 = t * t;
//
//	point.first = (long)(controlPoints[0].first * k0 + controlPoints[1].first * k1 + 
//		controlPoints[2].first * k2 + .5);
//	point.second = (long)(controlPoints[0].second * k0 + controlPoints[1].second * k1 + 
//		controlPoints[2].second * k2 + .5);
//}

void GetParametricFunctionCoefficients(double f0, double f1, double f2, double coefficients[])
{
	coefficients[0] = 2 * f0 - 4 * f1 + 2 * f2;
	coefficients[1] = -3 * f0 + 4 * f1 - f2;
	coefficients[2] = f0;
}

void GetParametricFunctionCoefficients(const std::vector<Point>& controlPoints, 
	double coefficients[])
{
	double xCoefficients[3];
	double yCoefficients[3];

	GetParametricFunctionCoefficients(controlPoints[0].x, controlPoints[1].x, 
		controlPoints[2].x, xCoefficients);
	GetParametricFunctionCoefficients(controlPoints[0].y, controlPoints[1].y, 
		controlPoints[2].y, yCoefficients);

	coefficients[0] = xCoefficients[0];
	coefficients[1] = xCoefficients[1];
	coefficients[2] = xCoefficients[2];

	coefficients[3] = yCoefficients[0];
	coefficients[4] = yCoefficients[1];
	coefficients[5] = yCoefficients[2];
}

void GetPointOnParametricCurve(double coefficients[], const double t, Point& point)
{
	point.x = (long)(coefficients[0] * t * t + coefficients[1] * t + coefficients[2] + .5);
	point.y = (long)(coefficients[3] * t * t + coefficients[4] * t + coefficients[5] + .5);
}

double GetDeltaForParametricCurve(double coefficients[], short coefficientStartIndex)
{
	double a = coefficients[coefficientStartIndex + 0];
	double b = coefficients[coefficientStartIndex + 1];

	double delta = 1;
	while(true)
	{
		double squareDelta = delta * delta;
		double squareDistance = squareDelta * (a * a * squareDelta + 2 * a * b + delta + b * b * squareDelta);
		if(squareDistance < 1)
			break;

		delta /= 2;
	}

	return delta / 2;
}

double GetDeltaForParametricCurve(double coefficients[])
{
	return std::min(GetDeltaForParametricCurve(coefficients, 0), 
		GetDeltaForParametricCurve(coefficients, 3));
}

//!!!temp
void GetPointOnLine(const Point& point0, const Point& point1, const double t, Point& point)
{
	point.x = (long)((point1.x - point0.x) * t + .5);
	point.y = (long)((point1.y - point0.y) * t + .5);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

TopologyRecognizer::TopologyRecognizer(const BinaryBitmap *pBinaryBitmap, const std::list<Shape*>* pShapes,
									   const std::list<Line*>* pLines, 
									   const std::list<TextGlyph>* pTextGlyphs)
{
	this->pBinaryBitmap = pBinaryBitmap;
	this->pShapes = pShapes;
	this->pLines = pLines;
	//this->pTextGlyphs = pTextGlyphs;

	unusedTextGlyphs.assign(pTextGlyphs->begin(), pTextGlyphs->end());
}


TopologyRecognizer::~TopologyRecognizer(void)
{
}

Shape* TopologyRecognizer::FindNearestShape(const Point& pixel) const
{
	long maxSquareDistance = pBinaryBitmap->GetBlockSize() * pBinaryBitmap->GetBlockSize();

	std::list<Shape*>::const_iterator it;
	for(it = extractedShapes.begin(); it != extractedShapes.end(); it++)
	{
		Shape* pShape = *it;
		if(pShape->GetRegion().GetDistanceSquareFromPoint(pixel) <= maxSquareDistance)
			return pShape;
	}

	return NULL;
}
		
bool TopologyRecognizer::ExpandCurveToShape(Line* pLine, Shape* pShape, bool fromBegin,
											Point& intersectionPoint) const
{
	std::vector<Point> linePoints;
	Get3LinePoints(pLine, fromBegin, linePoints);

	if(linePoints.size() == 3 && !ArePointsOnTheSameLine(linePoints))
	{
		double curveCoefficients[6];
		GetParametricFunctionCoefficients(linePoints, curveCoefficients);

		double delta = GetDeltaForParametricCurve(curveCoefficients);
		for(double t = 1; t < 1.5; t += delta)
		{
			//GetPointOnBezierCurve(linePoints, t, intersectionPoint);
			GetPointOnParametricCurve(curveCoefficients, t, intersectionPoint);
			if(pShape->HasIntersection(intersectionPoint))
				return true;
		}
	}
	else
	{
		///!!!!what about analitics algorithm (for ray and shape)?
		for(double t = 1; t < 1.5; t += 0.001)//!!!delta
		{
			GetPointOnLine(linePoints[0], linePoints[linePoints.size() - 1], t, intersectionPoint);
			if(pShape->HasIntersection(intersectionPoint))
				return true;
		}
	}

	//std::vector<double> curveEquationCoefficients;
	//std::pair<long, long> intersectionPoint;
	//bool findIntersection = false;
	//if(GetCurveEquationCoefficients(linePoints, curveEquationCoefficients))
	//{
	//	return pShape->FindIntersection(curveEquationCoefficients, endPoint, intersectionPoint);
	//}
	//else
	//{
	//	//!!!!y || line
	//}

	return false;
}
		
bool TopologyRecognizer::ExpandPointToShape(const Point& endPoint, Shape* pShape, 
											bool fromBegin, Point& intersectionPoint) const
{
	pShape->FindNearestPoint(endPoint, intersectionPoint);

	return true;
}

bool TopologyRecognizer::ExpandLineToShape(Line* pLine, Shape* pShape, bool fromBegin) const
{
	Point endPoint;

	while(pLine->GetPointCount() > 0)
	{
		endPoint = fromBegin ? pLine->GetBeginPoint() : pLine->GetEndPoint();

		if(!pShape->Contains(endPoint) || pShape->HasIntersection(endPoint))
			break;

		pLine->RemoveControlPoint(fromBegin);
	} 

	if(pLine->GetPointCount() == 0)
		return false;

	Point intersectionPoint;

	if(!ExpandCurveToShape(pLine, pShape, fromBegin, intersectionPoint))
	{
		if(!ExpandPointToShape(endPoint, pShape, fromBegin, intersectionPoint))
			return false;
	}	

	if(intersectionPoint != endPoint)
	{
		pLine->AddControlPoint(intersectionPoint, fromBegin);
	}

	return true;
}

void TopologyRecognizer::ExtractShapes()
{
	std::list<Shape*>::const_iterator it;
	for(it = pShapes->begin(); it != pShapes->end(); it++)
	{
		Shape* pShape = *it;
		extractedShapes.push_back(pShape->Extract());
	}
}
		
void TopologyRecognizer::ExtractFreeText()
{
	std::list<TextGlyph>::const_iterator textIterator;
	for(textIterator = unusedTextGlyphs.begin(); textIterator != unusedTextGlyphs.end(); textIterator++)
	{
		extractedShapes.push_back(new FreeText(*textIterator));
	}
}

std::string TopologyRecognizer::FindText(const Shape* pShape)
{
	std::list<TextGlyph> connectedText;

	std::list<TextGlyph>::iterator textIterator = unusedTextGlyphs.begin();
	while(textIterator != unusedTextGlyphs.end())
	{
		std::list<TextGlyph>::iterator currentText = textIterator;
		textIterator++;

		Rect intersection;
		if(pShape->GetRegion().GetIntersection(currentText->rect, intersection))
		{
			if(intersection.GetSquare() * 1.0 / currentText->rect.GetSquare() > 0.5)
				connectedText.push_back(*currentText);

			unusedTextGlyphs.erase(currentText);
		}
	}

	connectedText.sort(&CompareHeights);

	std::string resultText = "";
	for(textIterator = connectedText.begin(); textIterator != connectedText.end(); textIterator++)
	{
		if(resultText != "")
			resultText += std::string("\n") + textIterator->text;
		else
			resultText = textIterator->text;
	}

	return resultText;
}

void TopologyRecognizer::CreateNodes(Chart*& pChart)
{
	std::list<Shape*>::const_iterator it;
	for(it = extractedShapes.begin(); it != extractedShapes.end(); it++)
	{
		Shape* pShape = *it;

		pChart->AddNode(pShape, FindText(pShape));
	}

	//!!!!add unused texts
}

void TopologyRecognizer::ConnectNodes(Chart*& pChart)
{
	std::list<Line*>::const_iterator it;
	for(it = pLines->begin(); it != pLines->end(); it++)
	{
		Line* pLine = *it;

		Line* pExtractedLine = new Line();
		pLine->Extract(pBinaryBitmap->GetBlockSize(), *pExtractedLine);

		Shape* pFirstShape = FindNearestShape(pExtractedLine->GetBeginPoint());
		Shape* pSecondShape = FindNearestShape(pExtractedLine->GetEndPoint());

		if(pFirstShape)
		{
			if(!ExpandLineToShape(pExtractedLine, pFirstShape, true))
				pFirstShape = NULL;
		}

		if(pSecondShape)
		{
			if(!ExpandLineToShape(pExtractedLine, pSecondShape, false))
				pSecondShape = NULL;
		}

		if(pFirstShape || pSecondShape)
		{
			pExtractedLine->RemovePointsOnTheSameLine();
			pChart->AddSiblingLink(pFirstShape, pSecondShape, pExtractedLine); ///!!!!what about arrows and shape respects?
		}
	}
}

bool TopologyRecognizer::TryToRecognizeChartInternal(Chart*& pChart)
{
	pChart = new Chart(pBinaryBitmap->GetWidth() * pBinaryBitmap->GetBlockSize(), 
		pBinaryBitmap->GetHeight() * pBinaryBitmap->GetBlockSize());

	ExtractShapes();
	CreateNodes(pChart);

	ExtractFreeText();

	ConnectNodes(pChart);

	return true;
}
		
bool TopologyRecognizer::TryToRecognizeChart(const BinaryBitmap *pBinaryBitmap, 
											 const std::list<Shape*>& shapes, const std::list<Line*>& lines,
											 const std::list<TextGlyph>& textGlyphs, Chart*& pChart)
{
	TopologyRecognizer recognizer(pBinaryBitmap, &shapes, &lines, &textGlyphs);
	return recognizer.TryToRecognizeChartInternal(pChart);
}
