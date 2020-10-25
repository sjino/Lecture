#define _USE_MATH_DEFINES
#include <stdio.h>
#include <iostream>
#include <algorithm>
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
#define WIN_SIZE 16
#define INTERVAL (WIN_SIZE/2)
#define thr_ratio 0.08//8퍼 이하를 threshold로 설정
int filter_x[3][3] = { {-1,0,1},{-1,0,1},{-1,0,1} };
int filter_y[3][3] = { {-1,-1,-1},{0,0,0},{1,1,1} };
using namespace cv;
using namespace std;
typedef struct Loss
{
	int x, y;
	float loss;
	Loss(int x, int y, float loss);

};
Loss::Loss(int x, int y, float loss)
{
	this->x = x;
	this->y = y;
	this->loss = loss;
}
void extract_HOG(Mat imgGray, vector<float>& histogram, int start_x, int start_y, int end_x, int end_y, int win_size, int interval)
{
	float sum, mag, deg;
	int  conv_x, conv_y;
	int his_index = 0, deg_index;
	float his_tmp[9];
	int his_size = 9 * (((end_x - start_x) / interval)) * (((end_y - start_y) / interval));
	histogram.clear();
	histogram.resize(his_size);
	for (int y = start_y; (y + interval) < end_y; y = y + interval)
	{
		for (int x = start_x; (x + interval) < end_x; x = x + interval)
		{

			sum = 0;
			for (int i = 0; i < 9; i++)
			{
				his_tmp[i] = 0;
			}
			for (int b = y; b < y + win_size; b++)
			{
				for (int a = x; a < x + win_size; a++)
				{
					conv_x = conv_y = 0;
					for (int i = b - 1; i <= b + 1; i++)
					{
						for (int j = a - 1; j <= a + 1; j++)
						{
							if ((i >= start_y && i < end_y) && (j >= start_x && j < end_x))
							{
								conv_x += imgGray.at<uchar>(i, j) * filter_x[i - (b - 1)][j - (a - 1)];
								conv_y += imgGray.at<uchar>(i, j) * filter_y[i - (b - 1)][j - (a - 1)];
							}
						}
					}
					mag = sqrtf(powf(conv_x, 2) + powf(conv_y, 2));
					deg = rad2deg((atan2(conv_y, conv_x)))+180;
					if (deg > 180)
						deg -= 180;
					deg_index = (deg / 20);
					if (deg_index >= 9)
						deg_index = 8;
					his_tmp[deg_index] += mag;
					sum += powf(mag, 2);
				}
			}
			for (int s = 0; s < 9; s++) //L2 normalization
			{
				his_tmp[s] /= sqrtf(sum);
			}
			for (int i = his_index; i < his_index + 9; i++)
			{
				histogram[i] = his_tmp[i - his_index];
			}
			his_index += 9;
		}
	}
}
float compute_his_loss(vector<float> histogram1, vector<float> histogram2)
{
	float diff = 0;
	if (histogram1.size() == histogram2.size())
	{
		for (int i = 0; i < histogram1.size(); i++)
		{
			diff += fabsf(histogram1[i] - histogram2[i]);
		}
		return diff;
	}
	else
	{
		cout << "히스토그램 크기가 다릅니다." << endl;
		return -1;
	}

}

int main()
{
	int height_ref, width_ref, height_tar, width_tar;
	Mat ref_img = imread("face_ref.bmp", IMREAD_COLOR);
	Mat tar_img = imread("face_tar.bmp", IMREAD_COLOR);
	Mat ref_imgGray = imread("face_ref.bmp", IMREAD_GRAYSCALE);
	Mat tar_imgGray = imread("face_tar.bmp", IMREAD_GRAYSCALE);
	width_ref = ref_imgGray.cols;
	height_ref = ref_imgGray.rows;
	width_tar = tar_imgGray.cols;
	height_tar = tar_imgGray.rows;

	float loss_max = -FLT_MAX, loss_min = FLT_MAX;
	float loss_thres;
	float loss_tmp;
	vector<Loss> loss;
	vector<float> histogram_ref, histogram_tar;

	extract_HOG(ref_imgGray, histogram_ref, 0, 0, width_ref, height_ref, WIN_SIZE, INTERVAL); //ref에 대하여 HOG 추출
	for (int y = 0; y < height_tar - height_ref; y++)
		//타겟 이미지 0~타겟이미지높이-레퍼런스이미지높이 
		//한픽셀씩  이동하면서 레퍼런스 이미지크기만큼에 대하여 HOG 계산
	{
		for (int x = 0; x < width_tar - width_ref; x++)
			//타겟 이미지 0~타겟이미지너비-레퍼런스이미지너비 
			//한픽셀씩 이동하면서 레퍼런스 이미지크기만큼에 대하여 HOG 계산
		{
			extract_HOG(tar_imgGray, histogram_tar, x, y, x + width_ref, y + height_ref, WIN_SIZE, INTERVAL);
			loss_tmp = compute_his_loss(histogram_ref, histogram_tar);
			loss_max = (loss_tmp > loss_max) ? loss_tmp : loss_max;
			loss_min = (loss_tmp < loss_min) ? loss_tmp : loss_min;
			loss.push_back(Loss(x, y, loss_tmp));
		}
	}
	loss_thres = loss_min + (float)((loss_max - loss_min) * thr_ratio);

	for (vector<Loss>::iterator it = loss.begin(); it != loss.end(); it++)
	{
		if ((*it).loss < loss_thres)
		{
			rectangle(tar_img, Point((*it).x, (*it).y), Point((*it).x + width_ref, (*it).y + height_ref), Scalar(0, 255, 0), 1, 8, 0);
		}
	}
	imshow("result", tar_img);
	cv::waitKey(50000);
	

	histogram_ref.clear();
	histogram_tar.clear();
}