#define _USE_MATH_DEFINES
#include <stdio.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include<opencv2/core/core.hpp>
#include<opencv2/opencv.hpp>
#include<opencv2/imgcodecs/imgcodecs.hpp>
#include<opencv2/core/hal/interface.h>
#include<math.h>
#pragma warning (disable:4996)
#define deg2rad(X) (X*M_PI/180)
using namespace cv;

void main()
{
	int height, width, cy, cx;
	int i_reverse, j_reverse;
	int degree;
	Mat imgColor = imread("æ∆¿Ã¿Ø.jpg", IMREAD_COLOR);
	printf("Input rotation degree:");
	scanf_s("%d", &degree);
	width = imgColor.cols;
	height = imgColor.rows;
	cy = height / 2;
	cx = width / 2;
	Mat result(height, width, CV_8UC3);
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			j_reverse = cosf(deg2rad(degree)) * (j - cx) + sinf(deg2rad(degree)) * (i - cy) + cx;
			i_reverse = -sinf(deg2rad(degree)) * (j - cx) + cosf(deg2rad(degree)) * (i - cy) + cy;
			if ((j_reverse >= 0 && j_reverse < width) && (i_reverse >= 0 && i_reverse < height))
			{
				result.at<Vec3b>(i, j)[0] = imgColor.at<Vec3b>(i_reverse, j_reverse)[0];
				result.at<Vec3b>(i, j)[1] = imgColor.at<Vec3b>(i_reverse, j_reverse)[1];
				result.at<Vec3b>(i, j)[2] = imgColor.at<Vec3b>(i_reverse, j_reverse)[2];
			}
			else
			{
				result.at<Vec3b>(i, j)[0] = 0;
				result.at<Vec3b>(i, j)[1] = 0;
				result.at<Vec3b>(i, j)[2] = 0;
			}
		}
	}

	imshow("color", imgColor);
	//imwrite("result.jpg", result);
	imshow("result", result);
	waitKey(50000);
}