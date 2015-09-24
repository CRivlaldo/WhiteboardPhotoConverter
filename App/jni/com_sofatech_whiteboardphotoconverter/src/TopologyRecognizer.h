#pragma once

namespace WPC
{
	class TopologyRecognizer
	{
	private:
		const std::list<Shape*>* pShapes;
		const std::list<Line*>* pLines;
		std::list<Shape*> extractedShapes;
		const BinaryBitmap *pBinaryBitmap;
		std::list<TextGlyph> unusedTextGlyphs; 

		TopologyRecognizer(const BinaryBitmap *pBinaryBitmap, const std::list<Shape*>* pShapes, 
			const std::list<Line*>* pLines, const std::list<TextGlyph>* pTextGlyphs);
		~TopologyRecognizer(void);

		void ExtractShapes();
		void ExtractFreeText();
		void CreateNodes(Chart*& pChart);
		void ConnectNodes(Chart*& pChart);
		bool TryToRecognizeChartInternal(Chart*& pChart);
		Shape* FindNearestShape(const Point& pixel) const;
		bool ExpandLineToShape(Line* pLine, Shape* pShape, bool fromBegin) const;
		bool ExpandCurveToShape(Line* pLine, Shape* pShape, bool fromBegin, Point& intersectionPoint) const;
		bool ExpandPointToShape(const Point& endPoint, Shape* pShape, bool fromBegin,
			Point& intersectionPoint) const;
		std::string FindText(const Shape* pShape);

	public:
		static bool TryToRecognizeChart(const BinaryBitmap *pBinaryBitmap, const std::list<Shape*>& shapes, 
			const std::list<Line*>& lines, const std::list<TextGlyph>& textGlyphs, Chart*& pChart);
	};
}

