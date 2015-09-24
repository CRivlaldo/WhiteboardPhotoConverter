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

WhiteboardPhotoConverter::WhiteboardPhotoConverter(const std::string inputFileName,
		const std::string& tesseractDataPath)
{
	this->inputFileName = std::string(inputFileName);
	this->tesseractDataPath = tesseractDataPath;
	this->pDebugger = NULL;	
}

WhiteboardPhotoConverter::WhiteboardPhotoConverter(const std::string inputFileName,
		const std::string& tesseractDataPath, Debugger* pDebugger)
{
	this->inputFileName = std::string(inputFileName);	
	this->tesseractDataPath = tesseractDataPath;
	this->pDebugger = pDebugger;
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
		
bool WhiteboardPhotoConverter::ConvertToDiagram(const std::string& inputFileName,
												const std::string& tesseractDataPath, Chart*& pOutChart,
												std::string& errorMessage)
{
	return ConvertToDiagram(inputFileName, tesseractDataPath, pOutChart, errorMessage, NULL);
}

bool WhiteboardPhotoConverter::ConvertToDiagramInternal(Chart*& pOutChart, std::string& errorMessage)
{
	if(!Init())
	{
		errorMessage = "Could not initialize converter";
		return false;
	}
	
	this->pFIBitmap = FreeImageUtils::LoadInputFile(inputFileName);
	if(!this->pFIBitmap)
	{
		errorMessage = "Could not load input file";
		return false;
	}

	this->pFIBitmapInfo = FreeImage_GetInfo(pFIBitmap);

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


//	//!!!!vladimir: degug
//	{
//		char buffer[1024];
//
//		sprintf(buffer, "pFIBitmapInfo = %p", pFIBitmapInfo);
//		pDebugger->ShowMessage(buffer);
//
//		sprintf(buffer, "pFIBitmapInfo->bmiHeader.biWidth = %ld,  pFIBitmapInfo->bmiHeader.biHeight = %ld",
//				pFIBitmapInfo->bmiHeader.biWidth, pFIBitmapInfo->bmiHeader.biHeight);
//		pDebugger->ShowMessage(buffer);
//	}

	//std::string binarizedPath = StringUtils::ChangeExtension(inputFileName, ".bmp");
	BinaryBitmap binaryBitmap(pFIBitmapInfo->bmiHeader.biWidth, pFIBitmapInfo->bmiHeader.biHeight);

	pDebugger->ShowMessage("Binarizing image...");//!!!!vladimir: debug

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
	
	pDebugger->ShowMessage("Recognizing text...");//!!!!vladimir: debug

	if(!TextRecognizer::RecognizeText(&binaryBitmap, textGlyphs, tesseractDataPath, errorMessage, pDebugger))
	{
		return false;
	}

	if(pDebugger)
	{
		//pDebugger->ShowImageFromFile(binarizedPath);
		pDebugger->ShowBinaryImage(&binaryBitmap);
		pDebugger->WaitForContinue();
	}

	pDebugger->ShowMessage("Reducing bitmap...");//!!!!vladimir: debug

	BinaryBitmap reducedBB = binaryBitmap.Reduce(10, 5);///!!!need options or calculations

	if(pDebugger)
	{
		pDebugger->ShowBinaryImage(&reducedBB);
		pDebugger->WaitForContinue();
	}

	pDebugger->ShowMessage("Recognizing shapes...");//!!!!vladimir: debug

	if(!ShapeRecognizer::RecognizeShapes(&reducedBB, .7, shapes, lines, pDebugger)) ///!!!.7 to options
	{
		errorMessage = "Could not recognize shapes";
		return false;
	}
	
	pDebugger->ShowMessage("Recognizing chart...");//!!!!vladimir: debug

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

bool WhiteboardPhotoConverter::ConvertToDiagram(const std::string& inputFileName,
												const std::string& tesseractDataPath, Chart*& pOutChart,
												std::string& errorMessage, Debugger* pDebugger)
{	
	WhiteboardPhotoConverter wpc(inputFileName, tesseractDataPath, pDebugger);
	return wpc.ConvertToDiagramInternal(pOutChart, errorMessage);	
}
