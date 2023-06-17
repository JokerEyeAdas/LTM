/**
 * 
 * Function：LTM实现
 * Author:joker.mao
 * 
*/

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <iostream>
#include "ltm.hpp"


using namespace cv;
using namespace std;

#define H_NUMS 5
#define V_NUMS 4

#define IN_MAX  256
#define OUT_MAX 256

#define DEBUG 1

int main(int argc, char** argv)
{
    cv::Mat src = cv::imread("../../img/1.jpg", 0);
    
    cv::Mat out = src.clone();
    cv::equalizeHist(src, out);
    cv::imwrite("EQ_HIST.png", out);
    cv::imwrite("ORI.png", src);

    Ltm<uint8_t, uint8_t> ltm(src.data, out.data, src.cols, src.rows, H_NUMS, V_NUMS, IN_MAX, OUT_MAX);
#if DEBUG == 1
    
    /*for (int j = 0; j < V_NUMS; ++j) {
        for (int i = 0; i < H_NUMS; ++i) {
            cv::Mat im(cv::Size(src.cols / H_NUMS, src.rows / V_NUMS), CV_8UC1, ltm.GetDivImgPtr(i, j));
            imshow("[" + std::to_string(i) + std::to_string(j) + "]", im);
        }
    }*/
    
    ltm.Run();

   /* for (int j = 0; j < V_NUMS; ++j) {
        for (int i = 0; i < H_NUMS; ++i) {
            cv::Mat im(cv::Size(src.cols / H_NUMS, src.rows / V_NUMS), CV_8UC1, ltm.GetDivImgPtr(i, j));
            imshow("l[" + std::to_string(i) + std::to_string(j) + "]", im);
        }
    }*/

    cv::Mat banlance_lut(cv::Size(IN_MAX * H_NUMS, OUT_MAX * V_NUMS), CV_8UC3, cv::Scalar(255, 255, 255));
    for (int j = 0; j < V_NUMS; ++j) {
        for (int i = 0; i < H_NUMS; ++i) {
            auto scalar = cv::Scalar(255, 255, 255);
            if (((i % 2) && (!(j % 2))) || ((j % 2) && (!(i % 2))) )
                scalar = cv::Scalar(0, 0, 0);
            cv::Mat im(cv::Size(IN_MAX, OUT_MAX), CV_8UC3, scalar);
            auto ptr = ltm.GetLutMapPtr(i, j);
            for (int id = 0; id < IN_MAX - 1; ++id) {
                cv::line(im, cv::Point(id,  OUT_MAX - ptr[id]),  cv::Point(id + 1, OUT_MAX - ptr[id + 1]), cv::Scalar(0, 0, 255));
            }
            //imshow("lut[" + std::to_string(i) + std::to_string(j) + "]", im);
            cv::Mat roi = banlance_lut(cv::Rect(i * IN_MAX, j * OUT_MAX, IN_MAX, OUT_MAX));
            im.copyTo(roi);
        }
    }
    imshow("lut", banlance_lut);
    cv::imwrite("LUT.png", banlance_lut);
#else
    ltm.Run();
#endif
    cv::imwrite("LTM.png", out);
    cv::waitKey(0);
    return 0;
}