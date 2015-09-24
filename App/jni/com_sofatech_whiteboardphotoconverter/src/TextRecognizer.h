#pragma once

namespace WPC
{
	class TextRecognizer
	{
	private:
		std::string tesseractDataPath;
		tesseract::TessBaseAPI *pAPI;
		BinaryBitmap* pBinaryBitmap;
		std::list<Rect> textAreas;
		Debugger* pDebugger;
		std::list<TextGlyph> textGlyphs;

		TextRecognizer(BinaryBitmap* pBinaryBitmap,	const std::string& tesseractDataPath,
				Debugger* pDebugger);
		bool RecognizeTextInternal(std::list<TextGlyph>& outTextGlyphs, std::string& errorText);
		PIX* CreateImageByBinaryBitmap();
		PIX* CreateImageByBinaryBitmap(const Rect& rect);///!!!test
		bool ReconginzeTextBlocks(std::string& errorText);
		void FindTextAreas();
		void ConnectedComponentAnalisys(BinaryBitmap* pBitmap, long lowerThreshold, long higherThreshold, 
			double factor);
		void RemoveTextIntersections(std::list<TextGlyph>& outTextGlyphs);
		void RemoveTextFromBitmap(std::list<TextGlyph>& textGlyphs);
		//bool CutRim(TextGlyph& text);
		std::string RecognizeTextFromRect(const Rect& rect, int& confidence);
		std::string RecognizeTextFromRect(const Rect& rect);

	public:
		~TextRecognizer(void);

		static bool RecognizeText(BinaryBitmap* pBinaryBitmap, std::list<TextGlyph>& textGlyphs, 
				const std::string& tesseractDataPath, std::string& errorText, Debugger* pDebugger);
	};
}
