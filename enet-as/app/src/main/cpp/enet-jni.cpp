
#include <android/bitmap.h>
#include <android/log.h>
#include <jni.h>
#include <string>
#include <vector>


#include "enet_512288.id.h"   //这里看成自己的id.h
#include <sys/time.h>
//#include <unistd.h>
#include "net.h"
#include "enet.h"


#define TAG "enetso"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)



static ENET *ncnn_net;



extern "C"
JNIEXPORT jboolean JNICALL
Java_com_example_enet_ENET_Init(JNIEnv *env, jobject thiz, jbyteArray param, jbyteArray bin) {

    // TODO: implement Init()
    ncnn::Mat ncnn_param;
    ncnn::Mat ncnn_bin;


    // init param
    {
        int len = env->GetArrayLength(param);
        ncnn_param.create(len, (size_t) 1u);
        env->GetByteArrayRegion(param, 0, len, (jbyte *) ncnn_param);
    }

    // init bin
    {
        int len = env->GetArrayLength(bin);
        ncnn_bin.create(len, (size_t) 1u);
        env->GetByteArrayRegion(bin, 0, len, (jbyte *) ncnn_bin);
    }

    ncnn_net = new ENET(ncnn_param,ncnn_bin);
    return JNI_TRUE;


}



extern "C"
JNIEXPORT jfloatArray JNICALL
Java_com_example_enet_ENET_Process(JNIEnv *env, jobject thiz, jobject bitmap) {
    // TODO: implement Process()

    // ncnn from bitmap
    ncnn::Mat in;
    {
        AndroidBitmapInfo info;
        AndroidBitmap_getInfo(env, bitmap, &info);

        int width = info.width;
        int height = info.height;

        if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888)
            return NULL;


        void* indata;
        AndroidBitmap_lockPixels(env, bitmap, &indata);


        // 把像素转换成data，并指定通道顺序
        // 因为图像预处理每个网络层输入的数据格式不一样一般为300*300 128*128等等所以这类需要一个resize的操作可以在cpp中写，也可以是java读入图片时有个resize操作
        //in = ncnn::Mat::from_pixels_resize((const unsigned char*)indata, ncnn::Mat::PIXEL_RGBA2RGB, width, height,300,300);
        in = ncnn::Mat::from_pixels(static_cast<const unsigned char *>(indata), ncnn::Mat::PIXEL_RGBA2BGR, width, height);

        // 下面一行为debug代码
        __android_log_print(ANDROID_LOG_DEBUG, "ENetJniIn", "enet_process_has_input1, in.w: %d; in.h: %d in.c:%d ", in.w, in.h,in.c);
        //AndroidBitmap_unlockPixels(env, bitmap);
    }




    {

        ncnn::Mat out = ncnn_net->process(in);

        __android_log_print(ANDROID_LOG_DEBUG, "ENetJniIn", "enet_process_has_output, in.w: %d; in.h: %d in.c:%d ", out.w, out.h,out.c);
        int output_wsize = out.w;
        int output_hsize = out.h;
        //输出整理
        float *output[output_wsize * output_hsize];   // float类型
        for(int i = 0; i< out.h; i++) {
            for (int j = 0; j < out.w; j++) {
                output[i*output_wsize + j] = &out.row(
                        i)[j];
            }
        }
        //建立float数组 长度为 output_wsize * output_hsize,如果只是ouput_size相当于只有一行的out的数据那就是一个object检测数据
        jfloatArray jOutputData = env->NewFloatArray(output_wsize * output_hsize);
        if (jOutputData == nullptr) return nullptr;
        env->SetFloatArrayRegion(jOutputData, 0,  output_wsize * output_hsize,
                               reinterpret_cast<const jfloat *>(*output));
        return jOutputData;
    }


}


