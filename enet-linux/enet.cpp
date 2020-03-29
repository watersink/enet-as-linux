#include "enet.h"


ENET::ENET(string param_path, string enet_path,  string lut_path)
{
    int ret_p = enet.load_param_bin(param_path.c_str());
    int ret_b = enet.load_model(enet_path.c_str());

	std::cout<<"###  "<<ret_p<<" "<<ret_b<<std::endl;

    label_colours = cv::imread(lut_path, 1);
}



vector<cv::Mat> ENET::process(const cv::Mat &img)
{
    ncnn::Mat inputMat;

    inputMat = ncnn::Mat::from_pixels_resize(img.data, ncnn::Mat::PIXEL_BGR, img.cols, img.rows, resizeWidth, resizeHeight);

//    // 输入为空，返回失败
//    if(inputMat.empty())    return false;

    ncnn::Extractor extractor = enet.create_extractor();
    extractor.set_num_threads(2);
    extractor.input(enet_512288_param_id::BLOB_data, inputMat);

    ncnn::Mat outputMat;
    extractor.extract(enet_512288_param_id::BLOB_deconv6_0_01, outputMat);

    cv::Mat prediction = cv::Mat::zeros(cv::Size(outputMat.w, outputMat.h), CV_8UC1);

    ncnn::Mat chn_0 = outputMat.channel(0);
    ncnn::Mat chn_1 = outputMat.channel(1);
    ncnn::Mat chn_2 = outputMat.channel(2);
    ncnn::Mat chn_3 = outputMat.channel(3);



    for(int i = 0; i < outputMat.h; ++i)
    {
        const float* pCh0 = chn_0.row(i);
        const float* pCh1 = chn_1.row(i);
        const float* pCh2 = chn_2.row(i);
        const float* pCh3 = chn_3.row(i);

        uchar *pCowMask = prediction.ptr<uchar>(i);

        for(int j = 0; j < outputMat.w; ++j){
			int maxindex=0;
			float maxvalue=0;
			if (pCh0[j] > maxvalue){
				maxindex=0;
				maxvalue=pCh0[j];
			}
			if (pCh1[j] > maxvalue){
				maxindex=1;
				maxvalue=pCh1[j];
			}

			if (pCh2[j] > maxvalue){
				maxindex=2;
				maxvalue=pCh2[j];
			}

			if (pCh3[j] > maxvalue){
				maxindex=3;
				maxvalue=pCh3[j];
			}




            pCowMask[j] = maxindex;
        }
    }


	std::vector<Mat> channels(3,prediction);
	cv::Mat mergeImg;
	cv::merge(channels, mergeImg);
	cv::Mat prediction_rgb;
	cv::LUT(mergeImg, label_colours, prediction_rgb);

	std::vector<cv::Mat> out_p;
	out_p.push_back(prediction);
	out_p.push_back(prediction_rgb);
    return out_p;
}
