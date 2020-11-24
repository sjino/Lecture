#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/photo.hpp>
#pragma warning (disable:4996)

using namespace cv;
using namespace std;


class RC_mask
{
public:
	enum
	{
		IN_PROCESS = 3, RELEASED = 4, SET = 5, NOT_SET = 6
	};
	const int radius = 6;
	const int thickness = -1;
	const uchar marker = 255;
	RC_mask();
	~RC_mask();
	void mouseClick(int event, int x, int y, int flags, void*);
	void setRCMask(int flag, Point p);
	void imageshow();
	void set(string winName, Mat& image, Mat& mask);
	void reset();
	Mat* mask;
	Rect rect;
private:
	vector<Point> rc_pxl/*to recover pixels*/;
	uchar pressstate;
	uchar marking_state;
	string winName;
	Mat* image_damaged;


	Mat bgdModel, fgdModel;
	int init = 0;
};

RC_mask::RC_mask()
{
	reset();
}
void RC_mask::set(string winName, Mat& input_image, Mat& mask)
{
	this->winName = winName;
	this->image_damaged = &input_image;
	this->mask = &mask;
	this->mask->create(this->image_damaged->size(), CV_8UC1);
	this->mask->setTo(0);
}
RC_mask::~RC_mask()
{
	reset();
}
void RC_mask::reset()
{
	//if (!mask.empty())
	//mask->setTo(Scalar(0));
	marking_state = NOT_SET;
	pressstate = RELEASED;
	rc_pxl.clear();
}
void RC_mask::setRCMask(int flag, Point p)
{
	if (flag == EVENT_FLAG_LBUTTON)
	{
		rc_pxl.push_back(p);
		circle(*mask, p, radius, marker, thickness);
	}
}
void RC_mask::mouseClick(int event, int x, int y, int flags, void*)
{
	switch (event)
	{
	case EVENT_LBUTTONDOWN: // set GC_FGD labels
		setRCMask(flags, Point(x, y));
		marking_state = SET;
		pressstate = IN_PROCESS;
		break;
	case EVENT_LBUTTONUP:
		setRCMask(flags, Point(x, y));
		pressstate = RELEASED;
		imageshow();
		break;
	case EVENT_MOUSEMOVE:
		if (pressstate == IN_PROCESS)
		{
			setRCMask(flags, Point(x, y));
			imageshow();
		}
		break;
	}
}
void RC_mask::imageshow()
{
	Mat resultimg;
	image_damaged->copyTo(resultimg);

	vector<Point>::iterator it;
	for (it = rc_pxl.begin(); it != rc_pxl.end(); it++)
	{
		circle(resultimg, *it, radius, Scalar(0, 255, 191), thickness);
	}
	imshow(winName, resultimg);
}

RC_mask rcmask;

static void on_mouse(int event, int x, int y, int flags, void* param)
{
	rcmask.mouseClick(event, x, y, flags, param);
}

int main()
{
	char key;
	Mat image_damaged = imread("ÀÚ¸·3.jpg");
	Mat mask_to_recover;//(image_damaged.rows, image_damaged.cols, CV_8UC1);
	Mat image_recovered;
	const string winName = "Damaged image";
	namedWindow(winName, WINDOW_AUTOSIZE);
	setMouseCallback(winName, on_mouse, 0);
	rcmask.set(winName, image_damaged, mask_to_recover);

	rcmask.imageshow();
	cout << "Restore: 'r'" << endl << "Exit: 'q'" << endl;
	while (true)
	{
		key = (char)waitKey(0);
		if (key == 'q')
		{
			cout << "Exit" << endl;
			return 0;
		}
		else if (key == 'r')
		{
			cout << "recovery done... Exit" << endl;
			break;
		}

	}

	inpaint(image_damaged, mask_to_recover, image_recovered, 3, INPAINT_TELEA);


	imshow("Recovered image", image_recovered);
	waitKey(0);
}