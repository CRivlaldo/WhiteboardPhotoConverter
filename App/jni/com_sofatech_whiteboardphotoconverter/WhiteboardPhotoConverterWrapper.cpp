/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <assert.h>
#include <string>
#include <jni.h>
#include <android/log.h>

#include "WhiteboardPhotoConverter.h"
#include "ConsoleDebugger.h"

#include "simple_svg_1.0.0.hpp"
#include "SVGChartExporter.h"

using namespace WPC;

extern "C" JNIEXPORT bool JNICALL
Java_com_sofatech_whiteboardphotoconverter_WhiteBoardPhotoConverterNativeWrapper_convertImageToSVGNative(
		JNIEnv* env, jclass thiz, jstring inputPath, jstring outputPath, jstring tesseractDataPath ) {
    const char* intputPathUTF8 = env->GetStringUTFChars(inputPath, NULL);
    assert(NULL != intputPathUTF8);
    const char* outputPathUTF8 = env->GetStringUTFChars(outputPath, NULL);
    assert(NULL != outputPathUTF8);
    const char* tesseractDataPathUTF8 = env->GetStringUTFChars(tesseractDataPath, NULL);
    assert(NULL != tesseractDataPathUTF8);

    std::string errorText("");
    Chart* pChart;

    ConsoleDebugger* pDebugger = new ConsoleDebugger();

    bool success = WhiteboardPhotoConverter::ConvertToDiagram(intputPathUTF8, tesseractDataPathUTF8, pChart,
    		errorText, pDebugger);

    if(success)
    {
    	SVGChartExporter svgExporter(pChart);
    	if(!svgExporter.SaveChartToFile(outputPathUTF8, errorText))
    	{
    		success = false;
        	__android_log_write(ANDROID_LOG_INFO, "WPC Error", errorText.c_str());//!!!vladimir: push this error outside!

    		delete pChart;
    	}
    }
    else
    	__android_log_write(ANDROID_LOG_INFO, "WPC Error", errorText.c_str());//!!!vladimir: push this error outside!

    env->ReleaseStringUTFChars(inputPath, intputPathUTF8);
    env->ReleaseStringUTFChars(outputPath, outputPathUTF8);

    delete pDebugger;

	return success;
}
