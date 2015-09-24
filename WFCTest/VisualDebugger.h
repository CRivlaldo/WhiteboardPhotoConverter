#pragma once

class VisualDebugger : public WPC::Debugger
{
private:
	HWND hWindow;
	HBITMAP hBitmap;

	void DispatchMessages();	
	void UpdateImage();
	void DrawEllipse(unsigned char* pBits, long strip, const WPC::Point& center, double radiusX, double radiusY);
	void DrawRect(unsigned char* pBits, long strip, const WPC::Point& origin, long width, long height);
	void DrawLine(unsigned char* pBits, long strip, std::vector<WPC::Point> points);
public:
	VisualDebugger(void);
	~VisualDebugger(void);
	
	HWND GetWindowHandle() { return hWindow; }
	HBITMAP GetBitmapHandle() { return hBitmap; }

	void WaitForContinue();
	void ShowImageFromFile(const std::string& fileName);
	void ShowBinaryImage(WPC::BinaryBitmap* pBinaryBitmap);
	void ShowBinaryImageWithRects(WPC::BinaryBitmap* pBinaryBitmap, const std::list<WPC::Rect>& rectList);
	void ShowBinaryImageWithRegions(WPC::BinaryBitmap* pBinaryBitmap, WPC::QuickUnion* pRegions);
	void ShowBinaryImageWithRegionsAndShapes(WPC::BinaryBitmap* pBinaryBitmap, WPC::QuickUnion* pRegions, 
		const std::list<WPC::Shape*>& shapeList, const std::list<WPC::Line*>& lineList);
	void ShowChart(const WPC::Chart* pChart);
};

