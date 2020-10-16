#define _USE_MATH_DEFINES
#include <stdio.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/core/hal/interface.h>
#include <math.h>
#pragma warning (disable:4996)
#define deg2rad(X) (X*M_PI/180)
#define rad2deg(X) (X*180/M_PI)

int filter_x[3][3] = { {-1,0,1},{-1,0,1},{-1,0,1} };
int filter_y[3][3] = { {-1,-1,-1},{0,0,0},{1,1,1} };
using namespace cv;
using namespace std;

void main()
{
	int height, width, conv_x, conv_y, conv_x_cmp1, conv_y_cmp1, conv_x_cmp2, conv_y_cmp2;
	int height1, width1, his_size;
	Mat imgColor = imread("assignment3.bmp", IMREAD_COLOR);
	Mat imgGray = imread("assignment3.bmp", IMREAD_GRAYSCALE);
	Mat compimg1Gray = imread("compare1.bmp", IMREAD_GRAYSCALE);
	Mat compimg2Gray = imread("compare2.bmp", IMREAD_GRAYSCALE);
	width = imgGray.cols;
	height = imgGray.rows;
	width1 = (width / 8) - 1;
	height1 = (height / 8) - 1;

	his_size = 9 * width1 * height1;
	float* histogram = new float[his_size];
	float* histogram_cmp1 = new float[his_size];
	float* histogram_cmp2 = new float[his_size];
	float mag, deg, mag_cmp1, deg_cmp1, mag_cmp2, deg_cmp2;
	int deg_index;
	for (int i = 0; i < his_size; i++)
	{
		histogram[i] = histogram_cmp1[i] = histogram_cmp2[i] = 0;
	}
	float max = FLT_MIN, min = FLT_MAX;

	int his_index = 0;
	float sum = 0, sum1 = 0, sum2 = 0;
	for (int y = 0; y < 8 * height1; y = y + 8)
	{
		for (int x = 0; x < 8 * width1; x = x + 8)
		{
			sum = sum1 = sum2 = 0;
			for (int b = y; b < y + 16; b++)
			{

				for (int a = x; a < x + 16; a++)
				{
					conv_x = conv_y = conv_x_cmp1 = conv_y_cmp1 = conv_x_cmp2 = conv_y_cmp2 = 0;

					for (int i = b - 1; i <= b + 1; i++)
					{
						for (int j = a - 1; j <= a + 1; j++)
						{
							if ((i >= 0 && i < height) && (j >= 0 && j < width))
							{
								conv_x += imgGray.at<uchar>(i, j) * filter_x[i - (b - 1)][j - (a - 1)];
								conv_y += imgGray.at<uchar>(i, j) * filter_y[i - (b - 1)][j - (a - 1)];
								conv_x_cmp1 += compimg1Gray.at<uchar>(i, j) * filter_x[i - (b - 1)][j - (a - 1)];
								conv_y_cmp1 += compimg1Gray.at<uchar>(i, j) * filter_y[i - (b - 1)][j - (a - 1)];
								conv_x_cmp2 += compimg2Gray.at<uchar>(i, j) * filter_x[i - (b - 1)][j - (a - 1)];
								conv_y_cmp2 += compimg2Gray.at<uchar>(i, j) * filter_y[i - (b - 1)][j - (a - 1)];
							}
						}
					}
					mag = sqrtf(powf(conv_x, 2) + powf(conv_y, 2));
					deg = rad2deg((atan2(conv_y, conv_x))) + 90 + 180;
					mag_cmp1 = sqrtf(powf(conv_x_cmp1, 2) + powf(conv_y_cmp1, 2));
					deg_cmp1 = rad2deg((atan2(conv_y_cmp1, conv_x_cmp1))) + 90 + 180;
					mag_cmp2 = sqrtf(powf(conv_x_cmp2, 2) + powf(conv_y_cmp2, 2));
					deg_cmp2 = rad2deg((atan2(conv_y_cmp2, conv_x_cmp2))) + 90 + 180;
					if (deg > 180)
						deg -= 180;
					if (deg_cmp1 > 180)
						deg_cmp1 -= 180;
					if (deg_cmp2 > 180)
						deg_cmp2 -= 180;

					deg_index = (deg / 20);
					if (deg_index >= 9)
						deg_index = 8;
					histogram[deg_index + 9 * his_index] += mag;
					sum += powf(mag, 2);
					deg_index = (deg_cmp1 / 20);
					if (deg_index >= 9)
						deg_index = 8;
					histogram_cmp1[deg_index + 9 * his_index] += mag_cmp1;
					sum1 += powf(mag_cmp1, 2);
					deg_index = (deg_cmp2 / 20);
					if (deg_index >= 9)
						deg_index = 8;
					histogram_cmp2[deg_index + 9 * his_index] += mag_cmp2;
					sum2 += powf(mag_cmp2, 2);

				}
			}
			for (int s = 0; s < 9; s++) //L2 normalization
			{
				histogram[s + 9 * his_index] /= sqrtf(sum);
				histogram_cmp1[s + 9 * his_index] /= sqrtf(sum1);
				histogram_cmp2[s + 9 * his_index] /= sqrtf(sum2);
			}

			his_index++;
		}
	}

	float diff1 = 0, diff2 = 0;
	for (int i = 0; i < his_size; i++) //comparing
	{
		diff1 += fabsf(histogram[i] - histogram_cmp1[i]);
		diff2 += fabsf(histogram[i] - histogram_cmp2[i]);
	}
	imshow("원본", imgGray);
	imshow("비교1", compimg1Gray);
	imshow("비교2", compimg2Gray);
	cout << "원본-비교1 차이값:" << diff1 << endl;
	cout << "원본-비교2 차이값:" << diff2 << endl;
	cout << "원본과 비슷한 사진은 " << ((diff1 < diff2) ? "비교1" : "비교2") << " 사진입니다." << endl;
	waitKey(50000);

	delete[] histogram;
	delete[] histogram_cmp1;
	delete[] histogram_cmp2;
}