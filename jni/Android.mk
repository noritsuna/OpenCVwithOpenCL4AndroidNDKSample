LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

OPENCV_MODULES:=ocl contrib legacy ml stitching superres objdetect ts videostab video photo calib3d features2d highgui imgproc flann core androidcamera
OPENCV_3RDPARTY_COMPONENTS:=assetmanager libjpeg libpng libtiff libjasper IlmImf
OPENCV_CAMERA_MODULES:= native_camera_r2.2.0 native_camera_r2.3.3 native_camera_r3.0.1 native_camera_r4.0.0 native_camera_r4.0.3 native_camera_r4.1.1 native_camera_r4.2.0 native_camera_r4.3.0 native_camera_r4.4.0
#OPENCV_CAMERA_MODULES:= native_camera_r4.4.0


define add_opencv_module
    include $(CLEAR_VARS)
    LOCAL_MODULE:=opencv_$1
    LOCAL_SRC_FILES:=../libs_opencv/libopencv_$1.a
    include $(PREBUILT_STATIC_LIBRARY)
endef

define add_opencv_3rdparty_component
    include $(CLEAR_VARS)
    LOCAL_MODULE:=$1
    LOCAL_SRC_FILES:=../libs_opencv/lib$1.a
    include $(PREBUILT_STATIC_LIBRARY)
endef

define add_opencv_camera_module
    include $(CLEAR_VARS)
    LOCAL_MODULE:=$1
    LOCAL_SRC_FILES:=../libs_opencv/lib$1.so
    include $(PREBUILT_SHARED_LIBRARY)
endef

include $(CLEAR_VARS)
LOCAL_MODULE:=opencv_info
LOCAL_SRC_FILES:=../libs_opencv/libopencv_info.so
include $(PREBUILT_SHARED_LIBRARY)

$(foreach module,$(OPENCV_MODULES),$(eval $(call add_opencv_module,$(module))))
$(foreach module,$(OPENCV_3RDPARTY_COMPONENTS),$(eval $(call add_opencv_3rdparty_component,$(module))))
$(foreach module,$(OPENCV_CAMERA_MODULES),$(eval $(call add_opencv_camera_module,$(module))))



include $(CLEAR_VARS)

LOCAL_C_INCLUDES+=./jni/include

LOCAL_MODULE    := OpenCV_CL
LOCAL_SRC_FILES := OpenCVwithOpenCL4AndroidNDKNativeActivity.cpp

LOCAL_LDLIBS    += -lm -llog -lc -ldl -lz -landroid
LOCAL_STATIC_LIBRARIES := android_native_app_glue $(foreach mod, $(OPENCV_MODULES), opencv_$(mod)) $(OPENCV_3RDPARTY_COMPONENTS)

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)

