#pragma once

namespace WPC
{
	class Shape;
	class Line;
	class Chart;

	class Debugger
	{
	public:
		Debugger(void);
		virtual ~Debugger(void);

		virtual void WaitForContinue() = 0;
		virtual void ShowImageFromFile(const std::string& fileName) = 0;
		virtual void ShowBinaryImage(BinaryBitmap* pBinaryBitmap) = 0;
		virtual void ShowBinaryImageWithRects(BinaryBitmap* pBinaryBitmap, const std::list<Rect>& rectList) = 0;
		virtual void ShowBinaryImageWithRegions(BinaryBitmap* pBinaryBitmap, QuickUnion* pRegions) = 0;
		virtual void ShowBinaryImageWithRegionsAndShapes(BinaryBitmap* pBinaryBitmap, QuickUnion* pRegions, 
			const std::list<Shape*>& shapeList, const std::list<Line*>& lineList) = 0;
		virtual void ShowChart(const Chart* pChart) = 0;
		virtual void ShowMessage(const std::string& message) = 0;
		//virtual void AddHint(const std::string& message) = 0;
	};
}

