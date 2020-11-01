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
#define blocksize_div 8;
#define tar_test 1
inline int pow2(int N)
{
	int ret = 1;
	for (int i = 0; i < N; i++)
	{
		ret *= 2;
	}
	return ret;
}
const int pow2table[8] = { 1,2,4,8,16,32,64,128 };
const int uniform_LBP_table[256] =
{
	0,1,2,3,4,58,5,6,7,58,58,58,8,58,9,10,11,58,58,58,58,58,58,58,12,58,58,58,13,58,
	14,15,16,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,17,58,58,58,58,58,58,58,18,
	58,58,58,19,58,20,21,22,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,
	58,58,58,58,58,58,58,58,58,58,58,58,23,58,58,58,58,58,58,58,58,58,58,58,58,58,
	58,58,24,58,58,58,58,58,58,58,25,58,58,58,26,58,27,28,29,30,58,31,58,58,58,32,58,
	58,58,58,58,58,58,33,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,34,58,58,58,58,
	58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,
	58,35,36,37,58,38,58,58,58,39,58,58,58,58,58,58,58,40,58,58,58,58,58,58,58,58,58,
	58,58,58,58,58,58,41,42,43,58,44,58,58,58,45,58,58,58,58,58,58,58,46,47,48,58,49,
	58,58,58,50,51,52,58,53,54,55,56,57
};

void compute_LBP(Mat input_img, vector<float>& histogram, int block_size)
{
	int interval;
	interval = block_size / 2;
	histogram.clear();
	uchar pixel_current;
	int pixel_LBP;
	int sum;
	float his_tmp[256];
	for (int y = 0; (y + interval) < input_img.rows; y = y + interval)
	{
		for (int x = 0; (x + interval) < input_img.cols; x = x + interval)
		{
			sum = 0;
			for (int i = 0; i < 256; i++)
			{
				his_tmp[i] = 0;
			}
			for (int b = y; b < y + block_size; b++)
			{
				for (int a = x; a < x + block_size; a++)
				{
					////for문 대체////
					int i, j;
					pixel_LBP = 0;
					pixel_current = input_img.at<uchar>(b, a);
					//#1
					j = a;
					i = b - 1;
					if ((i >= 0 && i < input_img.rows) && (j >= 0 && j < input_img.cols))
					{
						pixel_LBP += (input_img.at<uchar>(b, a) > input_img.at<uchar>(i, j)) ? pow2table[7] : 0;

					}
					//#2
					j = a + 1;
					i = b - 1;
					if ((i >= 0 && i < input_img.rows) && (j >= 0 && j < input_img.cols))
					{
						pixel_LBP += (input_img.at<uchar>(b, a) > input_img.at<uchar>(i, j)) ? pow2table[6] : 0;

					}
					//#3
					j = a + 1;
					i = b;
					if ((i >= 0 && i < input_img.rows) && (j >= 0 && j < input_img.cols))
					{
						pixel_LBP += (input_img.at<uchar>(b, a) > input_img.at<uchar>(i, j)) ? pow2table[5] : 0;

					}
					//#4
					j = a + 1;
					i = b + 1;
					if ((i >= 0 && i < input_img.rows) && (j >= 0 && j < input_img.cols))
					{
						pixel_LBP += (input_img.at<uchar>(b, a) > input_img.at<uchar>(i, j)) ? pow2table[4] : 0;

					}
					//#5
					j = a;
					i = b + 1;
					if ((i >= 0 && i < input_img.rows) && (j >= 0 && j < input_img.cols))
					{
						pixel_LBP += (input_img.at<uchar>(b, a) > input_img.at<uchar>(i, j)) ? pow2table[3] : 0;

					}
					//#6
					j = a - 1;
					i = b + 1;
					if ((i >= 0 && i < input_img.rows) && (j >= 0 && j < input_img.cols))
					{
						pixel_LBP += (input_img.at<uchar>(b, a) > input_img.at<uchar>(i, j)) ? pow2table[2] : 0;

					}
					//#7
					j = a - 1;
					i = b;
					if ((i >= 0 && i < input_img.rows) && (j >= 0 && j < input_img.cols))
					{
						pixel_LBP += (input_img.at<uchar>(b, a) > input_img.at<uchar>(i, j)) ? pow2table[1] : 0;

					}
					//#8
					j = a - 1;
					i = b - 1;
					if ((i >= 0 && i < input_img.rows) && (j >= 0 && j < input_img.cols))
					{
						pixel_LBP += (input_img.at<uchar>(b, a) > input_img.at<uchar>(i, j)) ? pow2table[0] : 0;

					}
					his_tmp[pixel_LBP] += 1;
				}
			}
			for (int s = 0; s < 256; s++)//L2 normalization
			{
				sum += powf(his_tmp[s], 2);
			}
			for (int p = 0; p < 256; p++)
			{
				histogram.push_back((his_tmp[p] / sqrtf(sum)));
			}
		}
	}
}
void compute_uni_LBP(Mat input_img, vector<float>& histogram, int block_size)
{
	int interval;
	interval = block_size / 2;
	histogram.clear();
	uchar pixel_current;
	int pixel_LBP;
	int sum;
	float his_tmp[59];
	for (int y = 0; (y + interval) < input_img.rows; y = y + interval)
	{
		for (int x = 0; (x + interval) < input_img.cols; x = x + interval)
		{
			sum = 0;
			for (int i = 0; i < 59; i++)
			{
				his_tmp[i] = 0;
			}
			for (int b = y; b < y + block_size; b++)
			{
				for (int a = x; a < x + block_size; a++)
				{
					////for문 대체////
					int i, j;
					pixel_LBP = 0;
					pixel_current = input_img.at<uchar>(b, a);
					//#1
					j = a;
					i = b - 1;
					if ((i >= 0 && i < input_img.rows) && (j >= 0 && j < input_img.cols))
					{
						pixel_LBP += (input_img.at<uchar>(b, a) > input_img.at<uchar>(i, j)) ? pow2table[7] : 0;

					}
					//#2
					j = a + 1;
					i = b - 1;
					if ((i >= 0 && i < input_img.rows) && (j >= 0 && j < input_img.cols))
					{
						pixel_LBP += (input_img.at<uchar>(b, a) > input_img.at<uchar>(i, j)) ? pow2table[6] : 0;

					}
					//#3
					j = a + 1;
					i = b;
					if ((i >= 0 && i < input_img.rows) && (j >= 0 && j < input_img.cols))
					{
						pixel_LBP += (input_img.at<uchar>(b, a) > input_img.at<uchar>(i, j)) ? pow2table[5] : 0;

					}
					//#4
					j = a + 1;
					i = b + 1;
					if ((i >= 0 && i < input_img.rows) && (j >= 0 && j < input_img.cols))
					{
						pixel_LBP += (input_img.at<uchar>(b, a) > input_img.at<uchar>(i, j)) ? pow2table[4] : 0;

					}
					//#5
					j = a;
					i = b + 1;
					if ((i >= 0 && i < input_img.rows) && (j >= 0 && j < input_img.cols))
					{
						pixel_LBP += (input_img.at<uchar>(b, a) > input_img.at<uchar>(i, j)) ? pow2table[3] : 0;

					}
					//#6
					j = a - 1;
					i = b + 1;
					if ((i >= 0 && i < input_img.rows) && (j >= 0 && j < input_img.cols))
					{
						pixel_LBP += (input_img.at<uchar>(b, a) > input_img.at<uchar>(i, j)) ? pow2table[2] : 0;

					}
					//#7
					j = a - 1;
					i = b;
					if ((i >= 0 && i < input_img.rows) && (j >= 0 && j < input_img.cols))
					{
						pixel_LBP += (input_img.at<uchar>(b, a) > input_img.at<uchar>(i, j)) ? pow2table[1] : 0;

					}
					//#8
					j = a - 1;
					i = b - 1;
					if ((i >= 0 && i < input_img.rows) && (j >= 0 && j < input_img.cols))
					{
						pixel_LBP += (input_img.at<uchar>(b, a) > input_img.at<uchar>(i, j)) ? pow2table[0] : 0;

					}
					his_tmp[uniform_LBP_table[pixel_LBP]] += 1;
				}
			}
			for (int s = 0; s < 59; s++)//L2 normalization
			{
				sum += powf(his_tmp[s], 2);
			}
			for (int p = 0; p < 59; p++)
			{
				histogram.push_back((his_tmp[p] / sqrtf(sum)));
			}
		}
	}
}
void Convert_LBP_image(Mat input_img, Mat& output_img)
{
	uchar pixel_current;
	int pixel_LBP;
	int sum;
	for (int y = 0; y < input_img.rows; y++)
	{
		for (int x = 0; x < input_img.cols; x++)
		{

			////for문 대체////
			int i, j;
			pixel_LBP = 0;
			pixel_current = input_img.at<uchar>(y, x);
			//#1
			j = x;
			i = y - 1;
			if ((i >= 0 && i < input_img.rows) && (j >= 0 && j < input_img.cols))
			{
				pixel_LBP += (pixel_current > input_img.at<uchar>(i, j)) ? pow2(7) : 0;

			}
			//#2
			j = x + 1;
			i = y - 1;
			if ((i >= 0 && i < input_img.rows) && (j >= 0 && j < input_img.cols))
			{
				pixel_LBP += (pixel_current > input_img.at<uchar>(i, j)) ? pow2(6) : 0;

			}
			//#3
			j = x + 1;
			i = y;
			if ((i >= 0 && i < input_img.rows) && (j >= 0 && j < input_img.cols))
			{
				pixel_LBP += (pixel_current > input_img.at<uchar>(i, j)) ? pow2(5) : 0;

			}
			//#4
			j = x + 1;
			i = y + 1;
			if ((i >= 0 && i < input_img.rows) && (j >= 0 && j < input_img.cols))
			{
				pixel_LBP += (pixel_current > input_img.at<uchar>(i, j)) ? pow2(4) : 0;

			}
			//#5
			j = x;
			i = y + 1;
			if ((i >= 0 && i < input_img.rows) && (j >= 0 && j < input_img.cols))
			{
				pixel_LBP += (pixel_current > input_img.at<uchar>(i, j)) ? pow2(3) : 0;

			}
			//#6
			j = x - 1;
			i = y + 1;
			if ((i >= 0 && i < input_img.rows) && (j >= 0 && j < input_img.cols))
			{
				pixel_LBP += (pixel_current > input_img.at<uchar>(i, j)) ? pow2(2) : 0;

			}
			//#7
			j = x - 1;
			i = y;
			if ((i >= 0 && i < input_img.rows) && (j >= 0 && j < input_img.cols))
			{
				pixel_LBP += (pixel_current > input_img.at<uchar>(i, j)) ? pow2(1) : 0;

			}
			//#8
			j = x - 1;
			i = y - 1;
			if ((i >= 0 && i < input_img.rows) && (j >= 0 && j < input_img.cols))
			{
				pixel_LBP += (pixel_current > input_img.at<uchar>(i, j)) ? pow2(0) : 0;

			}
			/*
			for (int i = b - 1; i <= b + 1; i++)
			{
				for (int j = a - 1; j <= a + 1; j++)
				{
					if ((i >= 0 && i < input_img.rows) && (j >= 0 && j < input_img.cols))
					{
					}
				}
			}*/
			output_img.at<uchar>(y, x) = (uchar)pixel_LBP;

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
			diff += fabsf(((float)histogram1[i] - (float)histogram2[i]));
		}
		return diff;
	}
	else
	{
		cout << "히스토그램 크기가 다릅니다." << endl;
		cout << "his1_size: " << histogram1.size() << " / " << "his2_size: " << histogram2.size() << endl;
		return -1;
	}

}
int main()
{
	VideoCapture capture(0);
	Mat frame_measure;
	if (!capture.isOpened())
	{
		printf("Couldn't open the web camera!\n");
		return 0;
	}
	Mat frame_ref;

	CascadeClassifier cascade;
	vector<Rect> faces_ref, faces_tar;
	cascade.load("C:/opencv/sources/data/lbpcascades/lbpcascade_frontalface.xml");//학습모델 경로
	while (true)
	{
		capture >> frame_ref;
		cascade.detectMultiScale(frame_ref, faces_ref, 1.1, 4, 0 | CASCADE_SCALE_IMAGE, Size(10, 10));
		for (int y = 0; y < faces_ref.size(); y++)
		{
			Point lb(faces_ref[y].x + faces_ref[y].width, faces_ref[y].y + faces_ref[y].height);
			Point tr(faces_ref[y].x, faces_ref[y].y);
			rectangle(frame_ref, lb, tr, Scalar(0, 255, 0), 3, 8, 0);
		}
		imshow("Esc키를 눌러 얼굴을 등록해 주세요", frame_ref);
		if (waitKey(30) >= 0)
		{
			if (faces_ref.size() != 1)
			{
				cout << "인식된 얼굴이 없거나 두 개 이상입니다." << endl;
			}
			else
			{
				cout << "얼굴이 등록되었습니다." << endl;
				break;
			}
		}
		faces_ref.clear();
	}



	vector<float> his_ref, his_tar;
	Mat face_gray_ref, face_gray_ref_tmp;
	Mat frame_gray_ref;
	int block_size_ref;
	cvtColor(frame_ref, frame_gray_ref, COLOR_RGB2GRAY);
	face_gray_ref = frame_gray_ref(Range(faces_ref[0].y, faces_ref[0].y + faces_ref[0].height), Range(faces_ref[0].x, faces_ref[0].x + faces_ref[0].width));

	block_size_ref = (face_gray_ref.cols > face_gray_ref.rows) ? face_gray_ref.rows : face_gray_ref.cols; // 더 작은것
	block_size_ref /= blocksize_div;
	//compute_LBP(face_gray_ref, his_ref, block_size_ref);
	compute_uni_LBP(face_gray_ref, his_ref, block_size_ref);
	cout << "ref 완료   size: " << his_ref.size() << endl;
# if tar_test
	Mat frame_tar;
	Mat face_gray_tar, face_gray_tar_tmp;
	Mat frame_gray_tar;
	int block_size_tar;
	float loss, threshold;
	threshold = 500;
	while (true)
	{
		capture >> frame_tar;
		cascade.detectMultiScale(frame_tar, faces_tar, 1.1, 4, 0 | CASCADE_SCALE_IMAGE, Size(10, 10));

		if (faces_tar.size() == 1)
		{
			cvtColor(frame_tar, frame_gray_tar, COLOR_RGB2GRAY);
			face_gray_tar_tmp = frame_gray_ref(Range(faces_tar[0].y, faces_tar[0].y + faces_tar[0].height), Range(faces_tar[0].x, faces_tar[0].x + faces_tar[0].width));
			resize(face_gray_tar_tmp, face_gray_tar, Size(face_gray_ref.cols, face_gray_ref.rows));
			block_size_tar = (face_gray_tar.cols > face_gray_tar.rows) ? face_gray_tar.rows : face_gray_tar.cols; // 더 작은것
			block_size_tar /= blocksize_div;
			//compute_LBP(face_gray_tar, his_tar, block_size_tar);
			compute_uni_LBP(face_gray_tar, his_tar, block_size_tar);
			loss = compute_his_loss(his_ref, his_tar);
			cout << "loss: " << loss << endl;
			for (int y = 0; y < faces_tar.size(); y++)
			{
				Point lb(faces_tar[y].x + faces_tar[y].width, faces_tar[y].y + faces_tar[y].height);
				Point tr(faces_tar[y].x, faces_tar[y].y);
				rectangle(frame_tar, lb, tr, ((loss < threshold&& loss>0) ? Scalar(0, 0, 255) : Scalar(0, 255, 0)), 3, 8, 0);
			}

		}
		else
		{
			for (int y = 0; y < faces_tar.size(); y++)
			{
				Point lb(faces_tar[y].x + faces_tar[y].width, faces_tar[y].y + faces_tar[y].height);
				Point tr(faces_tar[y].x, faces_tar[y].y);
				rectangle(frame_tar, lb, tr, Scalar(0, 255, 0), 3, 8, 0);
			}
		}
		imshow("얼굴을 인식시켜주세요", frame_tar);
		if (waitKey(50) >= 0) break;
		faces_tar.clear();
	}
#endif
}