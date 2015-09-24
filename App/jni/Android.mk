LOCAL_PATH := $(call my-dir)
TESSERACT_PATH := $(LOCAL_PATH)/com_sofatech_tesseract/src
LEPTONICA_PATH := $(LOCAL_PATH)/com_sofatech_leptonica/src
WHITEBOARDPHOTOCONVERTER_PATH := $(LOCAL_PATH)/com_sofatech_whiteboardphotoconverter
FREEIMAGE_PATH := $(LOCAL_PATH)/com_sofatech_freeimage
SVGCHARTEXPORTER_PATH := $(LOCAL_PATH)/com_sofatech_svgchartexporter

# Just build the Android.mk files in the subdirs
include $(call all-subdir-makefiles)