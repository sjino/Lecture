#include <stdio.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/ximgproc.hpp>
#pragma warning (disable:4996)

using namespace cv;
using namespace cv::ml;
using namespace std;

int main()
{
	Mat image = imread("�ڸ�.jpg");
	Mat mask;
	cv::Ptr<ximgproc::SuperpixelSLIC >a = cv::ximgproc::createSuperpixelSLIC(image, cv::ximgproc::SLICO,10,0.00001); //region size=S=sqrt(N/K) / ruler = m
	a->iterate(); //superpixel segmentaion ���
	a->getLabelContourMask(mask); //contour�� ���ؼ��� ���, �������� �������� ����ũ�� return�Ѵ�.
	for (int i = 0; i < image.rows; i++)
	{
		for (int j = 0; j < image.cols; j++)
		{
			if (mask.at<uchar>(i, j) == 255) //��� �ȼ� ��ġ�� ���Ͽ� �ȼ��� �ٲ��ش�.
			{
				image.at<Vec3b>(i, j)[0] = 0;
				image.at<Vec3b>(i, j)[1] = 165;
				image.at<Vec3b>(i, j)[2] = 255;

			}
		}
	}

	imshow("SLICO", image);
	waitKey(0);
}