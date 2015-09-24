#include "Prerequisites.h"

using namespace WPC;

//ShapeMatcher::ShapeMatcher(BinaryBitmap* pBinaryBitmap, double matchLevel)
//{
//	this->pBinaryBitmap = pBinaryBitmap;
//}
//
//ShapeMatcher::~ShapeMatcher(void)
//{
//}

Shape* ShapeMatcher::CreateShapeByType(Shape::ShapeType shapeType, const Point pixel, long width, long height, long blockSize)
{
	switch(shapeType)
	{
	case Shape::ShapeType_Rect:
		return new RectShape(pixel, width, height, blockSize);
	case Shape::ShapeType_Ellipse:
		return new EllipseShape(pixel + Point(width / 2, height / 2), width / 2.0, height / 2.0, blockSize);
	}

	return NULL;
}

void ShapeMatcher::TryToMatchShape(BinaryBitmap* pBinaryBitmap, const Point pixel, Shape*& pMatchedShape, 
								   double& match, Debugger* pDebugger = NULL)
{
	pMatchedShape = NULL;
	match = 0;

	for(int shapeTypeIndex = 1; shapeTypeIndex < Shape::ShapeType_Count; shapeTypeIndex++)
	{
		if(!Shape::IsShapeForMatching((Shape::ShapeType)shapeTypeIndex))
			continue;

		Shape* pShape = CreateShapeByType((Shape::ShapeType)shapeTypeIndex, pixel, pBinaryBitmap->GetWidth(), 
			pBinaryBitmap->GetHeight(), pBinaryBitmap->GetBlockSize());

		if(pDebugger)
		{
			BinaryBitmap shapeBB(pBinaryBitmap->GetWidth(), pBinaryBitmap->GetHeight(), pBinaryBitmap->GetBlockSize());
			pShape->FillBinaryBitmap(shapeBB);
			printf("shapeType = %d, match = %.3f\n", shapeTypeIndex, pShape->GetMatch(pBinaryBitmap));///!!!!vladimir debug

			pDebugger->ShowBinaryImage(&shapeBB);
			pDebugger->WaitForContinue();

		}

		double shapeMatch = pShape->GetMatch(pBinaryBitmap);
		if(shapeMatch >= match)
		{
			if(pMatchedShape)
				delete pMatchedShape;

			pMatchedShape = pShape;
			match = shapeMatch;
		}
		else
			delete pShape;
	}
}