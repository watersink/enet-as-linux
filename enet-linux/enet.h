//
// Created by root on 6/3/19.
//

#ifndef COW_GAIT_ENET_H
#define COW_GAIT_ENET_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <iostream>
using namespace std;
using namespace cv;


#include "net.h"
#include "enet_512288.id.h"







class ENET {

public:
    ENET(string param_path, string bin_path, string lut_path);

	std::vector<cv::Mat> process(const cv::Mat &img);

    ~ENET(){};


private:
    //const int resizeWidth = 640;
    //const int resizeHeight = 360;
    const int resizeWidth = 512;
    const int resizeHeight = 288;
	cv::Mat label_colours;

    ncnn::Net enet;


};


#endif //COW_GAIT_ENET_H
