#include <stdio.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/core/hal/interface.h>
#include <math.h>
#pragma warning (disable:4996)
int filter_x[3][3] = { {-1,0,1},{-1,0,1},{-1,0,1} };
int filter_y[3][3] = { {-1,-1,-1},{0,0,0},{1,1,1} };
using namespace cv;

void main()
{
	int height, width, conv_x, conv_y;
	Mat imgColor = imread("아이유.jpg", IMREAD_COLOR);
	Mat imgGray=imread("아이유.jpg",IMREAD_GRAYSCALE);
	width = imgGray.cols;
	height = imgGray.rows;
	float **conv_mat=new float*[height];
	float max = FLT_MIN, min = FLT_MAX;
	for (int i = 0; i < height; i++)
	{
		conv_mat[i] = new float[width];
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
			conv_mat[y][x]= sqrtf(powf(conv_x, 2) + powf(conv_y, 2));
			max = (((conv_mat[y][x]) > (max)) ? (conv_mat[y][x]) : (max));
			min = (((conv_mat[y][x]) < (min)) ? (conv_mat[y][x]) : (min));
		}
	}
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			imgEdge.at<uchar>(y, x) = (((conv_mat[y][x] - min) * 255) / (max - min));
		}
	}
	imshow("color", imgColor);
	//imwrite("result.jpg", result);
	imshow("result", imgEdge);
	waitKey(50000);
	for (int i = 0; i < height; i++)
	{
		delete[] conv_mat[i];
	}
	delete[] conv_mat;
}