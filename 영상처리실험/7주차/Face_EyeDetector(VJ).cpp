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
	CascadeClassifier cascade_face, cascade_eye;
	cascade_face.load("C:/opencv/sources/data/lbpcascades/lbpcascade_frontalface.xml");
	Mat img = imread("아이유.jpg", 1); // image read (color)
	vector<Rect> faces, eyes;
	cascade_face.detectMultiScale(img, faces, 1.1, 4, 0 | CASCADE_SCALE_IMAGE, Size(10, 10));
	//얼굴 인식 완료
	cascade_eye.load("C:/opencv/sources/data/haarcascades/haarcascade_eye.xml");
	cascade_eye.detectMultiScale(img, eyes, 1.1, 4, 0 | CASCADE_SCALE_IMAGE, Size(10, 10));
	//눈 인식 완료
	for (int y = 0; y < faces.size(); y++)
	{
		Point lb(faces[y].x + faces[y].width, faces[y].y + faces[y].height);
		Point tr(faces[y].x, faces[y].y);
		rectangle(img, lb, tr, Scalar(0, 255, 0), 3, 8, 0);
	}
	for (int y = 0; y < eyes.size(); y++)
	{
		Point lb(eyes[y].x + eyes[y].width, eyes[y].y + eyes[y].height);
		Point tr(eyes[y].x, eyes[y].y);
		rectangle(img, lb, tr, Scalar(0, 0, 255), 3, 8, 0);
	}
	imshow("Face", img);
	waitKey(50000);
}