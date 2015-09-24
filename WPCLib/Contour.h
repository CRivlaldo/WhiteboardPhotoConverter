#pragma once

#define CONTOUR_PIXEL_DIRECTION_COUNT 4

namespace WPC
{
	class Contour : public Chunk
	{
	private:
		class ContourBypasser
		{
		private:
			enum ContourPixelDirection
			{
				Left,
				Up,
				Right,
				Down
			};
		
			struct ContourBypasserPixel : Point
			{
				/*long x;
				long y;*/
				ContourPixelDirection direction;
				
				ContourBypasserPixel();
				ContourBypasserPixel(const Point& pixel, ContourPixelDirection direction);
			};

		private:
			ContourBypasserPixel startPixel;
			ContourBypasserPixel currentPixel;
			Contour* pContour;
			BinaryBitmap* pBinaryBitmap;

			void GoToNextPixel(int attempt);
			bool CanMoveToPixel(const Point& pixel) const;
			void AddAroundPixels();

			static ContourPixelDirection GetNextContourPixelDirection(ContourPixelDirection direction);
			static ContourPixelDirection GetPreviousContourPixelDirection(ContourPixelDirection direction);

		public:
			ContourBypasser(Contour* pContour, BinaryBitmap* pBinaryBitmap, const Point& start);

			bool EndOfContour() const;
			void Next();
		};

	private:
		Contour(BinaryBitmap* pBinaryBitmap) : Chunk(pBinaryBitmap) {}

		bool IsPixelOutsideContour(const Point& pixel) const;

	public:

		BinaryBitmap ToBinaryBitmap() const;
		void EraseFromBitmap() const;
		
		static const Contour CreateContourOfRegion(BinaryBitmap* pBinaryBitmap, const Point& start);
	};
}