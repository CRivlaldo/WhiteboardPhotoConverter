#pragma once

class ConsoleDebugger : public WPC::Debugger
{
public:
	ConsoleDebugger(void);
	~ConsoleDebugger(void);

	void WaitForContinue();
	void ShowImageFromFile(const std::string& fileName);
	void ShowBinaryImage(WPC::BinaryBitmap* pBinaryBitmap);
	void ShowBinaryImageWithRects(WPC::BinaryBitmap* pBinaryBitmap, const std::list<WPC::Rect>& rectList);
	void ShowBinaryImageWithRegions(WPC::BinaryBitmap* pBinaryBitmap, WPC::QuickUnion* pRegions);
	void ShowBinaryImageWithRegionsAndShapes(WPC::BinaryBitmap* pBinaryBitmap, WPC::QuickUnion* pRegions, 
		const std::list<WPC::Shape*>& shapeList, const std::list<WPC::Line*>& lineList);
	void ShowChart(const WPC::Chart* pChart);
	void ShowMessage(const std::string& message);
};

