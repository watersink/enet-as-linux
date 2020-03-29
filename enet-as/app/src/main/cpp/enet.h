//
// Created by root on 6/3/19.
//

#ifndef COW_GAIT_ENET_H
#define COW_GAIT_ENET_H

#include <iostream>
using namespace std;


#include "net.h"
#include "enet_512288.id.h"







class ENET {

public:
    ENET(string param_path, string bin_path, string lut_path);
    ENET(ncnn::Mat param_path, ncnn::Mat bin_path);

    ncnn::Mat process(ncnn::Mat inputMat);

    ~ENET(){};


private:
    //const int resizeWidth = 640;
    //const int resizeHeight = 360;
    const int resizeWidth = 512;
    const int resizeHeight = 288;

    ncnn::Net enet;


};


#endif //COW_GAIT_ENET_H
