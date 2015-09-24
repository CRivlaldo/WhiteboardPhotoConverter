#pragma once

namespace WPC
{
	class Chunk
	{
	private:
		std::vector<Point> pixels;
		std::vector<std::pair<long, long>> adjacencyList;///!!!!it's better use adjencency matrix ??? what if we have not got const indices??

		void AddAdjacency(long index0, long index1);
		bool HasAdjacency(long index0, long index1) const;
		void FindAndCreatePixelAdjacency(const Point& pixel, long index);
		
	protected:
		BinaryBitmap* pBitmap;
		Point minPixel;
		Point maxPixel;

		Chunk(BinaryBitmap* pBitmap);

		void AddPixel(long x, long y);
		/*bool Contains(long x, long y) const { return Contains(Point(x, y)); }*/
		bool Contains(const Point& pixel) const;
		/*long FindPixel(long x, long y) const { return FindPixel(Point(x, y)); }*/
		long FindPixel(const Point& pixel) const;
		double GetDistanceBetweenPoints(long index0, long index1) const;

	public:
		virtual ~Chunk(void);

		std::vector<Point>::const_iterator GetBegin() const;
		std::vector<Point>::const_iterator GetEnd() const;
		double GetWeight(long index) const;
		long GetCount() const { return pixels.size(); }		
		virtual void EraseFromBitmap() const;
		void GetPathLengthesFromPixelToPixel(long index, double* pDistances) const;
		std::list<long> GetPathFromPixelToPixel(long startIndex, long endIndex) const;
		Point GetOrigin() const { return minPixel; }
		long GetWidth() const { return maxPixel.x - minPixel.x + 1; }
		long GetHeight() const { return maxPixel.y - minPixel.y + 1; }
		Point GetPixel(long index) const { return pixels[index]; }
		Rect GetRegion() const;

		static const Chunk CreateChunkFromUnion(BinaryBitmap* pBitmap, const QuickUnion& chunkUnion, 
			long chunkIndex);
	};
}