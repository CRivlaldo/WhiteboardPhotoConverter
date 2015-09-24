#pragma once

namespace WPC
{
	class ShapeRecognizer
	{
	private:
		QuickUnion regionUnion;
		QuickUnion chunkUnion;
		BinaryBitmap* pBinaryBitmap;
		Debugger* pDebugger;
		long lightPixelIndex;
		long outSurfaceIndex;
		long backgroundPixelIndex;
		double matchLevel;
		std::list<Shape*>* pShapes;
		std::list<Line*>* pLines;

		ShapeRecognizer(BinaryBitmap* pBinaryBitmap, double matchLevel, std::list<Shape*>* pShapes,
			std::list<Line*>* pLines);
		ShapeRecognizer(BinaryBitmap* pBinaryBitmap, double matchLevel, std::list<Shape*>* pShapes,
			std::list<Line*>* pLines, Debugger* pDebugger);
		~ShapeRecognizer(void);

		void InitInternal(BinaryBitmap* pBinaryBitmap, double matchLevel, Debugger* pDebugger);
		bool RecognizeShapesAndLines();
		bool RecognizeShapes();
		bool RecognizeLines();
		void InitRegionUnion();
		void InitChunkUnion();
		bool FindAndRecognizeShape(long& startY, std::list<long>& pointsFromNotRecognizedRegions);
		bool FindAndRecognizeLine(long& startY, std::list<long>& pointsFromNotRecognizedRegions,
			long& chunkIndex);
		bool IsPixelConnectedToNotRecognizedRegion(long x, long y, 
			std::list<long>& pointsFromNotRecognizedRegions);
		void RecreateRegionUnion(const Rect& region);
		//void RecreateChunkUnion(const Rect& region);
		void ExpandRegion(Rect& region);
		void FillHoles();

	public:
		static bool RecognizeShapes(BinaryBitmap* pBinaryBitmap, double matchLevel, 
			std::list<Shape*>& shapes, std::list<Line*>& lines, Debugger* pDebugger);
	};
}
