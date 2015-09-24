package com.sofatech.whiteboardphotoconverter;

import android.R.bool;

public class WhiteBoardPhotoConverterNativeWrapper {
	
	static {
        System.loadLibrary("leptonica");
        System.loadLibrary("tesseract");
        System.loadLibrary("WhiteboardPhotoConverter");
    }

	public static boolean convertImageToSVG(String inputPath, String outputPath, String tesseractDataPath) {
		return convertImageToSVGNative(inputPath, outputPath, tesseractDataPath);
	}
	
    private static native boolean convertImageToSVGNative(String inputPath, String outputPath, String tesseractDataPath);
}
