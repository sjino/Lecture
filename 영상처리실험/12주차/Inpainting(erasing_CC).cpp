#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/photo.hpp>
#include <opencv2/ximgproc.hpp>
#pragma warning (disable:4996)

using namespace cv;
using namespace std;
const int thresh = 35; //이미지에 따라 이거 반드시 조절할것!!!
const uchar marker = 255;
const int FPR_radius = 5, F_radius = 3;
const int thickness = -1;

void CC_Detector(Mat input_img, Mat& output_img)
{
	Mat image_gray;
	cvtColor(input_img, image_gray, COLOR_BGR2GRAY);
	Mat resultimg = Mat::zeros(image_gray.size(), CV_32FC1);

	cornerHarris(image_gray, resultimg, 2, 3, 0.04);
	Mat dst_norm, dst_norm_scaled;
	normalize(resultimg, dst_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat());
	convertScaleAbs(dst_norm, output_img);
	output_img.convertTo(output_img, CV_8UC1);

}
void make_cc_mask(Mat& original_image, const Mat& cc_image, Mat& cc_mask)
{
	Mat corner_mask, grabmask;
	Mat bgdModel, fgdModel;
	Rect rect;
	grabmask.create(cc_image.rows, cc_image.cols, CV_8UC1);
	grabmask.setTo(GC_PR_FGD); //Set mask all black
	corner_mask.create(cc_image.rows, cc_image.cols, CV_8UC1);
	corner_mask.setTo(GC_BGD); //Set mask all black
	for (int i = 0; i < cc_image.rows; i++)
	{
		for (int j = 0; j < cc_image.cols; j++)
		{
			if (cc_image.at<uchar>(i, j) < thresh)
			{
				circle(corner_mask, Point(j, i), FPR_radius, GC_PR_FGD, thickness);
			}
		}
	}
	for (int i = 0; i < cc_image.rows; i++)
	{
		for (int j = 0; j < cc_image.cols; j++)
		{
			if (cc_image.at<uchar>(i, j) < thresh)
			{
				corner_mask.at<uchar>(i, j) = GC_FGD;
				//circle(cc_mask, Point(j, i), F_radius, GC_FGD, -1);
			}
		}
	}
	//circle(corner_mask, Point(corner_mask.cols/2, corner_mask.rows/2), 5, GC_BGD, thickness);
	//line(corner_mask, Point(0, corner_mask.rows / 2), Point(corner_mask.cols, corner_mask.rows / 2), GC_BGD, 3);
	grabCut(original_image, corner_mask, rect, bgdModel, fgdModel, 10, GC_INIT_WITH_MASK);
	compare(corner_mask, GC_PR_FGD, cc_mask, CMP_EQ);
	//imshow("임시", cc_mask);
	for (int i = 0; i < cc_image.rows; i++)
	{
		for (int j = 0; j < cc_image.cols; j++)
		{
			if (cc_image.at<uchar>(i, j) < thresh)
			{
				//cc_mask.at<uchar>(i, j) = 255;
				circle(cc_mask, Point(j, i), F_radius, 255,2);
			}
		}
	}

}
int main()
{
	Mat image = imread("자막2.jpg");
	Mat output;
	Mat cc_image;
	Mat cc_mask;

	CC_Detector(image, cc_image);

	make_cc_mask(image, cc_image, cc_mask);

	//imshow("임시", cc_mask); 
	inpaint(image, cc_mask, output, 3, INPAINT_TELEA);
	imshow("원본", image);
	imshow("자막 지운 후", output);
	waitKey(0);
}