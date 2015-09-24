#include "stdafx.h"

//#include "FreeImage.h"
//#include "FreeImageUtils.h"
//#include "StringUtils.h"
//#include "BinaryBitmap.h"
//#include "QuickUnion.h"
//#include "Debugger.h"
//#include "ShapeMatcher.h"

#include "VisualDebugger.h"

static VisualDebugger* pVDInstance = NULL;

using namespace WPC;

long __stdcall WindowProcedure(HWND hWnd, unsigned int msg, WPARAM wp, LPARAM lp)
{
    switch(msg)
    {
        case WM_DESTROY:
			{
				//if(pVDInstance->GetBitmapHandle()) ///!!!!?????
				//	DeleteObject(pVDInstance->GetBitmapHandle());
				PostQuitMessage(0) ;
				return 0L ;
			}
        case WM_LBUTTONDOWN:
		case WM_PAINT:
        default:
            return DefWindowProc(hWnd, msg, wp, lp) ;
    }
}

VisualDebugger::VisualDebugger(void)
{
	pVDInstance = this;
	hBitmap = NULL;

	LPCWSTR className = L"VisualDebugger" ;
    WNDCLASSEX wndclass = { sizeof(WNDCLASSEX), CS_DBLCLKS, WindowProcedure,
                            0, 0, GetModuleHandle(0), LoadIcon(0,IDI_APPLICATION),
                            LoadCursor(0,IDC_ARROW), HBRUSH(COLOR_WINDOW+1),
                            0, className, LoadIcon(0,IDI_APPLICATION) } ;

    if( RegisterClassEx(&wndclass) )
    {
        hWindow = CreateWindowEx( 0, className, L"WPC Visual Debugger",
                   WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                   CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, GetModuleHandle(0), 0 ) ;
        if(!hWindow)
			return;

        ShowWindow(hWindow, SW_SHOWDEFAULT);
		DispatchMessages();
    }
}

void VisualDebugger::DispatchMessages()
{
	MSG msg;
	while(PeekMessage(&msg, hWindow,  0, 0, PM_REMOVE))
		DispatchMessage(&msg);
}

VisualDebugger::~VisualDebugger(void)
{
	pVDInstance = NULL;
	DestroyWindow(hWindow);
}

void VisualDebugger::WaitForContinue()
{
	DispatchMessages();
	printf("Press any key for continue...");
	getchar();
}

void VisualDebugger::UpdateImage()
{
	WINDOWPLACEMENT placement;
	GetWindowPlacement(hWindow, &placement);

	BITMAP bitmap;
	GetObject(hBitmap, sizeof(BITMAP), &bitmap);

	SetWindowPos(hWindow, NULL, placement.ptMinPosition.x, placement.ptMinPosition.y, 
		bitmap.bmWidth + 25, bitmap.bmHeight + 52, SWP_SHOWWINDOW);///!!!border stuff

	HDC hWindowDC = GetDC(hWindow);
    HDC hMemDC = CreateCompatibleDC(GetDC(0));

	HGDIOBJ hObj = SelectObject(hMemDC, hBitmap);
    BitBlt(hWindowDC, 3, 3, bitmap.bmWidth, bitmap.bmHeight, hMemDC, 0, 0, SRCCOPY);

    DeleteObject(hObj);
	DeleteObject(hMemDC);
		
	SendMessage(hWindow, WM_PAINT, 0, 0);
}
	
void VisualDebugger::ShowImageFromFile(const std::string& fileName)
{
	if(hBitmap)
		DeleteObject(hBitmap);

	if(WPC::StringUtils::GetExtension(fileName) == ".bmp")
	{
		hBitmap = (HBITMAP)LoadImage((HINSTANCE)GetWindowLong(hWindow, GWL_HINSTANCE),
			WPC::StringUtils::ConvertStringToWString(fileName).c_str(), 
			IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	}
	else
	{
		FIBITMAP* pBitmap = WPC::FreeImageUtils::LoadInputFile(fileName);	
		PBITMAPINFO pFIBitmapInfo = FreeImage_GetInfo(pBitmap);

		HDC hDC = GetDC(NULL);
		hBitmap = CreateDIBitmap(hDC, FreeImage_GetInfoHeader(pBitmap),
			CBM_INIT, FreeImage_GetBits(pBitmap), FreeImage_GetInfo(pBitmap), DIB_RGB_COLORS);
		ReleaseDC(NULL, hDC);

		WPC::FreeImageUtils::UnloadBitmap(pBitmap);
	}

	if(hBitmap)
		UpdateImage();	
}

void VisualDebugger::ShowBinaryImage(WPC::BinaryBitmap* pBinaryBitmap)
{
	if(hBitmap)
		DeleteObject(hBitmap);

	LONG alignedWidth = pBinaryBitmap->GetDIBWidth();
	while(alignedWidth % 4 != 0)
		alignedWidth++;
	
	BITMAPINFO* info;
	{
        info = (LPBITMAPINFO) malloc(sizeof(BITMAPINFO));
		ZeroMemory(info, sizeof(BITMAPINFO)); 
 
        info->bmiHeader.biSize = sizeof(info->bmiHeader);
        info->bmiHeader.biWidth = alignedWidth;
        info->bmiHeader.biHeight = pBinaryBitmap->GetDIBHeight();
        info->bmiHeader.biPlanes = 1;
        info->bmiHeader.biBitCount = 24;
        info->bmiHeader.biCompression = BI_RGB;
        info->bmiHeader.biSizeImage = 0;
        info->bmiHeader.biClrUsed = 0;
        info->bmiHeader.biClrImportant = 0;
	}

	HDC hDC = GetDC(NULL);
	unsigned char* pBits = (unsigned char*)pBinaryBitmap->GetDIBBits();
	//void* bits = (void*)&(pBits[0]);
	void* bits = malloc(alignedWidth * pBinaryBitmap->GetDIBHeight() * 3);///!!!!free
	hBitmap = CreateDIBSection(hDC, info, DIB_RGB_COLORS, &bits, NULL, 0);
	
	//memcpy(bits, pBits, pBinaryBitmap->GetDIBWidth() * pBinaryBitmap->GetDIBHeight() * 3);
	for(long y = 0; y < pBinaryBitmap->GetHeight(); y++)
	{
		for(long x = 0; x < pBinaryBitmap->GetWidth(); x++)
		{
			unsigned char r = pBinaryBitmap->GetPixel(x, y) ? 255 : 0;
			unsigned char g = pBinaryBitmap->GetPixel(x, y) ? 255 : 0;
			unsigned char b = pBinaryBitmap->GetPixel(x, y) ? 255 : 0;

			for(int dy = 0; dy < pBinaryBitmap->GetBlockSize(); dy++)
			{
				for(int dx = 0; dx < pBinaryBitmap->GetBlockSize(); dx++)
				{
					long _x = x * pBinaryBitmap->GetBlockSize() + dx;
					long _y = y * pBinaryBitmap->GetBlockSize() + dy;
					((unsigned char*)bits)[(_y * alignedWidth + _x) * 3 + 0] = b;
					((unsigned char*)bits)[(_y * alignedWidth + _x) * 3 + 1] = g;
					((unsigned char*)bits)[(_y * alignedWidth + _x) * 3 + 2] = r;
				}
			}
		}
	}

	ReleaseDC(NULL, hDC);

	//free(bits);
	free(info);

	if(hBitmap)
		UpdateImage();
}

void GenerateColorsForRegions(long regionCount, unsigned char* pColors)
{
	unsigned char step = (unsigned char)((int)(255 / pow(regionCount, 1 / 3.0))+ 1);
	if(step == 0)
		step = 1;

	printf("step = %d\n", step);///!!!!vladimir delete

	long counter = 0;
	for(int r = 0; r <= 255; r += step)
	{
		for(int g = 0; g <= 255; g += step)
		{
			for(int b = 0; b <= 255; b += step)
			{
				if(counter == regionCount)
					return;

				pColors[counter * 3 + 0] = (unsigned char)r;
				pColors[counter * 3 + 1] = (unsigned char)g;
				pColors[counter * 3 + 2] = (unsigned char)b;
				printf("color_%d = (%d, %d, %d)\n", counter, r, g, b);///!!!!vladimir delete

				counter++;
			}
		}
	}
}

void VisualDebugger::ShowBinaryImageWithRegions(BinaryBitmap* pBinaryBitmap, QuickUnion* pRegions)
{
	ShowBinaryImageWithRegionsAndShapes(pBinaryBitmap, pRegions, std::list<Shape*>(), std::list<Line*>());
}
	
void VisualDebugger::ShowBinaryImageWithRects(BinaryBitmap* pBinaryBitmap, const std::list<Rect>& rectList)
{
	if(hBitmap)
		DeleteObject(hBitmap);

	long dibWidth = pBinaryBitmap->GetDIBWidth();
	long dibHeight = pBinaryBitmap->GetDIBHeight();

	LONG alignedWidth = dibWidth;
	while(alignedWidth % 4 != 0)
		alignedWidth++;
	
	BITMAPINFO* info;
	{
        info = (LPBITMAPINFO) malloc(sizeof(BITMAPINFO));
		ZeroMemory(info, sizeof(BITMAPINFO)); 
 
        info->bmiHeader.biSize = sizeof(info->bmiHeader);
        info->bmiHeader.biWidth = alignedWidth;
        info->bmiHeader.biHeight = dibHeight;
        info->bmiHeader.biPlanes = 1;
        info->bmiHeader.biBitCount = 24;
        info->bmiHeader.biCompression = BI_RGB;
        info->bmiHeader.biSizeImage = 0;
        info->bmiHeader.biClrUsed = 0;
        info->bmiHeader.biClrImportant = 0;
	}

	HDC hDC = GetDC(NULL);
	void* bits = malloc(alignedWidth * dibHeight * 3);///!!!!free
	hBitmap = CreateDIBSection(hDC, info, DIB_RGB_COLORS, &bits, NULL, 0);

	for(long y = 0; y < pBinaryBitmap->GetHeight(); y++)
	{
		for(long x = 0; x < pBinaryBitmap->GetWidth(); x++)
		{
			unsigned char r = pBinaryBitmap->GetPixel(x, y) ? 255 : 0;
			unsigned char g = pBinaryBitmap->GetPixel(x, y) ? 255 : 0;
			unsigned char b = pBinaryBitmap->GetPixel(x, y) ? 255 : 0;

			for(int dy = 0; dy < pBinaryBitmap->GetBlockSize(); dy++)
			{
				for(int dx = 0; dx < pBinaryBitmap->GetBlockSize(); dx++)
				{
					long _x = x * pBinaryBitmap->GetBlockSize() + dx;
					long _y = y * pBinaryBitmap->GetBlockSize() + dy;
					((unsigned char*)bits)[(_y * alignedWidth + _x) * 3 + 0] = b;
					((unsigned char*)bits)[(_y * alignedWidth + _x) * 3 + 1] = g;
					((unsigned char*)bits)[(_y * alignedWidth + _x) * 3 + 2] = r;
				}
			}
		}
	}
	
	std::list<Rect>::const_iterator rectIterator;
	for(rectIterator = rectList.begin(); rectIterator != rectList.end(); rectIterator++)
	{
		const Rect rect = *rectIterator;
		DrawRect((unsigned char*)bits, dibWidth, rect.origin * pBinaryBitmap->GetBlockSize(), 
			rect.width * pBinaryBitmap->GetBlockSize(), rect.height * pBinaryBitmap->GetBlockSize());
	}

	ReleaseDC(NULL, hDC);

	free(info);

	if(hBitmap)
		UpdateImage();
}
	
void VisualDebugger::ShowBinaryImageWithRegionsAndShapes(BinaryBitmap* pBinaryBitmap, QuickUnion* pRegions, 
														 const std::list<Shape*>& shapeList, 
														 const std::list<Line*>& lineList)
{
	if(hBitmap)
		DeleteObject(hBitmap);

	LONG alignedWidth = pBinaryBitmap->GetDIBWidth();
	while(alignedWidth % 4 != 0)
		alignedWidth++;
	
	BITMAPINFO* info;
	{
        info = (LPBITMAPINFO) malloc(sizeof(BITMAPINFO));
		ZeroMemory(info, sizeof(BITMAPINFO)); 
 
        info->bmiHeader.biSize = sizeof(info->bmiHeader);
        info->bmiHeader.biWidth = alignedWidth;
        info->bmiHeader.biHeight = pBinaryBitmap->GetDIBHeight();
        info->bmiHeader.biPlanes = 1;
        info->bmiHeader.biBitCount = 24;
        info->bmiHeader.biCompression = BI_RGB;
        info->bmiHeader.biSizeImage = 0;
        info->bmiHeader.biClrUsed = 0;
        info->bmiHeader.biClrImportant = 0;
	}

	HDC hDC = GetDC(NULL);
	unsigned char* pBits = (unsigned char*)pBinaryBitmap->GetDIBBits();
	//void* bits = (void*)&(pBits[0]);
	void* bits = malloc(alignedWidth * pBinaryBitmap->GetDIBHeight() * 3);///!!!!free
	hBitmap = CreateDIBSection(hDC, info, DIB_RGB_COLORS, &bits, NULL, 0);
	//memcpy(bits, pBits, pBinaryBitmap->GetDIBWidth() * pBinaryBitmap->GetDIBHeight() * 3);

	unsigned char* pColors = new unsigned char[pRegions->GetUnionCount() * 3];
	GenerateColorsForRegions(pRegions->GetUnionCount(), pColors);

	long colorCounter = 0;
	long* pRegionToColorDictionary = new long[pRegions->GetSize()];
	for(long i = 0; i < pRegions->GetSize(); i++)
		pRegionToColorDictionary[i] = -1;

	for(long y = 0; y < pBinaryBitmap->GetHeight(); y++)
	{
		for(long x = 0; x < pBinaryBitmap->GetWidth(); x++)
		{
			long regionIndex = pRegions->FindUnionIndex(y * pBinaryBitmap->GetWidth() + x);
			if(pRegionToColorDictionary[regionIndex] == -1)
			{
				pRegionToColorDictionary[regionIndex] = colorCounter;
				colorCounter++;
			}
			long colorIndex = pRegionToColorDictionary[regionIndex];
			unsigned char r = pColors[colorIndex * 3 + 0];
			unsigned char g = pColors[colorIndex * 3 + 1];
			unsigned char b = pColors[colorIndex * 3 + 2];

			///!!!debug
			if(pRegions->GetSize() == (pBinaryBitmap->GetWidth() * pBinaryBitmap->GetHeight() + 2) &&
				pRegions->HasConnection(y * pBinaryBitmap->GetWidth() + x, 
					pBinaryBitmap->GetWidth() * pBinaryBitmap->GetHeight() + 1) ||
				pRegions->GetSize() == (pBinaryBitmap->GetWidth() * pBinaryBitmap->GetHeight() + 1) &&
				pRegions->HasConnection(y * pBinaryBitmap->GetWidth() + x, 
					pBinaryBitmap->GetWidth() * pBinaryBitmap->GetHeight()))
			{
				r = 255;
				g = 0;
				b = 255;
			}

			for(int dy = 0; dy < pBinaryBitmap->GetBlockSize(); dy++)
			{
				for(int dx = 0; dx < pBinaryBitmap->GetBlockSize(); dx++)
				{
					long _x = x * pBinaryBitmap->GetBlockSize() + dx;
					long _y = y * pBinaryBitmap->GetBlockSize() + dy;
					((unsigned char*)bits)[(_y * alignedWidth + _x) * 3 + 0] = b;
					((unsigned char*)bits)[(_y * alignedWidth + _x) * 3 + 1] = g;
					((unsigned char*)bits)[(_y * alignedWidth + _x) * 3 + 2] = r;
				}
			}
		}
	}

	std::list<Shape*>::const_iterator shapeIterator;
	for(shapeIterator = shapeList.begin(); shapeIterator != shapeList.end(); shapeIterator++)
	{
		Shape* pShape = *shapeIterator;
		switch(pShape->GetShapeType())
		{
		case Shape::ShapeType_Ellipse:
			{
				EllipseShape* pEllipse = (EllipseShape*)pShape;
				DrawEllipse((unsigned char*)bits, pBinaryBitmap->GetDIBWidth(), pEllipse->GetDIBOrigin(), 
					pEllipse->GetDIBRadiusX(), pEllipse->GetDIBRadiusY());
			}
			break;
		case Shape::ShapeType_Rect:
			{
				RectShape* pRect = (RectShape*)pShape;
				DrawRect((unsigned char*)bits, pBinaryBitmap->GetDIBWidth(), pRect->GetDIBOrigin(), 
					pRect->GetDIBWidth(), pRect->GetDIBHeight());
			}
			break;
		}
	}
	
	std::list<Line*>::const_iterator lineIterator;
	for(lineIterator = lineList.begin(); lineIterator != lineList.end(); lineIterator++)
	{
		Line* pLine = *lineIterator;
		std::vector<Point> lineListOfPoints;

		std::list<Point>::const_iterator linePointsIterator;
		for(linePointsIterator = pLine->GetBegin(); linePointsIterator != pLine->GetEnd(); 
			linePointsIterator++)
		{
			Point point = *linePointsIterator;
			lineListOfPoints.push_back(point * pBinaryBitmap->GetBlockSize());
		}

		DrawLine((unsigned char*)bits, pBinaryBitmap->GetDIBWidth(), lineListOfPoints);
	}

	ReleaseDC(NULL, hDC);

	delete pRegionToColorDictionary;
	delete pColors;
	free(info);

	if(hBitmap)
		UpdateImage();
}

void VisualDebugger::ShowChart(const Chart* pChart)
{
	if(hBitmap)
		DeleteObject(hBitmap);

	long dibWidth = pChart->GetWidth();
	long dibHeight = pChart->GetHeight();

	LONG alignedWidth = dibWidth;
	while(alignedWidth % 4 != 0)
		alignedWidth++;
	
	BITMAPINFO* info;
	{
        info = (LPBITMAPINFO) malloc(sizeof(BITMAPINFO));
		ZeroMemory(info, sizeof(BITMAPINFO)); 
 
        info->bmiHeader.biSize = sizeof(info->bmiHeader);
        info->bmiHeader.biWidth = alignedWidth;
        info->bmiHeader.biHeight = dibHeight;
        info->bmiHeader.biPlanes = 1;
        info->bmiHeader.biBitCount = 24;
        info->bmiHeader.biCompression = BI_RGB;
        info->bmiHeader.biSizeImage = 0;
        info->bmiHeader.biClrUsed = 0;
        info->bmiHeader.biClrImportant = 0;
	}

	HDC hDC = GetDC(NULL);
	void* bits = malloc(alignedWidth * dibHeight * 3);///!!!!free
	hBitmap = CreateDIBSection(hDC, info, DIB_RGB_COLORS, &bits, NULL, 0);
	
	std::vector<Chart::Node*>::const_iterator nodeIterator;
	for(nodeIterator = pChart->GetNodesBegin(); nodeIterator != pChart->GetNodesEnd(); nodeIterator++)
	{
		Chart::Node* pNode = *nodeIterator;
		const Shape* pShape = pNode->GetShape();
		switch(pShape->GetShapeType())
		{
		case Shape::ShapeType_Ellipse:
			{
				EllipseShape* pEllipse = (EllipseShape*)pShape;
				DrawEllipse((unsigned char*)bits, dibWidth, pEllipse->GetDIBOrigin(), 
					pEllipse->GetDIBRadiusX(), pEllipse->GetDIBRadiusY());
			}
			break;
		case Shape::ShapeType_Rect:
			{
				RectShape* pRect = (RectShape*)pShape;
				DrawRect((unsigned char*)bits, dibWidth, pRect->GetDIBOrigin(), 
					pRect->GetDIBWidth(), pRect->GetDIBHeight());
			}
			break;
		}
	}
	
	std::vector<Chart::Link*>::const_iterator linkIterator;
	for(linkIterator = pChart->GetLinksBegin(); linkIterator != pChart->GetLinksEnd(); linkIterator++)
	{
		Chart::Link* pLink = *linkIterator;
		const Line* pLine = pLink->GetLine();

		std::vector<Point> lineListOfPoints(pLine->GetPointCount());
		std::copy(pLine->GetBegin(), pLine->GetEnd(), lineListOfPoints.begin());


		DrawLine((unsigned char*)bits, dibWidth, lineListOfPoints);
	}

	ReleaseDC(NULL, hDC);

	free(info);

	if(hBitmap)
		UpdateImage();
}
	
void VisualDebugger::DrawEllipse(unsigned char* pBits, long strip, const Point& center, double radiusX, double radiusY)
{
	long width = (long)(2 * radiusX);
	long height = (long)(2 * radiusY);
	//long originX = (long)(origin.x - radiusX + .5);
	//long originY = (long)(origin.y - radiusY + .5);
	Point origin = Point((long)(center.x - radiusX + .5), (long)(center.y - radiusY + .5));

	for(double x = -radiusX; x <= radiusX; x += .05f)
	{
		double y = radiusY * sqrt(1 - x * x / (radiusX * radiusX));

		long _x = (long)(radiusX + x);
		long _y1 = (long)(radiusY + y - 0.5);
		long _y2 = (long)(radiusY - y + 0.5);

		if(_x >= 0 && _x < width && _y1 >= 0 && _y1 < height)
		{
			pBits[((origin.y + _y1) * strip + origin.x + _x) * 3 + 0] = 255;
			pBits[((origin.y + _y1) * strip + origin.x + _x) * 3 + 1] = 255;
			pBits[((origin.y + _y1) * strip + origin.x + _x) * 3 + 2] = 255;
		}
		
		if(_x >= 0 && _x < width && _y2 >= 0 && _y2 < height)
		{
			pBits[((origin.y + _y2) * strip + origin.x + _x) * 3 + 0] = 255;
			pBits[((origin.y + _y2) * strip + origin.x + _x) * 3 + 1] = 255;
			pBits[((origin.y + _y2) * strip + origin.x + _x) * 3 + 2] = 255;
		}
	}
}
	
void VisualDebugger::DrawRect(unsigned char* pBits, long strip, const Point& origin, long width, long height)
{
	for(long dx = 0; dx < width; dx++)
	{
		pBits[((origin.y + 0) * strip + origin.x + dx) * 3 + 0] = 255;
		pBits[((origin.y + 0) * strip + origin.x + dx) * 3 + 1] = 255;
		pBits[((origin.y + 0) * strip + origin.x + dx) * 3 + 2] = 255;
		pBits[((origin.y + height - 1) * strip + origin.x + dx) * 3 + 0] = 255;
		pBits[((origin.y + height - 1) * strip + origin.x + dx) * 3 + 1] = 255;
		pBits[((origin.y + height - 1) * strip + origin.x + dx) * 3 + 2] = 255;
	}
	
	for(long dy = 1; dy < height - 1; dy++)
	{
		pBits[((origin.y + dy) * strip + origin.x) * 3 + 0] = 255;
		pBits[((origin.y + dy) * strip + origin.x) * 3 + 1] = 255;
		pBits[((origin.y + dy) * strip + origin.x) * 3 + 2] = 255;
		pBits[((origin.y + dy) * strip + origin.x + width - 1) * 3 + 0] = 255;
		pBits[((origin.y + dy) * strip + origin.x + width - 1) * 3 + 1] = 255;
		pBits[((origin.y + dy) * strip + origin.x + width - 1) * 3 + 2] = 255;
	}
}
	
void VisualDebugger::DrawLine(unsigned char* pBits, long strip, std::vector<Point> points)
{
	for(unsigned long pointIndex = 0; pointIndex < points.size() - 1; pointIndex++)
	{
		double x0 = points[pointIndex].x;
		double y0 = points[pointIndex].y;
		double x1 = points[pointIndex + 1].x;
		double y1 = points[pointIndex + 1].y;

		if(points[pointIndex] == points[pointIndex + 1])
		{
			continue;
		}

		double l = sqrt((x0 - x1) * (x0 - x1) + (y0 - y1) * (y0 - y1));
		double a = (x1 - x0) / l;
		double c = (y1 - y0) / l;

		for(double t = 0; t <= l; t += 1.0)
		{
			double x = a * t + x0;
			double y = c * t + y0;
			long _x = (long)(x + .5);
			long _y = (long)(y + .5);

			//if(_x >= 0 && _x < width && _y1 >= 0 && _y1 < height)
			{
				pBits[(_y * strip + _x) * 3 + 0] = 255;
				pBits[(_y * strip + _x) * 3 + 1] = 255;
				pBits[(_y * strip + _x) * 3 + 2] = 255;
			}
		}
	}
}