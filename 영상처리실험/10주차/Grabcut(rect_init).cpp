#include <stdio.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#pragma warning (disable:4996)

using namespace cv;
using namespace std;
void main()
{
	Mat image = imread("³ëÁö¼±8.jpg");
	Rect rectangle(375, 50, 600, 870);
	Mat result;
	Mat bg, fg;
	grabCut(image, result, rectangle, bg, fg, 5, GC_INIT_WITH_RECT);
	compare(result, GC_PR_FGD, result, CMP_EQ);
	Mat foreground(image.size(), CV_8UC3, Scalar(255, 255, 255));
	image.copyTo(foreground, result);
	imshow("image", image);
	imshow("result", result);
	imshow("foreground", foreground);
	waitKey(0);
}