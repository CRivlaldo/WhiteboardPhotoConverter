#pragma once

#include <math.h>
#include "Prerequisites.h"
//#include <string>
//#include <list>
//#include <vector>
//#include <algorithm>
//#include <functional> 
//#include <cctype>
//#include <locale>
//
//#include "StringUtils.h"
//#include "FreeImage.h"
//#include "FreeImageUtils.h"
//#include "Point.h"
//#include "Rect.h"
//#include "QuickUnion.h"
//#include "BinaryBitmap.h"
//#include "Shapes.h"
//#include "Lines.h"
//#include "Debugger.h"
//#include "Chunk.h"
//#include "Chart.h"
//#include "ChartExporter.h"

namespace WPC
{
	class WhiteboardPhotoConverter
	{
	private:
		std::string inputFileName;
		Debugger* pDebugger;
		FIBITMAP* pFIBitmap;
		PBITMAPINFO pFIBitmapInfo;
		std::list<Shape*> shapes;
		std::list<TextGlyph> textGlyphs;
		std::list<Line*> lines;
		std::string tesseractDataPath;

		WhiteboardPhotoConverter(const std::string inputFileName, const std::string& tesseractDataPath);
		WhiteboardPhotoConverter(const std::string inputFileName, const std::string& tesseractDataPath, Debugger* pDebugger);

		bool Init();
		bool Shutdown();
		bool ConvertToDiagramInternal(Chart*& pOutChart, std::string& errorMessage);
	public:
		static bool ConvertToDiagram(const std::string& inputFileName, const std::string& tesseractDataPath,
				Chart*& pOutChart, std::string& errorMessage);
		static bool ConvertToDiagram(const std::string& inputFileName, const std::string& tesseractDataPath,
				Chart*& pOutChart, std::string& errorMessage, Debugger* pDebugger);

		~WhiteboardPhotoConverter();
	};
}
