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

#define DEBUG 0

int main(int argc, char** argv)
{
    cv::Mat src = cv::imread("./2.jpg", 0);
    
    cv::Mat out = src.clone();
    cv::equalizeHist(src, out);
    cv::imshow("EQ_HITS", out);
    cv::imshow("ORI", src);

    Ltm<uint8_t, uint8_t> ltm(src.data, out.data, src.cols, src.rows, H_NUMS, V_NUMS, 256, 256);
#if DEBUG == 1
    
    for (int j = 0; j < V_NUMS; ++j) {
        for (int i = 0; i < H_NUMS; ++i) {
            cv::Mat im(cv::Size(src.cols / H_NUMS, src.rows / V_NUMS), CV_8UC1, ltm.GetDivImgPtr(i, j));
            imshow("[" + std::to_string(i) + std::to_string(j) + "]", im);
        }
    }
    
    ltm.Run();

    for (int j = 0; j < V_NUMS; ++j) {
        for (int i = 0; i < H_NUMS; ++i) {
            cv::Mat im(cv::Size(src.cols / H_NUMS, src.rows / V_NUMS), CV_8UC1, ltm.GetDivImgPtr(i, j));
            imshow("l[" + std::to_string(i) + std::to_string(j) + "]", im);
        }
    }
#else
    ltm.Run();
#endif
    cv::imshow("LTM", out);
    cv::waitKey(0);
    return 0;
}