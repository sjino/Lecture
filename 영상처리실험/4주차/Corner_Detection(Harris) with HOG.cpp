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
#define k 0.04
#define sigma 1
#define thr_ratio 0.15//15퍼 이상을 threshold로 설정
#define USE_GAUSSIAN 0 //가우시안 필터 사용하면 1 아니면 나머지

struct Corner
{
	int x, y;
	float histogram[9];
	Corner(int x, int y);
};
Corner::Corner(int x, int y)
{
	this->x = x;
	this->y = y;
	for (int i = 0; i < 9; i++)
	{
		histogram[i] = 0;
	}
}

int filter_x[3][3] = { {-1,0,1},{-1,0,1},{-1,0,1} };
int filter_y[3][3] = { {-1,-1,-1},{0,0,0},{1,1,1} };
float Gaussian_filter[5][5];

using namespace cv;
using namespace std;
void Harris_Corner(Mat img, bool** is_Corner)
{
	Mat imgGray;
	if (img.type() == CV_8UC1)
	{
		img.copyTo(imgGray);
	}
	else
	{
		cvtColor(img, imgGray, COLOR_BGR2GRAY);
	}
	int height = img.rows, width = img.cols, conv_x, conv_y;
	const int win_size = 5;
	float** conv_mat_x = new float* [height];
	float** conv_mat_y = new float* [height];
	float** R_mat = new float* [height];
	is_Corner = new bool* [height];
	Mat imgGray_Gfilter(height, width, CV_8UC1);
	for (int i = 0; i < height; i++)
	{
		conv_mat_x[i] = new float[width];
		conv_mat_y[i] = new float[width];
		R_mat[i] = new float[width];
		is_Corner[i] = new bool[width];
	}

	vector<Corner> corner_ori;
	float mag, deg;
	int deg_index;

	float max_ori = FLT_MIN, min_ori = FLT_MAX;
	float thr_ori;
	float conv_G;
#if USE_GAUSSIAN == 1
	for (int i = 0; i < 5; i++) //가우시안 필터 생성
	{
		for (int j = 0; j < 5; j++)
		{
			Gaussian_filter[i][j] = (float)(1 / (sqrtf(2 * M_PI) * sigma)) * exp(-((powf(i - 2, 2) + powf(j - 2, 2)) / (2 * sigma * sigma)));
		}
	}

	for (int y = 0; y < height; y++) //가우시안 필터 적용
	{
		for (int x = 0; x < width; x++)
		{
			conv_G = 0;
			for (int i = y - 2; i <= y + 2; i++)
			{
				for (int j = x - 2; j <= x + 2; j++)
				{
					if ((i >= 0 && i < height) && (j >= 0 && j < width))
					{
						conv_G += imgGray.at<uchar>(i, j) * Gaussian_filter[i - (y - 2)][j - (x - 2)];
					}
				}
			}
			imgGray_Gfilter.at<uchar>(y, x) = conv_G;
		}
	}
	for (int i = 0; i < height; i++) //다시 복사
	{
		for (int j = 0; j < width; j++)
		{
			imgGray.at<uchar>(i, j) = imgGray_Gfilter.at<uchar>(i, j);
		}
	}
#endif

	for (int y = 0; y < height; y++) //Ix Iy 계산(필터로 구현)
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
			conv_mat_x[y][x] = conv_x;
			conv_mat_y[y][x] = conv_y;
		}
	}


	///0~255 정규화 진행 X 


	float M_ori[2][2];
	for (int y = 0; y < height; y++) //R계산 R=det(M)-K*tr^2(M)
	{
		for (int x = 0; x < width; x++)
		{
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
				{
					M_ori[i][j] = 0;
				}
			}
			for (int i = y - win_size / 2; i <= y + win_size / 2; i++) //윈도우(5) 계산
			{
				for (int j = x - win_size / 2; j <= x + win_size / 2; j++)
				{
					if ((i >= 0 && i < height) && (j >= 0 && j < width))
					{
						M_ori[0][0] += conv_mat_x[i][j] * conv_mat_x[i][j];
						M_ori[0][1] += conv_mat_x[i][j] * conv_mat_y[i][j];
						M_ori[1][0] += conv_mat_x[i][j] * conv_mat_y[i][j];
						M_ori[1][1] += conv_mat_y[i][j] * conv_mat_y[i][j];
						
					}
				}
			}
			R_mat[y][x] = /*det*/(M_ori[0][0] * M_ori[1][1] - M_ori[0][1] * M_ori[1][0]) - k * powf(/*trace*/(M_ori[0][0] + M_ori[1][1]), 2);
			max_ori = (((R_mat[y][x]) > (max_ori)) ? (R_mat[y][x]) : (max_ori));
			min_ori = (((R_mat[y][x]) < (min_ori)) ? (R_mat[y][x]) : (min_ori));

		}
	}
	thr_ori = min_ori + (float)((max_ori - min_ori) * thr_ratio);
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			is_Corner[y][x] = false;
		}
	}
	for (int y = 0; y < height; y++)//Corner 분류
	{
		for (int x = 0; x < width; x++)
		{
			if (R_mat[y][x] >= thr_ori)
			{
				is_Corner[y][x] = true;
			}
		}
	}

	int i_max = 0, j_max = 0, i_max_cmp1 = 0, j_max_cmp1 = 0;
	bool ischanged = false, ischanged_cmp1 = false;
	for (int y = 0; y < height; y++)//Non-maxima suppression
	{
		for (int x = 0; x < width; x++)
		{
			max_ori = FLT_MIN;
			for (int i = y - win_size / 2; i <= y + win_size / 2; i++) //윈도우(5) 계산
			{
				for (int j = x - win_size / 2; j <= x + win_size / 2; j++)
				{
					if ((i >= 0 && i < height) && (j >= 0 && j < width))
					{
						if (is_Corner[i][j] == true)
						{
							ischanged = true;
							is_Corner[i][j] = false;
							if ((R_mat[i][j]) > (max_ori))
							{
								max_ori = R_mat[i][j];
								i_max = i;
								j_max = j;
							}
						}
						
					}
				}
			}
			if (ischanged == true)
			{
				is_Corner[i_max][j_max] = true;
				ischanged = false;
			}
			
		}
	}
	for (int i = 0; i < height; i++)
	{
		delete[] conv_mat_x[i];
		delete[] conv_mat_y[i];
		delete[] R_mat[i];
	}
	delete[] conv_mat_x;
	delete[] conv_mat_y;
	delete[] R_mat;

}
void main()
{
	int height, width, conv_x, conv_y, conv_x_cmp1, conv_y_cmp1;
	const int win_size = 5;
	Mat imgColor = imread("ref.bmp", IMREAD_COLOR);
	Mat imgGray = imread("ref.bmp", IMREAD_GRAYSCALE);
	Mat compimg1Color = imread("tar.bmp", IMREAD_COLOR);
	Mat compimg1Gray = imread("tar.bmp", IMREAD_GRAYSCALE);
	width = imgGray.cols;
	height = imgGray.rows;
	Mat imgGray_Gfilter(height, width, CV_8UC1);
	Mat compimg1Gray_Gfilter(height, width, CV_8UC1);
	Mat resultimg(height, 2 * width, CV_8UC3, Scalar(0, 0, 0));



	float** conv_mat_x = new float* [height];
	float** conv_mat_x_cmp1 = new float* [height];
	float** conv_mat_y = new float* [height];
	float** conv_mat_y_cmp1 = new float* [height];
	float** R_mat = new float* [height];
	float** R_mat_cmp1 = new float* [height];
	bool** is_Corner = new bool* [height];
	bool** is_Corner_cmp1 = new bool* [height];

	for (int i = 0; i < height; i++)
	{
		conv_mat_x[i] = new float[width];
		conv_mat_x_cmp1[i] = new float[width];
		conv_mat_y[i] = new float[width];
		conv_mat_y_cmp1[i] = new float[width];
		R_mat[i] = new float[width];
		R_mat_cmp1[i] = new float[width];
		is_Corner[i] = new bool[width];
		is_Corner_cmp1[i] = new bool[width];
	}

	vector<Corner> corner_ori, corner_cmp1;
	float mag, deg, mag_cmp1, deg_cmp1;
	int deg_index;

	float max_ori = FLT_MIN, min_ori = FLT_MAX, max_cmp1 = FLT_MIN, min_cmp1 = FLT_MAX;
	float thr_ori, thr_cmp1;
	float conv_G, conv_G_cmp1;

#if USE_GAUSSIAN==1
	for (int i = 0; i < 5; i++) //가우시안 필터 생성
	{
		for (int j = 0; j < 5; j++)
		{
			Gaussian_filter[i][j] = (float)(1 / (sqrtf(2 * M_PI) * sigma)) * exp(-((powf(i - 2, 2) + powf(j - 2, 2)) / (2 * sigma * sigma)));
		}
	}

	for (int y = 0; y < height; y++) //가우시안 필터 적용
	{
		for (int x = 0; x < width; x++)
		{
			conv_G = conv_G_cmp1 = 0;
			for (int i = y - 2; i <= y + 2; i++)
			{
				for (int j = x - 2; j <= x + 2; j++)
				{
					if ((i >= 0 && i < height) && (j >= 0 && j < width))
					{
						conv_G += imgGray.at<uchar>(i, j) * Gaussian_filter[i - (y - 2)][j - (x - 2)];
						conv_G_cmp1 += compimg1Gray.at<uchar>(i, j) * Gaussian_filter[i - (y - 2)][j - (x - 2)];
					}
				}
			}
			imgGray_Gfilter.at<uchar>(y, x) = conv_G;
			compimg1Gray_Gfilter.at<uchar>(y, x) = conv_G_cmp1;
		}
}
	for (int i = 0; i < height; i++) //다시 복사
	{
		for (int j = 0; j < width; j++)
		{
			imgGray.at<uchar>(i, j) = imgGray_Gfilter.at<uchar>(i, j);
			compimg1Gray.at<uchar>(i, j) = compimg1Gray_Gfilter.at<uchar>(i, j);
		}
	}
#endif

	for (int y = 0; y < height; y++) //Ix Iy 계산(필터로 구현)
	{
		for (int x = 0; x < width; x++)
		{
			conv_x = 0;
			conv_y = 0;
			conv_x_cmp1 = 0;
			conv_y_cmp1 = 0;
			for (int i = y - 1; i <= y + 1; i++)
			{
				for (int j = x - 1; j <= x + 1; j++)
				{
					if ((i >= 0 && i < height) && (j >= 0 && j < width))
					{
						conv_x += imgGray.at<uchar>(i, j) * filter_x[i - (y - 1)][j - (x - 1)];
						conv_y += imgGray.at<uchar>(i, j) * filter_y[i - (y - 1)][j - (x - 1)];
						conv_x_cmp1 += compimg1Gray.at<uchar>(i, j) * filter_x[i - (y - 1)][j - (x - 1)];
						conv_y_cmp1 += compimg1Gray.at<uchar>(i, j) * filter_y[i - (y - 1)][j - (x - 1)];
					}

				}
			}
			conv_mat_x[y][x] = conv_x;
			conv_mat_x_cmp1[y][x] = conv_x_cmp1;
			conv_mat_y[y][x] = conv_y;
			conv_mat_y_cmp1[y][x] = conv_y_cmp1;
		}
	}


	///0~255 정규화 진행 X 



	float M_ori[2][2], M_cmp1[2][2];
	for (int y = 0; y < height; y++) //R계산 R=det(M)-K*tr^2(M)
	{
		for (int x = 0; x < width; x++)
		{
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
				{
					M_ori[i][j] = M_cmp1[i][j] = 0;
				}
			}
			for (int i = y - win_size / 2; i <= y + win_size / 2; i++) //윈도우(5) 계산
			{
				for (int j = x - win_size / 2; j <= x + win_size / 2; j++)
				{
					if ((i >= 0 && i < height) && (j >= 0 && j < width))
					{
						M_ori[0][0] += conv_mat_x[i][j] * conv_mat_x[i][j];
						M_ori[0][1] += conv_mat_x[i][j] * conv_mat_y[i][j];
						M_ori[1][0] += conv_mat_x[i][j] * conv_mat_y[i][j];
						M_ori[1][1] += conv_mat_y[i][j] * conv_mat_y[i][j];
						M_cmp1[0][0] += conv_mat_x_cmp1[i][j] * conv_mat_x_cmp1[i][j];
						M_cmp1[0][1] += conv_mat_x_cmp1[i][j] * conv_mat_y_cmp1[i][j];
						M_cmp1[1][0] += conv_mat_x_cmp1[i][j] * conv_mat_y_cmp1[i][j];
						M_cmp1[1][1] += conv_mat_y_cmp1[i][j] * conv_mat_y_cmp1[i][j];
					}
				}
			}
			R_mat[y][x] = /*det*/(M_ori[0][0] * M_ori[1][1] - M_ori[0][1] * M_ori[1][0]) - k * powf(/*trace*/(M_ori[0][0] + M_ori[1][1]), 2);
			R_mat_cmp1[y][x] = /*det*/(M_cmp1[0][0] * M_cmp1[1][1] - M_cmp1[0][1] * M_cmp1[1][0]) - k * powf(/*trace*/(M_cmp1[0][0] + M_cmp1[1][1]), 2);
			max_ori = (((R_mat[y][x]) > (max_ori)) ? (R_mat[y][x]) : (max_ori));
			min_ori = (((R_mat[y][x]) < (min_ori)) ? (R_mat[y][x]) : (min_ori));
			max_cmp1 = (((R_mat_cmp1[y][x]) > (max_cmp1)) ? (R_mat_cmp1[y][x]) : (max_cmp1));
			min_cmp1 = (((R_mat_cmp1[y][x]) < (min_cmp1)) ? (R_mat_cmp1[y][x]) : (min_cmp1));
		}
	}
	thr_ori = min_ori + (float)((max_ori - min_ori) * thr_ratio);
	thr_cmp1 = min_cmp1 + (float)((max_cmp1 - min_cmp1) * thr_ratio);
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			is_Corner[y][x] = is_Corner_cmp1[y][x] = false;
		}
	}
	for (int y = 0; y < height; y++)//Corner 분류
	{
		for (int x = 0; x < width; x++)
		{
			if (R_mat[y][x] >= thr_ori)
			{
				is_Corner[y][x] = true;
			}
			if (R_mat_cmp1[y][x] >= thr_cmp1)
			{
				is_Corner_cmp1[y][x] = true;
			}
		}
	}

	int i_max = 0, j_max = 0, i_max_cmp1 = 0, j_max_cmp1 = 0;
	bool ischanged = false, ischanged_cmp1 = false;
	for (int y = 0; y < height; y++)//Non-maxima suppression
	{
		for (int x = 0; x < width; x++)
		{
			max_ori = max_cmp1 = FLT_MIN;
			for (int i = y - win_size / 2; i <= y + win_size / 2; i++) //윈도우(5) 계산
			{
				for (int j = x - win_size / 2; j <= x + win_size / 2; j++)
				{
					if ((i >= 0 && i < height) && (j >= 0 && j < width))
					{
						if (is_Corner[i][j] == true)
						{
							ischanged = true;
							is_Corner[i][j] = false;
							if ((R_mat[i][j]) > (max_ori))
							{
								max_ori = R_mat[i][j];
								i_max = i;
								j_max = j;
							}
						}
						if (is_Corner_cmp1[i][j] == true)
						{
							ischanged_cmp1 = true;
							is_Corner_cmp1[i][j] = false;
							if ((R_mat_cmp1[i][j]) > (max_cmp1))
							{
								max_cmp1 = R_mat_cmp1[i][j];
								i_max_cmp1 = i;
								j_max_cmp1 = j;
							}
						}
					}
				}
			}
			if (ischanged == true)
			{
				is_Corner[i_max][j_max] = true;
				ischanged = false;
			}
			if (ischanged_cmp1 == true)
			{
				is_Corner_cmp1[i_max_cmp1][j_max_cmp1] = true;
				ischanged_cmp1 = false;
			}
		}
	}
	Scalar c;
	Point pCenter;
	int radius = 3;
	c.val[0] = 0;
	c.val[1] = 0;
	c.val[2] = 255;

	for (int y = 0; y < height; y++) // 코너 픽셀 마킹
	{
		for (int x = 0; x < width; x++)
		{
			if (is_Corner[y][x] == true)
			{
				pCenter.x = x;
				pCenter.y = y;
				circle(imgColor, pCenter, radius, c, 2, 8, 0);
				corner_ori.push_back(Corner(x, y)); //코너 좌표 저장
			}
			if (is_Corner_cmp1[y][x] == true)
			{
				pCenter.x = x;
				pCenter.y = y;
				circle(compimg1Color, pCenter, radius, c, 2, 8, 0);
				corner_cmp1.push_back(Corner(x, y)); //코너 좌표 저장
			}
		}
	}

	//결과 이미지 생성
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			resultimg.at<Vec3b>(y, x)[0] = imgColor.at<Vec3b>(y, x)[0];
			resultimg.at<Vec3b>(y, x)[1] = imgColor.at<Vec3b>(y, x)[1];
			resultimg.at<Vec3b>(y, x)[2] = imgColor.at<Vec3b>(y, x)[2];
			resultimg.at<Vec3b>(y, x + width)[0] = compimg1Color.at<Vec3b>(y, x)[0];
			resultimg.at<Vec3b>(y, x + width)[1] = compimg1Color.at<Vec3b>(y, x)[1];
			resultimg.at<Vec3b>(y, x + width)[2] = compimg1Color.at<Vec3b>(y, x)[2];
		}
	}


	float sum = 0, sum1 = 0;
	for (vector<Corner>::iterator it = corner_ori.begin(); it != corner_ori.end(); it++)
	{
		sum = 0;

		for (int b = (*it).y - 8; b < (*it).y + 8; b++)
		{
			for (int a = (*it).x - 8; a < (*it).x + 8; a++)
			{
				conv_x = conv_y = 0;

				for (int i = b - 1; i <= b + 1; i++)
				{
					for (int j = a - 1; j <= a + 1; j++)
					{
						if ((i >= 0 && i < height) && (j >= 0 && j < width))
						{
							conv_x += imgGray.at<uchar>(i, j) * filter_x[i - (b - 1)][j - (a - 1)];
							conv_y += imgGray.at<uchar>(i, j) * filter_y[i - (b - 1)][j - (a - 1)];
						}
					}
				}
				mag = sqrtf(powf(conv_x, 2) + powf(conv_y, 2));
				deg = rad2deg((atan2(conv_y, conv_x))) + 90 + 180;
				if (deg > 180)
					deg -= 180;
				deg_index = (deg / 20);
				if (deg_index >= 9)
					deg_index = 8;
				(*it).histogram[deg_index] += mag;
				sum += powf(mag, 2);

			}
		}
		for (int s = 0; s < 9; s++) //L2 normalization
		{
			(*it).histogram[s] /= sqrtf(sum);
		}

	}
	for (vector<Corner>::iterator it = corner_cmp1.begin(); it != corner_cmp1.end(); it++)
	{
		sum1 = 0;
		for (int b = (*it).y - 8; b < (*it).y + 8; b++)
		{
			for (int a = (*it).x - 8; a < (*it).x + 8; a++)
			{
				conv_x_cmp1 = conv_y_cmp1 = 0;
				for (int i = b - 1; i <= b + 1; i++)
				{
					for (int j = a - 1; j <= a + 1; j++)
					{
						if ((i >= 0 && i < height) && (j >= 0 && j < width))
						{
							conv_x_cmp1 += compimg1Gray.at<uchar>(i, j) * filter_x[i - (b - 1)][j - (a - 1)];
							conv_y_cmp1 += compimg1Gray.at<uchar>(i, j) * filter_y[i - (b - 1)][j - (a - 1)];

						}
					}
				}
				mag_cmp1 = sqrtf(powf(conv_x_cmp1, 2) + powf(conv_y_cmp1, 2));
				deg_cmp1 = rad2deg((atan2(conv_y_cmp1, conv_x_cmp1))) + 90 + 180;
				if (deg_cmp1 > 180)
					deg_cmp1 -= 180;
				deg_index = (deg_cmp1 / 20);
				if (deg_index >= 9)
					deg_index = 8;
				(*it).histogram[deg_index] += mag_cmp1;
				sum1 += powf(mag_cmp1, 2);
			}
		}
		for (int s = 0; s < 9; s++) //L2 normalization
		{
			(*it).histogram[s] /= sqrtf(sum1);
		}
	}


	float diff_min = FLT_MAX, diff = 0;
	vector<Corner>::iterator it_min = corner_cmp1.begin();
	for (vector<Corner>::iterator it = corner_ori.begin(); it != corner_ori.end(); it++)// 원본 비교 픽셀 히스토그램 차이값 계산 및 유사 픽셀 찾기
	{
		for (vector<Corner>::iterator it1 = corner_cmp1.begin(); it1 != corner_cmp1.end(); it1++)
		{
			diff = 0;
			for (int i = 0; i < 9; i++)
			{
				diff += fabsf((*it).histogram[i] - (*it1).histogram[i]);
			}
			if (diff_min > diff)
			{
				diff_min = diff;
				it_min = it1;
			}
			if (it1 == corner_cmp1.end() - 1)
			{
				line(resultimg, Point((*it).x, (*it).y), Point((*it_min).x + width, (*it_min).y), Scalar(255, 0, 0), 2, 9, 0);
			}
		}
		diff_min = FLT_MAX;
		it_min = corner_cmp1.begin();
	}

	cv::imshow("결과", resultimg);

	cv::waitKey(50000);


	for (int i = 0; i < height; i++)
	{
		delete[] conv_mat_x[i];
		delete[] conv_mat_x_cmp1[i];
		delete[] conv_mat_y[i];
		delete[] conv_mat_y_cmp1[i];
		delete[] R_mat[i];
		delete[] R_mat_cmp1[i];
		delete[] is_Corner[i];
		delete[] is_Corner_cmp1[i];
	}
	delete[] conv_mat_x;
	delete[] conv_mat_x_cmp1;
	delete[] conv_mat_y;
	delete[] conv_mat_y_cmp1;
	delete[] R_mat;
	delete[] R_mat_cmp1;
	delete[] is_Corner;
	delete[] is_Corner_cmp1;
}