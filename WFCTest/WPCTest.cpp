// WFCTest.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

//#include "WhiteboardPhotoConverter.h"
//#include "StringUtils.h"

#include "VisualDebugger.h"

using namespace WPC;

bool debugMode = false;
VisualDebugger* pDebugger = NULL;

void Fatal(const std::string& message)
{
	printf("Fatal:\n%s\n", message.c_str());
}

bool ParseCommandLineFlags(int argc, _TCHAR* argv[])
{
	for(int i = 2; i < argc; i++)
	{
		const std::string flag = StringUtils::ConvertWStringToString(argv[i]);
		if(flag == "-d")
		{
			debugMode = true;
			continue;
		}

		Fatal(flag + std::string(" is unknown flag"));
	}

	return true;
}

bool RunConverter(int argc, _TCHAR* argv[])
{	
	if(argc < 1)
	{
		Fatal("Not enough parameters");
		return false;
	}

	ParseCommandLineFlags(argc, argv);

	if(debugMode)
		pDebugger = new VisualDebugger();
	
	std::string errorText("");
	Chart* pChart;

	const std::string inputFileName = StringUtils::ConvertWStringToString(argv[1]);
	const std::string outputFileName = StringUtils::ChangeExtension(inputFileName, ".svg");

	bool success = WhiteboardPhotoConverter::ConvertToDiagram(inputFileName, pChart, errorText, pDebugger);

	if(pDebugger)
		delete pDebugger;

	if(!success)
		Fatal(errorText);

	if(success)
	{
		SVGChartExporter svgExporter(pChart);
		if(!svgExporter.SaveChartToFile(outputFileName, errorText))
		{
			success = false;
			Fatal(errorText);
		}

		delete pChart;
	}
	
	return success;
}

int _tmain(int argc, _TCHAR* argv[])
{
	if(!RunConverter(argc, argv))
	{
		getchar();
		return 1;
	}

	return 0;
}

