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

void main()
{
	int height, width, conv_x, conv_y;
	Mat imgColor = imread("아이유.jpg", IMREAD_COLOR);
	Mat imgGray = imread("아이유.jpg", IMREAD_GRAYSCALE);
	width = imgGray.cols;
	height = imgGray.rows;
	float** conv_mat_mag = new float* [height];
	float** conv_mat_deg = new float* [height];
	float histogram[9];
	float max = FLT_MIN, min = FLT_MAX;
	for (int i = 0; i < height; i++)
	{
		conv_mat_mag[i] = new float[width];
		conv_mat_deg[i] = new float[width];
	}

	Mat imgEdge(height, width, CV_8UC1);

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			conv_x = 0;
			conv_y = 0;
			for (int i = y - 1; i <= y + 1; i++)
			{
				for (int j = x - 1; j <= x + 1; j++)
				{
					if ((i >= 0 && i < height) && (j >= 0 && j < width))
					{
						conv_x += imgGray.at<uchar>(i, j) * filter_x[i - (y - 1)][j - (x - 1)];
						conv_y += imgGray.at<uchar>(i, j) * filter_y[i - (y - 1)][j - (x - 1)];
					}

				}
			}
			conv_mat_mag[y][x] = sqrtf(powf(conv_x, 2) + powf(conv_y, 2));
			conv_mat_deg[y][x] = rad2deg((atan2(conv_y, conv_x))) + 90;
			if (conv_mat_deg[y][x]>180)
			{
				conv_mat_deg[y][x] -= 180;
			}
			else if (conv_mat_deg[y][x]<0)
			{
				conv_mat_deg[y][x] += 180;
			}
			max = (((conv_mat_mag[y][x]) > (max)) ? (conv_mat_mag[y][x]) : (max));
			min = (((conv_mat_mag[y][x]) < (min)) ? (conv_mat_mag[y][x]) : (min));
		}
	}
	for (int y = 0; y < height; y++) //normalization
	{
		for (int x = 0; x < width; x++)
		{
			conv_mat_mag[y][x] = (((conv_mat_mag[y][x] - min) * 255) / (max - min));
		}
	}
	int deg;
	for (int i = 0; i < 9; i++)
	{
		histogram[i] = 0;
	}
	for (int y = 0; y < height; y++) //normalization
	{
		for (int x = 0; x < width; x++)
		{
			deg = (conv_mat_deg[y][x] / 20);
			if (deg>=9)
			{
				deg = 8;
			}
			histogram[deg] += conv_mat_mag[y][x];
		}
	}
	imshow("color", imgColor);
	//imwrite("result.jpg", result);
	//imshow("result", imgEdge);'
	for (int i = 0; i < 9; i++)
	{
		std::cout << histogram[i] << std::endl;
	}
	waitKey(5000);
	for (int i = 0; i < height; i++)
	{
		delete[] conv_mat_mag[i];
		delete[] conv_mat_deg[i];
	}
	delete[] conv_mat_mag;
	delete[] conv_mat_deg;
}