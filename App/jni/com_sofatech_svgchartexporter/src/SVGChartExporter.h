#pragma once

class SVGChartExporter : public WPC::ChartExporter
{
private:	
	svg::Document* pDocument;

	svg::Point WPCPointToSVGPoint(const WPC::Point& point);
	void WriteRectangle(const WPC::RectShape* pRect);
	void WriteEllipse(const WPC::EllipseShape* pEllipse);
	void WriteLine(const WPC::Line* pLine);
	void WriteText(const WPC::Shape* pShape, const std::string& text);

	const std::string WrapTextLinesToTSpanElements(const std::list<std::string>& lines, 
		const svg::Point& origin, const double fontHeight);

public:
	SVGChartExporter(const WPC::Chart* pChart) : WPC::ChartExporter(pChart) {}
	~SVGChartExporter();

	bool SaveChartToFile(const std::string& filePath, std::string& errorText);
};

