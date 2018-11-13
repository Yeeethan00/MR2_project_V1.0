/*************************************************************************
	> File Name: KinectV2_func.cpp
	> Author: yi rongjie
	> Mail: yirj0809@gmail.com
	> Created Time: 2018年10月25日 星期四 15时59分00秒
 ************************************************************************/

#include"KinectV2_func.h"
int kinectv2_get_picture_demo(){
    //! [context] 
    libfreenect2::Freenect2 freenect2; 
    libfreenect2::Freenect2Device *dev = NULL; 
    libfreenect2::PacketPipeline *pipeline = NULL; 
    //! [context] 
    //! [discovery] 
    if(freenect2.enumerateDevices() == 0)
    { 
        std::cout << "no device connected!" << std::endl; 
        return -1; 
    } 
    string serial = freenect2.getDefaultDeviceSerialNumber(); 
    if(serial == "") 
        return -1; 
    cout<<"The serial number is :"<<serial<<endl; 
    //! [discovery] 
    int depthProcessor = cl; 
    if(depthProcessor == cpu) 
    { 
        if(!pipeline) 
        { 
            //! [pipeline] 
            pipeline = new libfreenect2::CpuPacketPipeline(); 
            //! [pipeline] 
        }
    } 
    else if (depthProcessor == gl) 
    { 
    #ifdef LIBFREENECT2_WITH_OPENGL_SUPPORT 
        if(!pipeline) 
            pipeline = new libfreenect2::OpenGLPacketPipeline(); 
    #else 
        std::cout << "OpenGL pipeline is not supported!" << std::endl; 
    #endif 
    } 
    else if (depthProcessor == cl) 
    { 
    #ifdef LIBFREENECT2_WITH_OPENCL_SUPPORT 
        if(!pipeline) 
            pipeline = new libfreenect2::OpenCLPacketPipeline(); 
    #else 
        std::cout << "OpenCL pipeline is not supported!" << std::endl; 
    #endif 
    } 
    if(pipeline) 
    { 
        //! [open] 
        dev = freenect2.openDevice(serial, pipeline); 
        //! [open] 
    } 
    else 
    { 
        dev = freenect2.openDevice(serial); 
    } 
    if(dev == 0) 
    {
        std::cout << "failure opening device!" << std::endl; 
        return -1; 
    }
    //! [listeners] 
    libfreenect2::SyncMultiFrameListener listener(libfreenect2::Frame::Color |libfreenect2::Frame::Depth |libfreenect2::Frame::Ir);
    libfreenect2::FrameMap frames; 
    dev->setColorFrameListener(&listener); 
    dev->setIrAndDepthFrameListener(&listener); 
    //! [listeners] 
    //! [start] 
    dev->start(); 
    std::cout << "device serial: " << dev->getSerialNumber() << std::endl; 
    std::cout << "device firmware: " << dev->getFirmwareVersion() << std::endl; 
    //! [start] 
    libfreenect2::Registration* registration = new libfreenect2::Registration(dev->getIrCameraParams(), dev->getColorCameraParams());
    libfreenect2::Frame undistorted(512, 424, 4), registered(512, 424, 4), depth2rgb(1920, 1080 + 2, 4); 
    Mat rgbmat, depthmat, irmat, depthmatUndistorted, rgbd, rgbd2; 
    while(!(waitKey(1)==27)) 
    { 
        listener.waitForNewFrame(frames); 
        libfreenect2::Frame *rgb = frames[libfreenect2::Frame::Color]; 
        libfreenect2::Frame *ir = frames[libfreenect2::Frame::Ir]; 
        libfreenect2::Frame *depth = frames[libfreenect2::Frame::Depth]; 
        //! [loop start] 
        Mat rgbmat = cv::Mat(rgb->height, rgb->width, CV_8UC4, rgb->data); 
        Mat irmat = cv::Mat(ir->height, ir->width, CV_32FC1, ir->data); 
        Mat depthmat =cv::Mat(depth->height, depth->width, CV_32FC1, depth->data); 
        cv::imshow("rgb", rgbmat); cv::imshow("ir", irmat / 4500.0f); cv::imshow("depth", depthmat / 4500.0f); 
        // //! [registration] 
        // registration->apply(rgb, depth, &undistorted, &registered, true, &depth2rgb); 
        // //! [registration] 
        // cv::Mat(undistorted.height, undistorted.width, CV_32FC1, undistorted.data).copytTo(depthmatUndistorted); 
        // cv::Mat(registered.height, registered.width, CV_8UC4, registered.data).copyTo(rgbd); 
        // cv::Mat(depth2rgb.height, depth2rgb.width, CV_32FC1, depth2rgb.data).copyTo(rgbd2); 
        // cv::imshow("undistorted", depthmatUndistorted / 4500.0f); 
        // cv::imshow("registered", rgbd); 
        // cv::imshow("depth2RGB", rgbd2 / 4500.0f); 
        //! [loop end] 
        listener.release(frames); 
    } 
    //! [loop end] 
    //! [stop] 
    dev->stop(); 
    dev->close(); 
    //! [stop] delete registration; 
    std::cout << "Goodbye World!" << std::endl;
    return 0;
}
