#include "Prerequisites.h"

using namespace WPC;

FIBITMAP* FreeImageUtils::LoadInputFile(const std::string&  path)
{
	FREE_IMAGE_FORMAT fif = FreeImage_GetFIFFromFilename(path.c_str());

	if(fif != FIF_UNKNOWN)
		return FreeImage_Load(fif, path.c_str());

	return NULL;
}
		
void FreeImageUtils::UnloadBitmap(FIBITMAP* pBitmap)
{
	FreeImage_Unload(pBitmap);
}

bool FreeImageUtils::ConvertInputPhotoToBMP(const std::string& inputPath, const std::string& outputPath)
{
	FIBITMAP* pFIB = LoadInputFile(inputPath);
	if(!pFIB)
		return false;
	
	try
    {
		FreeImage_Save(FIF_BMP, pFIB, outputPath.c_str());
    }
    catch(...)
    {
		return false;
    }

	UnloadBitmap(pFIB);

	return true;
}

bool FreeImageUtils::Init()
{
// call this ONLY when linking with FreeImage as a static library
#ifdef FREEIMAGE_LIB
	FreeImage_Initialise();
#endif // FREEIMAGE_LIB

	return true;
}

bool FreeImageUtils::Shutdown()
{
#ifdef FREEIMAGE_LIB
	FreeImage_DeInitialise();
#endif // FREEIMAGE_LIB

	return true;
}
