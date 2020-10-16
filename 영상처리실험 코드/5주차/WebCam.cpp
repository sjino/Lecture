#include <stdio.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#pragma warning (disable:4996)
int thresh = 200; 
int max_thresh = 255;

using namespace cv;
using namespace std;
void main()
{
	VideoCapture capture(0);
	Mat frame;
	Mat gray;
	Mat cornerMap, cornerMap_norm, cornerMap_scaled;
	int blockSize = 2;
	int apertureSize = 3;
	double k = 0.04;

	if (!capture.isOpened()) {
		printf("Couldn¡¯t open the web camera¡¦\n");
		return;
	}
	while (true) {
		capture >> frame;
		capture >> cornerMap;
		cvtColor(frame, gray, COLOR_RGB2GRAY);
		cornerHarris(gray, cornerMap, blockSize, apertureSize, k);
		normalize(cornerMap, cornerMap_norm, 0, 255, NORM_MINMAX, CV_8UC1, Mat());
		convertScaleAbs(cornerMap_norm, cornerMap_scaled);
		for (int j = 0; j < cornerMap_norm.rows; j++)
		{
			for (int i = 0; i < cornerMap_norm.cols; i++)
			{
				if ((int)cornerMap_norm.at<uchar>(j, i) > thresh)
				{
					circle(cornerMap_scaled, Point(i, j), 5, Scalar(0), 2, 8, 0);
				}
			}
		}

		imshow("Video", cornerMap_scaled);
		if (waitKey(30) >= 0) break;
	}
}