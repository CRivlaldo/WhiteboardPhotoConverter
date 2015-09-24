#pragma once

namespace WPC
{
	class ShapeMatcher
	{
	private:
		//BinaryBitmap* pBinaryBitmap;

		/*ShapeMatcher(BinaryBitmap* pBinaryBitmap, double matchLevel);
		~ShapeMatcher(void);*/
		static Shape* CreateShapeByType(Shape::ShapeType shapeType, const Point pixel, long width, long height, 
			long blockSize);

	public:
		static void TryToMatchShape(BinaryBitmap* pBinaryBitmap, const Point pixel, Shape*& pOutShape,
			double& match, Debugger* pDebugger);
	};	
}



