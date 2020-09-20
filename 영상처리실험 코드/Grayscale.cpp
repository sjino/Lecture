#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include<opencv2/core/core.hpp>
#include<opencv2/imgcodecs/imgcodecs.hpp>

using namespace cv;
void main()
{
	// You can set the path of an image file
	Mat imgColor = imread("아이유.jpg", IMREAD_COLOR);
	Mat imgGray = imread("아이유.jpg", IMREAD_GRAYSCALE);
	imshow("color", imgColor);
	imshow("gray", imgGray);
	waitKey(5000);
}