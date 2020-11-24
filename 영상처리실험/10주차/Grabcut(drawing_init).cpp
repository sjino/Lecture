#include <stdio.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#pragma warning (disable:4996)
using namespace cv;
using namespace std;

class Grabcut
{
public:
	enum
	{
		IN_PROCESS = 3, RELEASED = 4, SET = 5, NOT_SET = 6
	};
	const int radius = 2;
	const int thickness = -1;
	Grabcut();
	~Grabcut();
	void mouseClick(int event, int x, int y, int flags, void*);
	void setRCMask(int flag, Point p);
	void imageshow();
	void set(string winName, Mat& image);
	void proceed_gcut();
	void state_reset();
	void image_restore();
	Rect makeRectmask(vector<Point>fgp, vector<Point>bgp);
	void setRectInMask(Rect input);
	Mat mask;
	Mat binmask;
	Rect rect;
private:
	vector<Point> fgpxl/*foreground pixels*/, bgpxl /*background pixels*/;
	uchar pressstate;
	uchar gstate;
	uchar bstate;
	string winName;
	Mat* image_to_cut;

	Mat bgdModel, fgdModel;
	int init = 0;
};

Grabcut::Grabcut()
{
	init = 1;
	state_reset();
}
Grabcut::~Grabcut()
{
	state_reset();
}
void Grabcut::set(string winName, Mat& image)
{
	this->winName = winName;
	this->image_to_cut = &image;
	mask.create(this->image_to_cut->size(), CV_8UC1);
	mask.setTo(GC_PR_FGD);
	binmask.create(this->image_to_cut->size(), CV_8UC1);
}
Rect Grabcut::makeRectmask(vector<Point>fgp, vector<Point>bgp)
{
	vector<Point>::iterator it;
	Point lefttop, rightbot;
	int min_x = image_to_cut->cols, min_y = image_to_cut->rows, max_x = 0, max_y = 0;
	for (it = fgp.begin(); it != fgp.end(); it++)
	{
		if (min_x>(*it).x)
			min_x = (*it).x;
		else if (max_x < (*it).x)
			max_x = (*it).x;
		if (min_y>(*it).y)
			min_y = (*it).y;
		else if (max_y<(*it).y)
			max_y = (*it).y;
	}
	for (it = bgp.begin(); it != bgp.end(); it++)
	{
		if (min_x > (*it).x)
			min_x = (*it).x;
		else if (max_x < (*it).x)
			max_x = (*it).x;
		if (min_y > (*it).y)
			min_y = (*it).y;
		else if (max_y < (*it).y)
			max_y = (*it).y;
	}
	lefttop.x = min_x;
	lefttop.y = min_y;
	rightbot.x = max_x;
	rightbot.y = max_y;
	return Rect(lefttop, rightbot);
}
void Grabcut::state_reset()
{
	//if (!mask.empty())
	gstate = NOT_SET;
	bstate = NOT_SET;
	pressstate = RELEASED;
	fgpxl.clear();
	bgpxl.clear();

}
void Grabcut::image_restore()
{
	mask.release();
	binmask.release();
	state_reset();
	mask.create(this->image_to_cut->size(), CV_8UC1);
	mask.setTo(GC_PR_FGD);
	binmask.create(this->image_to_cut->size(), CV_8UC1);
	init = 1;
}
void Grabcut::proceed_gcut()
{
	Mat tmp;
	vector<Point>::iterator it;

	if (gstate == SET && bstate == SET)
	{
		if (init==1)
		{
			setRectInMask(makeRectmask(fgpxl, bgpxl));
		}
		grabCut(*image_to_cut, mask, rect, bgdModel, fgdModel, 5, GC_INIT_WITH_MASK);
		//compare(tmp, GC_FGD, tmp, CMP_EQ);

		compare(mask, GC_PR_FGD, binmask, CMP_EQ);
		for (it = fgpxl.begin(); it != fgpxl.end(); it++) //GC_FGD에 대한 값 복구
		{
			circle(binmask, *it, radius, 255, thickness);
		}
		state_reset();
		init = 0;
	}
}
void Grabcut::setRCMask(int flag, Point p)
{
	uchar bvalue, fvalue;
	bvalue = GC_BGD;
	fvalue = GC_FGD;
	if (flag == EVENT_FLAG_LBUTTON)
	{
		fgpxl.push_back(p);
		circle(mask, p, radius, fvalue, thickness);
	}
	if (flag == EVENT_FLAG_RBUTTON)
	{
		bgpxl.push_back(p);
		circle(mask, p, radius, bvalue, thickness);
	}
}
void Grabcut::setRectInMask(Rect input)
{
	mask.setTo(GC_BGD);
	rect.x = max(0, input.x);
	rect.y = max(0, input.y);
	rect.width = min(input.width, image_to_cut->cols - input.x);
	rect.height = min(input.height, image_to_cut->rows - input.y);
	(mask(rect)).setTo(Scalar(GC_PR_FGD));
}
void Grabcut::mouseClick(int event, int x, int y, int flags, void*)
{
	switch (event)
	{
	case EVENT_LBUTTONDOWN: // set GC_FGD labels
		setRCMask(flags, Point(x, y));
		gstate = SET;
		pressstate = IN_PROCESS;
		break;
	case EVENT_RBUTTONDOWN: // set GC_BGD labels
		setRCMask(flags, Point(x, y));
		bstate = SET;
		pressstate = IN_PROCESS;
		break;
	case EVENT_LBUTTONUP:
		setRCMask(flags, Point(x, y));
		pressstate = RELEASED;
		imageshow();
		break;
	case EVENT_RBUTTONUP:
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
void Grabcut::imageshow()
{
	Mat resultimg;

	if (init == 1)
	{
		image_to_cut->copyTo(resultimg);
	}
	else
	{
		resultimg.create(image_to_cut->size(), CV_8UC3);
		resultimg.setTo(Scalar(255, 255, 255));
		image_to_cut->copyTo(resultimg, binmask);
	}
	vector<Point>::iterator it;


	for (it = bgpxl.begin(); it != bgpxl.end(); it++)
	{
		circle(resultimg, *it, radius, Scalar(255, 0, 0), thickness);
	}
	for (it = fgpxl.begin(); it != fgpxl.end(); it++)
	{
		circle(resultimg, *it, radius, Scalar(0, 0, 255), thickness);
	}
	imshow(winName, resultimg);

}
Grabcut gcut;

static void on_mouse(int event, int x, int y, int flags, void* param)
{
	gcut.mouseClick(event, x, y, flags, param);
}
int main()
{
	char key;
	Mat image = imread("노지선8.jpg");
	const string winName = "Grabcut";
	gcut.set(winName, image);
	namedWindow(winName, WINDOW_AUTOSIZE);
	setMouseCallback(winName, on_mouse, 0);
	gcut.setRectInMask(Rect(375, 50, 600, 870));


	cout << "Restore: 'r'" << endl << "Grabcut: 'g'" << endl << "Exit: 'q'" << endl;
	cout << "왼쪽 마우스:Foreground 지정" << endl << "오른쪽 마우스: Background 지정" << endl;
	//gcut.proceed_gcut(1);
	gcut.imageshow();

	while (true)
	{
		key = (char)waitKey(0);
		if (key == 'q')
		{
			cout << "Exit" << endl;
			break;
		}
		else if (key == 'r')
		{
			gcut.image_restore();
			gcut.imageshow();
			cout << "Back to original image" << endl;
		}
		else if (key == 'g')
		{
			cout << "Go grabcut" << endl;
			gcut.proceed_gcut();
			cout << "Grabcut done" << endl;
			gcut.imageshow();
		}

	}
}
