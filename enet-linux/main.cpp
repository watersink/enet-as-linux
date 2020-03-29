#include <stdio.h>
#include <vector>
#include <string>

#include "enet.h"

void test(){

    ENET enet("../assets/enet_512288.param.bin","../assets/enet_512288.bin","../assets/cityscapes19.png");
    string imagepath="../img00113.png";
    cv::Mat bgr = cv::imread(imagepath, 1);

	std::vector<cv::Mat> out_p= enet.process(bgr);
	cv::Mat prediction  =out_p[0];
	cv::Mat prediction_rgb  =out_p[1];

	cv::imwrite("a.png",prediction_rgb);

}



int main()
{
    test();
    return 0;
}
