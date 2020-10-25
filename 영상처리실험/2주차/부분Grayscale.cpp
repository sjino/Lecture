#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include<opencv2/core/core.hpp>
#include<opencv2/imgcodecs/imgcodecs.hpp>
#define BLK 200

using namespace cv;
void main()
{
	// You can set the path of an image file
	int height, width, cy, cx;
	int r, g, b;
	Mat imgColor = imread("æ∆¿Ã¿Ø.jpg", IMREAD_COLOR);
	Mat result = imgColor.clone();
	width = imgColor.cols;
	height = imgColor.rows;
	cy = height / 2;
	cx = width / 2;
	for (int i = cy - BLK; i < cy + BLK; i++)
	{
		for (int j = cx - BLK; j < cx + BLK; j++)
		{

			b = result.at<Vec3b>(i, j)[0];
			g = result.at<Vec3b>(i, j)[1];
			r = result.at<Vec3b>(i, j)[2];
			result.at<Vec3b>(i, j)[0] = result.at<Vec3b>(i, j)[1] = result.at<Vec3b>(i, j)[2] = (r + g + b) / 3;
		}
	}
	imshow("color", imgColor);
	imshow("result", result);
	waitKey(5000);
}