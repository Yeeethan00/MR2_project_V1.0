/*************************************************************************
	> File Name: KinectV2_func.h
	> Author: yi rongjie
	> Mail: yirj0809@gmail.com
	> Created Time: 2018年10月25日 星期四 15时59分08秒
 ************************************************************************/

#ifndef _KINECTV2_FUNC_H
#define _KINECTV2_FUNC_H
#include <libfreenect2/libfreenect2.hpp> 
#include <libfreenect2/frame_listener_impl.h>
#include <libfreenect2/registration.h>
#include <libfreenect2/packet_pipeline.h> 
#include <libfreenect2/logger.h> 
#include <opencv2/opencv.hpp> 
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp> 
#include <opencv2/objdetect/objdetect.hpp> 
#include <iostream>
#include <cstdlib>
using namespace std; 
using namespace cv; 
enum Processor { cl, gl, cpu };

int kinectv2_get_picture_demo();
#endif
