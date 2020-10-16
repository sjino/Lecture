#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/video/tracking.hpp>
#include <vector>
#include <stdio.h>
#include <Windows.h>
#define Width 480
#define Height 360
#define gridsize 5
using namespace cv;
using namespace std;

int main()
{
	VideoCapture capture(0);
	int key;
	Mat flow, frame, prevFrame, img;

	while (1)
	{
		capture >> frame;
		resize(frame, img, Size(Width, Height));
		cvtColor(img, frame, COLOR_BGR2GRAY);
		if (prevFrame.empty() == false)
		{
			calcOpticalFlowFarneback(prevFrame, frame, flow, 0.4, 1, 12, 2, 8, 1.2, 0);
			for (int y = 0; y < Height; y += gridsize)
			{
				for (int x = 0; x < Width; x += gridsize)
				{
					const Point2f flowatxy = flow.at<Point2f>(y, x) * 2;
					line(img, Point(x, y), Point(cvRound(x + flowatxy.x), cvRound(y + flowatxy.y)), Scalar(0, 255, 0));
					circle(img, Point(x, y), 1, Scalar(0, 0, 0), -1);
				}
			}
			frame.copyTo(prevFrame);
		}
		else frame.copyTo(prevFrame);
		imshow("Optical Flow", img);
		key = waitKey(20);
	}
}