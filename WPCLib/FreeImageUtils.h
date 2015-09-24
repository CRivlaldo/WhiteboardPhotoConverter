#pragma once

namespace WPC
{
	class FreeImageUtils
	{
	public:
		static bool Init();
		static bool Shutdown();
		static FIBITMAP* LoadInputFile(const std::string& path);
		static void UnloadBitmap(FIBITMAP* pBitmap);
		static bool ConvertInputPhotoToBMP(const std::string& inputPath, const std::string& outputPath);
	};
}

