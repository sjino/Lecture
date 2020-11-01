#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include<opencv2/core/core.hpp>
#include<opencv2/opencv.hpp>
#include<opencv2/imgcodecs/imgcodecs.hpp>
#include<opencv2/core/hal/interface.h>
#pragma warning (disable:4996)

using namespace cv;
int resizing_pixel(int p1, int p2, int p3, int p4, float gap_x, float gap_y)
{
	float p12, p13, p24, p34;
	int pixel;
	p12 = (1 - gap_x) * p1 + gap_x * p2;
	p13 = (1 - gap_y) * p1 + gap_y * p3;
	p34 = (1 - gap_x) * p3 + gap_x * p4;
	p24 = (1 - gap_y) * p2 + gap_y * p4;

	pixel = (((1 - gap_x) * p13 + gap_x * p24) + ((1 - gap_y) * p12 + gap_y * p34)) / 2;
	return pixel;
}
void main()
{
	// You can set the path of an image file
	float sfac;
	int height, width;
	int r, g, b;
	int height_resized, width_resized;
	int i_reverse, j_reverse;
	Mat imgColor = imread("æ∆¿Ã¿Ø.jpg", IMREAD_COLOR);
	printf("Input scale factor:");
	scanf_s("%f", &sfac);
	width = imgColor.cols;
	height = imgColor.rows;
	width_resized = (int)(width * sfac);
	height_resized = (int)(height * sfac);
	Mat result(height_resized, width_resized, CV_8UC3);
	//printf("%d %d", width_resized, height_resized);
	float gapi, gapj;
	for (int i = 0; i < height_resized; i++)
	{
		for (int j = 0; j < width_resized; j++)
		{
			i_reverse = i * (1 / sfac);
			j_reverse = j * (1 / sfac);
			gapi = (float)i * (1 / sfac) - i_reverse;
			gapj = (float)j * (1 / sfac) - j_reverse;
			result.at<Vec3b>(i, j)[0] = resizing_pixel(imgColor.at<Vec3b>(i_reverse, j_reverse)[0], imgColor.at<Vec3b>(i_reverse, j_reverse + 1)[0], imgColor.at<Vec3b>(i_reverse + 1, j_reverse)[0], imgColor.at<Vec3b>(i_reverse + 1, j_reverse + 1)[0], gapj, gapi);
			result.at<Vec3b>(i, j)[1] = resizing_pixel(imgColor.at<Vec3b>(i_reverse, j_reverse)[1], imgColor.at<Vec3b>(i_reverse, j_reverse + 1)[1], imgColor.at<Vec3b>(i_reverse + 1, j_reverse)[1], imgColor.at<Vec3b>(i_reverse + 1, j_reverse + 1)[1], gapj, gapi);
			result.at<Vec3b>(i, j)[2] = resizing_pixel(imgColor.at<Vec3b>(i_reverse, j_reverse)[2], imgColor.at<Vec3b>(i_reverse, j_reverse + 1)[2], imgColor.at<Vec3b>(i_reverse + 1, j_reverse)[2], imgColor.at<Vec3b>(i_reverse + 1, j_reverse + 1)[2], gapj, gapi);
		}
	}

	imshow("color", imgColor);
	//imwrite("result.jpg", result);
	imshow("result", result);
	waitKey(5000);
}