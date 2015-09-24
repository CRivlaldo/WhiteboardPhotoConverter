//#include <vector>
//#include <iostream>
//#include <clocale>
//#include <locale>
//#include <string>
//#include <list>
#include "allheaders.h"
#include "baseapi.h"
#include "strngs.h"

#include "WhiteboardPhotoConverter.h"
#include "ImageBinarization.h"
#include "ShapeMatcher.h"
#include "ShapeRecognizer.h"
#include "LineRecognizer.h"
#include "TopologyRecognizer.h"
#include "TextRecognizer.h"

using namespace WPC;

WhiteboardPhotoConverter::WhiteboardPhotoConverter(const std::string inputFileName)
{
	this->inputFileName = std::string(inputFileName);
	this->pDebugger = NULL;	
	this->pFIBitmap = FreeImageUtils::LoadInputFile(inputFileName);
	this->pFIBitmapInfo = FreeImage_GetInfo(pFIBitmap);
}

WhiteboardPhotoConverter::WhiteboardPhotoConverter(const std::string inputFileName, Debugger* pDebugger)
{
	this->inputFileName = std::string(inputFileName);	
	this->pDebugger = pDebugger;
	this->pFIBitmap = FreeImageUtils::LoadInputFile(inputFileName);
	this->pFIBitmapInfo = FreeImage_GetInfo(pFIBitmap);
}

WhiteboardPhotoConverter::~WhiteboardPhotoConverter()
{
	std::list<Line*>::iterator lineIterator;
	for(lineIterator = lines.begin(); lineIterator != lines.end(); lineIterator++)
	{
		delete *lineIterator;
	}
	
	std::list<Shape*>::iterator shapeterator;
	for(shapeterator = shapes.begin(); shapeterator != shapes.end(); shapeterator++)
	{
		delete *shapeterator;
	}
}

bool WhiteboardPhotoConverter::Init()
{
	//std::setlocale(LC_ALL, "");
	
	if(!FreeImageUtils::Init())
		return false;

	return true;
}

bool WhiteboardPhotoConverter::Shutdown()
{
	if(!FreeImageUtils::Shutdown())
		return false;

	return true;
}
		
bool WhiteboardPhotoConverter::ConvertToDiagram(const std::string inputFileName, Chart*& pOutChart, 
												std::string& errorMessage)
{
	return ConvertToDiagram(inputFileName, pOutChart, errorMessage, NULL);
}

bool WhiteboardPhotoConverter::ConvertToDiagramInternal(Chart*& pOutChart, std::string& errorMessage)
{
	if(!Init())
	{
		errorMessage = "Could not initialize converter";
		return false;
	}
	
	/*std::string bmpPath = StringUtils::ChangeExtension(inputFileName, ".bmp");
	if(!FreeImageUtils::ConvertInputPhotoToBMP(inputFileName, bmpPath))
	{
		errorMessage = "Could not convert photo to BMP-file";
		return false;
	}*/

	if(pDebugger)
	{
		pDebugger->ShowImageFromFile(inputFileName);
		pDebugger->WaitForContinue();
	}

	//std::string binarizedPath = StringUtils::ChangeExtension(inputFileName, ".bmp");
	BinaryBitmap binaryBitmap(pFIBitmapInfo->bmiHeader.biWidth, pFIBitmapInfo->bmiHeader.biHeight);

	if(!ImageBinarization::BinarizeImage(inputFileName, pFIBitmap, pFIBitmapInfo, &binaryBitmap))
	{
		errorMessage = "Could not binarize image";
		return false;
	}

	if(pDebugger)
	{
		//pDebugger->ShowImageFromFile(binarizedPath);
		pDebugger->ShowBinaryImage(&binaryBitmap);
		pDebugger->WaitForContinue();
	}
	
	if(!TextRecognizer::RecognizeText(&binaryBitmap, textGlyphs, errorMessage, pDebugger))
	{
		return false;
	}

	if(pDebugger)
	{
		//pDebugger->ShowImageFromFile(binarizedPath);
		pDebugger->ShowBinaryImage(&binaryBitmap);
		pDebugger->WaitForContinue();
	}

	BinaryBitmap reducedBB = binaryBitmap.Reduce(10, 5);///!!!need options or calculations

	if(pDebugger)
	{
		pDebugger->ShowBinaryImage(&reducedBB);
		pDebugger->WaitForContinue();
	}

	if(!ShapeRecognizer::RecognizeShapes(&reducedBB, .7, shapes, lines, pDebugger)) ///!!!.7 to options
	{
		errorMessage = "Could not recognize shapes";
		return false;
	}
	
	if(!TopologyRecognizer::TryToRecognizeChart(&reducedBB, shapes, lines, textGlyphs, pOutChart))
	{
		errorMessage = "Could not recognize chart topology";
		return false;
	}

	if(pDebugger)
	{
		pDebugger->ShowChart(pOutChart);
		pDebugger->WaitForContinue();
	}

	if(!Shutdown())
	{
		errorMessage = "Could not shutdown converter";
		return false;
	}

	return true;
}

bool WhiteboardPhotoConverter::ConvertToDiagram(const std::string inputFileName, Chart*& pOutChart, 
												std::string& errorMessage, Debugger* pDebugger)
{	
	WhiteboardPhotoConverter wpc(inputFileName, pDebugger);
	return wpc.ConvertToDiagramInternal(pOutChart, errorMessage);	
}