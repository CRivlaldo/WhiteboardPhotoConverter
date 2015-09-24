#include <vector>
#include <list>

#include "simple_svg_1.0.0.hpp"
#include "WhiteboardPhotoConverter.h"
#include "SVGChartExporter.h"

using namespace WPC;
using namespace svg;

#ifdef ANDROID
template <typename T>
std::string to_string(T value)
{
    std::ostringstream os ;
    os << value ;
    return os.str() ;
}
#endif // ANDROID

std::list<std::string> GetTextLines(const std::string& text)
{
	std::list<std::string> lines;
	
	size_t endPosition = 0;
	size_t startPosition = text.find("\n", 0);
	while(startPosition != std::string::npos)
	{
         std::string line = text.substr(endPosition, startPosition - endPosition);
		 //if(line.length() > 0)
			lines.push_back(line);
			
		 endPosition = startPosition;

		 startPosition = text.find("\n", startPosition + 1);
	}

	if(endPosition < text.length())
		lines.push_back(text.substr(endPosition, text.length() - endPosition));

	return lines;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

SVGChartExporter::~SVGChartExporter()
{
	delete pDocument;
}

//!!!to the class
svg::Point SVGChartExporter::WPCPointToSVGPoint(const WPC::Point& point)
{
	return svg::Point(point.x, pChart->GetHeight() - point.y);
}

void SVGChartExporter::WriteRectangle(const WPC::RectShape* pRect)
{
	*pDocument << Rectangle(WPCPointToSVGPoint(pRect->GetOrigin()), pRect->GetWidth(), pRect->GetHeight(), 
		Color::White, Stroke(1, Color::Black));
}

void SVGChartExporter::WriteEllipse(const WPC::EllipseShape* pEllipse)
{
	*pDocument << Elipse(WPCPointToSVGPoint(pEllipse->GetOrigin()), pEllipse->GetRadiusX() * 2,
		pEllipse->GetRadiusY() * 2, 	Color::White, Stroke(1, Color::Black));
}

void SVGChartExporter::WriteLine(const WPC::Line* pLine)
{
	Path path(Stroke(1, Color::Black));

	std::list<WPC::Point>::const_iterator pointIterator;
	for(pointIterator = pLine->GetBegin(); pointIterator != pLine->GetEnd(); pointIterator++)
	{
		path << WPCPointToSVGPoint(*pointIterator);
	}
	*pDocument << path;
}

const std::string SVGChartExporter::WrapTextLinesToTSpanElements(const std::list<std::string>& lines,
																 const svg::Point& origin,
																 const double fontHeight)
{
	std::string outText = "";
	
	double dy = 0;
	std::list<std::string>::const_iterator lineIterator;
	for(lineIterator = lines.begin(); lineIterator != lines.end(); lineIterator++)
	{
#ifndef ANDROID
		std::string line = std::string("<tspan x=\"") + std::to_string((int)origin.x) + std::string("\" dy=\"") +
			std::to_string((int)dy) + std::string("\">") + *lineIterator + std::string("</tspan>");
#else
		std::string line = std::string("<tspan x=\"") + to_string((int)origin.x) + std::string("\" dy=\"") +
					to_string((int)dy) + std::string("\">") + *lineIterator + std::string("</tspan>");
#endif
		if(outText == "")
			outText = line;
		else
			outText += std::string("\n") + line;

		dy = dy + fontHeight * 1.25;
	}

	return outText;
}

void SVGChartExporter::WriteText(const WPC::Shape* pShape, const std::string& text)
{
	WPC::Point centerPoint = (pShape->GetRegion().GetMax() + pShape->GetRegion().GetMin()) / 2.0;
	svg::Point origin = WPCPointToSVGPoint(centerPoint);

	//std::string xmlText = StringUtils::Replace(text, "\n", "\\n");
	std::string wrappedText = WrapTextLinesToTSpanElements(GetTextLines(text), origin, 10);

	*pDocument << Text(origin, wrappedText, Color::Black, Font(10, "Verdana"));///!!!!size, font
}

bool SVGChartExporter::SaveChartToFile(const std::string& filePath, std::string& errorText)
{
	Dimensions dimensions(pChart->GetWidth(), pChart->GetHeight());
	pDocument = new Document(filePath.c_str(), Layout(dimensions, Layout::TopLeft));

	std::vector<Chart::Node*>::const_iterator nodeIterator;
	for(nodeIterator = pChart->GetNodesBegin(); nodeIterator != pChart->GetNodesEnd(); nodeIterator++)
	{
		const Chart::Node* pNode = *nodeIterator;
		const WPC::Shape* pShape = pNode->GetShape();
		switch(pShape->GetShapeType())
		{
		case WPC::Shape::ShapeType_Rect:
			WriteRectangle((const RectShape*)pShape);
			break;
		case WPC::Shape::ShapeType_Ellipse:
			WriteEllipse((const EllipseShape*)pShape);
			break;
		}

		WriteText(pShape, pNode->GetText());
	}

	std::vector<WPC::Chart::Link*>::const_iterator linkIterator;
	for(linkIterator = pChart->GetLinksBegin(); linkIterator != pChart->GetLinksEnd(); linkIterator++)
	{
		const WPC::Chart::Link* pLink = *linkIterator;
		WriteLine(pLink->GetLine());
	}

	///////////////

    //// Red image border.
    //Polygon border(Stroke(1, Color::Red));
    //border << svg::Point(0, 0) << svg::Point(dimensions.width, 0)
    //    << svg::Point(dimensions.width, dimensions.height) << svg::Point(0, dimensions.height);
    //doc << border;

    //// Long notation.  Local variable is created, children are added to varaible.
    //LineChart chart(5.0);
    //Polyline polyline_a(Stroke(.5, Color::Blue));
    //Polyline polyline_b(Stroke(.5, Color::Aqua));
    //Polyline polyline_c(Stroke(.5, Color::Fuchsia));
    //polyline_a << svg::Point(0, 0) << svg::Point(10, 30)
    //    << svg::Point(20, 40) << svg::Point(30, 45) << svg::Point(40, 44);
    //polyline_b << svg::Point(0, 10) << svg::Point(10, 22)
    //    << svg::Point(20, 30) << svg::Point(30, 32) << svg::Point(40, 30);
    //polyline_c << svg::Point(0, 12) << svg::Point(10, 15)
    //    << svg::Point(20, 14) << svg::Point(30, 10) << svg::Point(40, 2);
    //chart << polyline_a << polyline_b << polyline_c;
    //doc << chart;

    //// Condensed notation, parenthesis isolate temporaries that are inserted into parents.
    //doc << (LineChart(Dimensions(65, 5))
    //    << (Polyline(Stroke(.5, Color::Blue)) << svg::Point(0, 0) << svg::Point(10, 8) << svg::Point(20, 13))
    //    << (Polyline(Stroke(.5, Color::Orange)) << svg::Point(0, 10) << svg::Point(10, 16) << svg::Point(20, 20))
    //    << (Polyline(Stroke(.5, Color::Cyan)) << svg::Point(0, 5) << svg::Point(10, 13) << svg::Point(20, 16)));

    //doc << Circle(svg::Point(80, 80), 20, Fill(Color(100, 200, 120)), Stroke(1, Color(200, 250, 150)));

    //doc << Text(svg::Point(5, 77), "Simple SVG", Color::Silver, Font(10, "Verdana"));

    //doc << (Polygon(Color(200, 160, 220), Stroke(.5, Color(150, 160, 200))) << svg::Point(20, 70)
    //    << svg::Point(25, 72) << svg::Point(33, 70) << svg::Point(35, 60) << svg::Point(25, 55) << svg::Point(18, 63));

    //doc << Rectangle(svg::Point(70, 55), 20, 15, Color::Yellow);

	///////////////

	if(!pDocument->save())
	{
		errorText = "Could not write image to file";
		return false;
	}

	return true;
}

//bool SVGChartExporter::InitFile(const std::string& filePath)
//{
//	
//}

//bool SaveFile()
//{
//	return document.save();
//}
