#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/ml/ml.hpp>
using namespace cv;
using namespace cv::ml;
using namespace std;

int main()
{
	VideoCapture capture(0);
	if (!capture.isOpened()) {
		printf("Couldn't open the web camera!\n");
		return 0;
	}
	Mat frame;
	CascadeClassifier cascade, cascade1;
	vector<Rect> faces, eyes;
	cascade.load("C:/opencv/sources/data/lbpcascades/lbpcascade_frontalface.xml");//학습모델 경로
	cascade1.load("C:/opencv/sources/data/haarcascades/haarcascade_eye.xml"); //학습모델 경로

	while (true)
	{
		capture >> frame;
		cascade.detectMultiScale(frame, faces, 1.1, 4, 0 | CASCADE_SCALE_IMAGE, Size(10, 10));
		cascade1.detectMultiScale(frame, eyes, 1.1, 4, 0 | CASCADE_SCALE_IMAGE, Size(10, 10));
		for (int y = 0; y < faces.size(); y++)
		{
			Point lb(faces[y].x + faces[y].width, faces[y].y + faces[y].height);
			Point tr(faces[y].x, faces[y].y);
			rectangle(frame, lb, tr, Scalar(0, 255, 0), 3, 8, 0);
		}
		for (int y = 0; y < eyes.size(); y++)
		{
			Point lb(eyes[y].x + eyes[y].width, eyes[y].y + eyes[y].height);
			Point tr(eyes[y].x, eyes[y].y);
			rectangle(frame, lb, tr, Scalar(0, 0, 255), 3, 8, 0);
		}
		imshow("Face_Eye Detector", frame);
		faces.clear();
		eyes.clear();
		if (waitKey(30) >= 0) break;
	}
}