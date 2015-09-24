#include <android/log.h>

#include "WhiteboardPhotoConverter.h"
#include "ConsoleDebugger.h"

static ConsoleDebugger* pVDInstance = NULL;

using namespace WPC;

ConsoleDebugger::ConsoleDebugger(void)
{
	pVDInstance = this;
}

ConsoleDebugger::~ConsoleDebugger(void)
{
	pVDInstance = NULL;
}

void ConsoleDebugger::WaitForContinue()
{
	__android_log_write(ANDROID_LOG_INFO, "WPC Console Debugger", "...\n");
}
	
void ConsoleDebugger::ShowImageFromFile(const std::string& fileName)
{
	std::string message = "WPC wants to show " + fileName;
	__android_log_write(ANDROID_LOG_INFO, "WPC Console Debugger", message.c_str());
}

void ConsoleDebugger::ShowBinaryImage(WPC::BinaryBitmap* pBinaryBitmap)
{
	char buffer[1024];
	sprintf(buffer, "WPC wants to show binary bitmap (%d x %d)", pBinaryBitmap->GetWidth(),
			pBinaryBitmap->GetHeight());

	__android_log_write(ANDROID_LOG_INFO, "WPC Console Debugger", buffer);
}

void ConsoleDebugger::ShowBinaryImageWithRegions(BinaryBitmap* pBinaryBitmap, QuickUnion* pRegions)
{
	ShowBinaryImageWithRegionsAndShapes(pBinaryBitmap, pRegions, std::list<Shape*>(), std::list<Line*>());
}
	
void ConsoleDebugger::ShowBinaryImageWithRects(BinaryBitmap* pBinaryBitmap, const std::list<Rect>& rectList)
{
	char buffer[1024];
	sprintf(buffer, "WPC wants to show binary bitmap (%d x %d) with %d rect(s)",
			pBinaryBitmap->GetWidth(), pBinaryBitmap->GetHeight(), rectList.size());
	__android_log_write(ANDROID_LOG_INFO, "WPC Console Debugger", buffer);

	std::list<Rect>::const_iterator rectIterator;
	for(rectIterator = rectList.begin(); rectIterator != rectList.end(); rectIterator++)
	{
		const Rect rect = *rectIterator;

		sprintf(buffer, "Rect: left = %d, top = %d, bottom = %d, right = %d",
				rect.GetMin().x, rect.GetMin().y, rect.GetMax().x, rect.GetMax().y);
		__android_log_write(ANDROID_LOG_INFO, "WPC Console Debugger", buffer);
	}

	__android_log_write(ANDROID_LOG_INFO, "WPC Console Debugger", "-------------------------------------");
}
	
void ConsoleDebugger::ShowBinaryImageWithRegionsAndShapes(BinaryBitmap* pBinaryBitmap, QuickUnion* pRegions,
														 const std::list<Shape*>& shapeList, 
														 const std::list<Line*>& lineList)
{
	char buffer[1024];
	sprintf(buffer, "WPC wants to show binary bitmap (%d x %d) with %d shape(s) and %d line(s)",
			pBinaryBitmap->GetWidth(), pBinaryBitmap->GetHeight(), shapeList.size(), lineList.size());
	__android_log_write(ANDROID_LOG_INFO, "WPC Console Debugger", buffer);

	std::list<Shape*>::const_iterator shapeIterator;
	for(shapeIterator = shapeList.begin(); shapeIterator != shapeList.end(); shapeIterator++)
	{
		Shape* pShape = *shapeIterator;
		switch(pShape->GetShapeType())
		{
		case Shape::ShapeType_Ellipse:
			{
				EllipseShape* pEllipse = (EllipseShape*)pShape;

				sprintf(buffer, "Ellipse: x = %d, y = %d, radiusX = %d, radiusY = %d",
						pEllipse->GetOrigin().x, pEllipse->GetOrigin().y,
						pEllipse->GetRadiusX(), pEllipse->GetRadiusY());
				__android_log_write(ANDROID_LOG_INFO, "WPC Console Debugger", buffer);
			}
			break;
		case Shape::ShapeType_Rect:
			{
				RectShape* pRect = (RectShape*)pShape;

				sprintf(buffer, "Rect: x = %d, y = %d, radiusX = %d, radiusY = %d",
						pRect->GetOrigin().x, pRect->GetOrigin().y,
						pRect->GetWidth(), pRect->GetHeight());
				__android_log_write(ANDROID_LOG_INFO, "WPC Console Debugger", buffer);
			}
			break;
		}
	}
	
	std::list<Line*>::const_iterator lineIterator;
	for(lineIterator = lineList.begin(); lineIterator != lineList.end(); lineIterator++)
	{
		Line* pLine = *lineIterator;

		sprintf(buffer, "Line: begin = (%d, %d), end = (%d, %d)",
				pLine->GetBegin()->x, pLine->GetBegin()->y,
				pLine->GetEnd()->x, pLine->GetEnd()->y);
		__android_log_write(ANDROID_LOG_INFO, "WPC Console Debugger", buffer);
	}

	__android_log_write(ANDROID_LOG_INFO, "WPC Console Debugger", "-------------------------------------");
}

void ConsoleDebugger::ShowChart(const Chart* pChart)
{
	char buffer[1024];
	sprintf(buffer, "WPC wants to show chart (%d x %d)", pChart->GetWidth(), pChart->GetHeight());
	__android_log_write(ANDROID_LOG_INFO, "WPC Console Debugger", buffer);

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

				sprintf(buffer, "Ellipse: x = %d, y = %d, radiusX = %d, radiusY = %d",
						pEllipse->GetOrigin().x, pEllipse->GetOrigin().y,
						pEllipse->GetRadiusX(), pEllipse->GetRadiusY());
				__android_log_write(ANDROID_LOG_INFO, "WPC Console Debugger", buffer);
			}
			break;
		case Shape::ShapeType_Rect:
			{
				RectShape* pRect = (RectShape*)pShape;

				sprintf(buffer, "Rect: x = %d, y = %d, radiusX = %d, radiusY = %d",
						pRect->GetOrigin().x, pRect->GetOrigin().y,
						pRect->GetWidth(), pRect->GetHeight());
				__android_log_write(ANDROID_LOG_INFO, "WPC Console Debugger", buffer);
			}
			break;
		}
	}
	
	std::vector<Chart::Link*>::const_iterator linkIterator;
	for(linkIterator = pChart->GetLinksBegin(); linkIterator != pChart->GetLinksEnd(); linkIterator++)
	{
		Chart::Link* pLink = *linkIterator;
		const Line* pLine = pLink->GetLine();

		sprintf(buffer, "Line: begin = (%d, %d), end = (%d, %d)",
				pLine->GetBegin()->x, pLine->GetBegin()->y,
				pLine->GetEnd()->x, pLine->GetEnd()->y);
		__android_log_write(ANDROID_LOG_INFO, "WPC Console Debugger", buffer);
	}

	__android_log_write(ANDROID_LOG_INFO, "WPC Console Debugger", "-------------------------------------");
}

void ConsoleDebugger::ShowMessage(const std::string& message)
{
	__android_log_write(ANDROID_LOG_INFO, "WPC Console Debugger", message.c_str());
}
