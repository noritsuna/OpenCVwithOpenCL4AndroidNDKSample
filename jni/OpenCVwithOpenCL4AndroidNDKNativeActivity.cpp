/*
   OpenCV with OpenCL for Android NDK
   Copyright (c) 2006-2014 SIProp Project http://www.siprop.org/

   This software is provided 'as-is', without any express or implied warranty.
   In no event will the authors be held liable for any damages arising from the use of this software.
   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it freely,
   subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
   2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
   3. This notice may not be removed or altered from any source distribution.
*/
#include <android_native_app_glue.h>
#include <android/log.h>

#include <errno.h>
#include <sys/time.h>
#include <time.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <math.h>
#include <queue>

#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/cvaux.h>
#include <opencv/ml.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ocl/ocl.hpp>
#include <assetmanager.h>


#define  LOG_TAG    "NativeActivity"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

//for Android5.0 or later
//#define DATA_PATH "/data/app"
#define DATA_PATH "/data/data"
#define PACKAGE_NAME "org.siprop.android.opencl"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define WIDTH 80
#define HEIGHT 60


struct Engine {
	struct android_app* app;
};

int createAssetFile(const char *image_name) {
	const char package_name[] = PACKAGE_NAME;
	int result = 0;
	result = loadAseetFile(package_name, image_name);
	return result;
}

static void engine_draw_frame(Engine* engine, const cv::Mat& frame) {
    if (engine->app->window == NULL)
        return; // No window.

    cv::Mat tmp_frame(cv::Size(HEIGHT, WIDTH), frame.type(), cv::Scalar(0, 0, 0));
    cv::Mat rot_frame(cv::Size(WIDTH, HEIGHT), frame.type());
    cv::transpose(frame, tmp_frame);
    cv::flip(tmp_frame, tmp_frame, 1);
    cv::resize(tmp_frame, rot_frame, rot_frame.size(), cv::INTER_CUBIC);
    LOGI("Finish rotate frame.");

    ANativeWindow_Buffer buffer;
    if (ANativeWindow_lock(engine->app->window, &buffer, NULL) < 0) {
        LOGW("Unable to lock window buffer");
        return;
    }

    int32_t* pixels = (int32_t*)buffer.bits;

    int left_indent = (buffer.width-rot_frame.cols)/2;
    int top_indent = (buffer.height-rot_frame.rows)/2;

    if (top_indent > 0) {
        memset(pixels, 0, top_indent*buffer.stride*sizeof(int32_t));
        pixels += top_indent*buffer.stride;
    }

    for (int yy = 0; yy < rot_frame.rows; yy++) {
        if (left_indent > 0){
            memset(pixels, 0, left_indent*sizeof(int32_t));
            memset(pixels+left_indent+rot_frame.cols, 0, (buffer.stride-rot_frame.cols-left_indent)*sizeof(int32_t));
        }
        int32_t* line = pixels + left_indent;
        size_t line_size = rot_frame.cols*4*sizeof(unsigned char);
        memcpy(line, rot_frame.ptr<unsigned char>(yy), line_size);
        // go to next line
        pixels += buffer.stride;
    }
    ANativeWindow_unlockAndPost(engine->app->window);
}

static void engine_handle_cmd(android_app* app, int32_t cmd) {
    Engine* engine = (Engine*)app->userData;
    switch (cmd) {
        case APP_CMD_START:
        	LOGI("APP_CMD_START");
        	break;
        case APP_CMD_RESUME:
        	LOGI("APP_CMD_RESUME");
        	break;
        case APP_CMD_SAVE_STATE:
        	LOGI("APP_CMD_SAVE_STATE");
        	break;
        case APP_CMD_PAUSE:
        	LOGI("APP_CMD_PAUSE");
        	break;
        case APP_CMD_STOP:
        	LOGI("APP_CMD_STOP");
        	break;
        case APP_CMD_DESTROY:
        	LOGI("APP_CMD_DESTROY");
        	break;
        case APP_CMD_GAINED_FOCUS:
        	LOGI("APP_CMD_GAINED_FOCUS");
        	break;
        case APP_CMD_LOST_FOCUS:
        	LOGI("APP_CMD_LOST_FOCUS");
        	break;
        case APP_CMD_INIT_WINDOW:
        	LOGI("APP_CMD_INIT_WINDOW");
            if (app->window != NULL) {
                if (ANativeWindow_setBuffersGeometry(app->window, WINDOW_WIDTH,
                    WINDOW_HEIGHT, WINDOW_FORMAT_RGBA_8888) < 0) {
                    LOGE("Cannot set pixel format!");
                    return;
                }
            }
        	break;
        case APP_CMD_WINDOW_RESIZED:
        	LOGI("APP_CMD_WINDOW_RESIZED");
        	break;
        case APP_CMD_WINDOW_REDRAW_NEEDED:
        	LOGI("APP_CMD_WINDOW_REDRAW_NEEDED");
        	break;
        case APP_CMD_TERM_WINDOW:
        	LOGI("APP_CMD_TERM_WINDOW");
        	break;
        case APP_CMD_INPUT_CHANGED:
        	LOGI("APP_CMD_INPUT_CHANGED");
        	break;
        case APP_CMD_CONTENT_RECT_CHANGED:
        	LOGI("APP_CMD_CONTENT_RECT_CHANGED");
        	break;
        case APP_CMD_CONFIG_CHANGED:
        	LOGI("APP_CMD_CONFIG_CHANGED");
        	break;
        case APP_CMD_LOW_MEMORY:
        	LOGI("APP_CMD_LOW_MEMORY");
        	break;
    }
}


void android_main(android_app* app) {

	// Init Parameters
	struct Engine engine;

	// It's magic func for NativeActivityGlue.
	app_dummy();

	// Set UserData
	memset(&engine, 0, sizeof(engine));
	app->userData = &engine;

	app->onAppCmd = engine_handle_cmd;
	engine.app = app;
    cv::Mat drawing_frame;

	char file_path[256] = {0};
	cv::Mat colorSampleImage;

	// Create Sample Image
	createAssetFile("images/colorsample_80.bmp");
	sprintf(file_path, "%s/%s/%s", DATA_PATH, PACKAGE_NAME, "assets/images/colorsample_80.bmp");
	colorSampleImage = cv::imread(file_path);

    cv::ocl::DevicesInfo devInfo;
    int res = cv::ocl::getOpenCLDevices(devInfo, cv::ocl::CVCL_DEVICE_TYPE_GPU);
//    int res = cv::ocl::getOpenCLDevices(devInfo, cv::ocl::CVCL_DEVICE_TYPE_ALL);
    if(res == 0) {
           LOGD("There is no OPENCL Here !");
           exit(0);
    }

	// Loop
	while(1) {
        // Read all pending events.
        int ident;
        int events;
        android_poll_source* source;

        // Process system events
        while ((ident=ALooper_pollAll(0, NULL, &events, (void**)&source)) >= 0) {
            // Process this event.
            if (source != NULL) {
                source->process(app, source);
            }

            // Create Mat Object for OpenCL
            cv::ocl::oclMat oclIn(colorSampleImage), oclOut;
            // Change Color RGB->Black&White on OpenCL
            cv::ocl::cvtColor(oclIn, oclOut, cv::COLOR_BGR2GRAY);
            // Download Mat for OpenCL to Mat for CPU
            oclOut.download(drawing_frame);

            // Show Image on Display
          	engine_draw_frame(&engine, drawing_frame);

        }
	}
}
