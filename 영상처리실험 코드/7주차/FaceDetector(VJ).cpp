#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/ml/ml.hpp>
using namespace cv;
using namespace cv::ml;
using namespace std;

int main()
{
	CascadeClassifier cascade;
	cascade.load("C:/opencv/sources/data/lbpcascades/lbpcascade_frontalface.xml");
	Mat img = imread("³ëÁö¼±8.jpg", 1); // image read (color)
	vector<Rect> faces;
	cascade.detectMultiScale(img, faces, 1.1, 4, 0 | CASCADE_SCALE_IMAGE, Size(10, 10));
	for (int y = 0; y < faces.size(); y++)
	{
		Point lb(faces[y].x + faces[y].width, faces[y].y + faces[y].height);
		Point tr(faces[y].x, faces[y].y);
		rectangle(img, lb, tr, Scalar(0, 255, 0), 3, 8, 0);
	}
	imshow("Face", img);
	waitKey(50000);
}