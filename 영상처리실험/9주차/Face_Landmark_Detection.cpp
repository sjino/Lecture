#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/ml/ml.hpp>
#include "include/ldmarkmodel.h"


using namespace cv;
using namespace cv::ml;
using namespace std;
#define blocksize_div 8;
#define tar_test 1
#define INDEX_EXCEPT_OUTLINE 27
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
void compute_uni_LBP(const Mat& input_img, vector<float>& histogram, int cx, int cy, int window_size)
//center_x, center_y 주변 윈도우에 대해
//한 윈도우에 대해서만 계산하므로 벡터 clear X, 이미지 전체 X
{
	int half_window_size;
	half_window_size = window_size / 2;
	uchar pixel_current;
	int pixel_LBP;
	int sum = 0;
	float his_tmp[59];

	int y = cy - half_window_size;
	int x = cx - half_window_size;

	for (int i = 0; i < 59; i++)
	{
		his_tmp[i] = 0;
	}
	for (int b = y; b < y + window_size; b++)
	{
		for (int a = x; a < x + window_size; a++)
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
void compute_uni_LBP_aroundpoints(const Mat& input_img, vector<float>& histogram, vector<Point> points_lmark, int block_size, int begin, int end)
{
	histogram.clear();
	for (vector<Point>::iterator it = points_lmark.begin() + begin; it != points_lmark.begin() + end; it++)
	{
		compute_uni_LBP(input_img, histogram, (*it).x, (*it).y, block_size);
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
	ldmarkmodel modelt;
	std::string modelFilePath = "roboman-landmark-model.bin";
	if (!load_ldmarkmodel(modelFilePath, modelt))
	{
		cout << "Couldn't open roboman-landmark-model.bin" << endl;
		return 0;
	}

	VideoCapture capture(0);
	Mat frame_measure;
	if (!capture.isOpened())
	{
		cout << "Couldn't open the web camera!\n" << endl;
		return 0;
	}
	Mat frame_ref, frame_disp;
	Mat current_shape_ref;
	int numLandmarks_ref;
	CascadeClassifier cascade;
	vector<Rect> faces_ref, faces_tar;
	cascade.load("haar_roboman_ff_alt2.xml");//학습모델 경로
	while (true)
	{
		capture >> frame_disp >> frame_ref;
		modelt.track(frame_disp, current_shape_ref);
		Vec3d eav;
		//modelt.EstimateHeadPose(current_shape, eav);
		//modelt.drawPose(frame_ref, current_shape, 50);
		cascade.detectMultiScale(frame_disp, faces_ref, 1.1, 4, 0 | CASCADE_SCALE_IMAGE, Size(10, 10));

		for (int y = 0; y < faces_ref.size(); y++)
		{
			Point lb(faces_ref[y].x + faces_ref[y].width, faces_ref[y].y + faces_ref[y].height);
			Point tr(faces_ref[y].x, faces_ref[y].y);

			rectangle(frame_disp, lb, tr, Scalar(0, 255, 0), 3, 8, 0);
		}
		numLandmarks_ref = current_shape_ref.cols / 2;
		for (int j = 0; j < numLandmarks_ref; j++)
		{
			int x = current_shape_ref.at<float>(j);
			int y = current_shape_ref.at<float>(j + numLandmarks_ref);
			std::stringstream ss;
			ss << j;
			cv::putText(frame_disp, ss.str(), cv::Point(x, y), 0.5, 0.5, cv::Scalar(0, 0, 255));
			cv::circle(frame_disp, cv::Point(x, y), 2, cv::Scalar(0, 0, 255), -1);
		}
		imshow("Press escape key to register your face", frame_disp);
		if (waitKey(30) >= 0)
		{
			if (faces_ref.size() != 1)
			{
				cout << "None or more than one of faces have been recognized" << endl;
			}
			else
			{
				if (numLandmarks_ref == 0)
				{
					cout << "No Landmark... Try again" << endl;
				}

				else
				{
					cout << numLandmarks_ref << endl;
					cout << "Face has been enrolled" << endl;
					destroyWindow("Press escape key to register your face");
					break;
				}
			}
		}
		faces_ref.clear();
	}

	vector<float> his_ref, his_tar;
	Mat face_gray_ref, face_gray_ref_tmp;
	Mat frame_gray_ref;
	int block_size_ref;
	vector <Point> points_landmark_ref;
	cvtColor(frame_ref, frame_gray_ref, COLOR_RGB2GRAY);
	//face_gray_ref = frame_gray_ref(Range(faces_ref[0].y, faces_ref[0].y + faces_ref[0].height), Range(faces_ref[0].x, faces_ref[0].x + faces_ref[0].width));
	points_landmark_ref.clear();

	for (int j = 0, x, y; j < numLandmarks_ref; j++)
	{
		x = current_shape_ref.at<float>(j);
		y = current_shape_ref.at<float>(j + numLandmarks_ref);
		points_landmark_ref.push_back(Point(x, y));
	}
	block_size_ref = (faces_ref[0].width > faces_ref[0].height) ? faces_ref[0].height : faces_ref[0].width; // 더 작은것
	block_size_ref /= blocksize_div;
	compute_uni_LBP_aroundpoints(frame_gray_ref, his_ref, points_landmark_ref, block_size_ref, INDEX_EXCEPT_OUTLINE, points_landmark_ref.size());
	if (!((points_landmark_ref[30].y > faces_ref[0].y) &&
		(points_landmark_ref[30].y < faces_ref[0].y + faces_ref[0].height) &&
		(points_landmark_ref[30].x > faces_ref[0].x) &&
		(points_landmark_ref[30].x < faces_ref[0].x + faces_ref[0].width)))
	{
		cout << "landmark is out of face boundary... try again" << endl;
		return 0;
	}
	cout << "Face enrollment done" << endl;
	cout << "ref 완료   size: " << his_ref.size() << endl;


#if tar_test
	Mat frame_tar;
	Mat current_shape_tar;
	Mat face_gray_tar, face_gray_tar_tmp;
	Mat frame_gray_tar;
	vector <Point> points_landmark_tar;

	int numLandmarks_tar;
	int block_size_tar;
	float loss, threshold;
	threshold = 65;
	while (true)
	{
		capture >> frame_tar;
		cascade.detectMultiScale(frame_tar, faces_tar, 1.1, 4, 0 | CASCADE_SCALE_IMAGE, Size(10, 10));

		if (faces_tar.size() == 1)
		{
			modelt.track(frame_tar, current_shape_tar);

			cvtColor(frame_tar, frame_gray_tar, COLOR_RGB2GRAY);
			//face_gray_tar_tmp = frame_gray_ref(Range(faces_tar[0].y, faces_tar[0].y + faces_tar[0].height), Range(faces_tar[0].x, faces_tar[0].x + faces_tar[0].width));
			//resize(face_gray_tar_tmp, face_gray_tar, Size(face_gray_ref.cols, face_gray_ref.rows));
			points_landmark_tar.clear();
			numLandmarks_tar = current_shape_tar.cols / 2;
			for (int j = 0, x, y; j < numLandmarks_tar; j++)
			{
				x = current_shape_tar.at<float>(j);
				y = current_shape_tar.at<float>(j + numLandmarks_tar);
				points_landmark_tar.push_back(Point(x, y));
				std::stringstream ss;
				ss << j;
				cv::putText(frame_tar, ss.str(), cv::Point(x, y), 0.5, 0.5, cv::Scalar(0, 0, 255));
				cv::circle(frame_tar, cv::Point(x, y), 2, cv::Scalar(0, 0, 255), -1);
			}
			if ((
				(points_landmark_tar[30].y > faces_tar[0].y)&&
				(points_landmark_tar[30].y < faces_tar[0].y + faces_tar[0].height)&&
				(points_landmark_tar[30].x > faces_tar[0].x) &&
				(points_landmark_tar[30].x < faces_tar[0].x + faces_tar[0].width)
				))
			{
				block_size_tar = (faces_tar[0].width > faces_tar[0].height) ? faces_tar[0].height : faces_tar[0].width; // 더 작은것
				block_size_tar /= blocksize_div;
				//compute_LBP(face_gray_tar, his_tar, block_size_tar);
				compute_uni_LBP_aroundpoints(frame_gray_tar, his_tar, points_landmark_tar, block_size_tar, INDEX_EXCEPT_OUTLINE, points_landmark_tar.size());
				cout << "tar 완료   size: " << his_tar.size() << endl;
				loss = compute_his_loss(his_ref, his_tar);
				cout << "loss: " << loss << endl;
				cout << "ref window:"<<block_size_ref<<" tar window:"<<block_size_tar<<endl;
			}
			else
			{
				cout << "landmark is out of face boundary... try again" << endl;
				loss = -1;
			}
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
		imshow("Show your face", frame_tar);
		if (waitKey(50) >= 0) break;
		faces_tar.clear();
	}
#endif

}