#include <opencv2/features2d.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include "utils.h"
#define _CRT_SECURE_NO_WARNINGS

using namespace cv;
using namespace std;

int ORB_MAX_KPTS = 1500;
float ORB_SCALE_FACTOR = 1.2;
int ORB_PYRAMID_LEVELS = 4;
float ORB_EDGE_THRESHOLD = 31.0;
int ORB_FIRST_PYRAMID_LEVEL = 0;
int ORB_WTA_K = 2;
int ORB_PATCH_SIZE = 31;

float MIN_H_ERROR = 2.50f;
float DRATIO = 0.80f;


void main()
{
	VideoCapture capture(0);

	Mat img1_input, img1, img1_32, img2_input, img2, img2_32;
	string img_path1, img_path2, homography_path;
	double t1 = 0.0, t2 = 0.0;
	vector<KeyPoint> kpts1_orb, kpts2_orb;
	vector<Point2f> matches_orb, inliers_orb;
	vector<vector<DMatch>> dmatches_orb;
	Mat desc1_orb, desc2_orb;
	int nmatches_orb = 0, ninliers_orb = 0, noutliers_orb = 0;
	int nkpts1_orb = 0, nkpts2_orb = 0;
	float ratio_orb = 0.0;
	double torb = 0.0; // Create the L2 and L1 matchers
	Ptr<DescriptorMatcher> matcher_l2 = DescriptorMatcher::create("BruteForce");
	Ptr<DescriptorMatcher> matcher_l1 = DescriptorMatcher::create("BruteForce-Hamming");

	if (!capture.isOpened())
	{
		printf("Couldn’t open the web camera…\n");
		return;
	}
	while (true)
	{
		capture >> img1_input;
		imshow("캡쳐를 위해 esc를 눌러 주세요", img1_input);
		if (waitKey(30) >= 0)
		{
			cvtColor(img1_input, img1, COLOR_RGB2GRAY);
			break;
		}
	}
	// Convert the images to float
	img1.convertTo(img1_32, CV_32F, 1.0 / 255.0, 0);
	// Color images for results visualization
	Mat img1_rgb_orb = Mat(Size(img1.cols, img1.rows), CV_8UC3);
	Mat img2_rgb_orb = Mat(Size(img2.cols, img1.rows), CV_8UC3);
	Mat img_com_orb = Mat(Size(img1.cols * 2, img1.rows), CV_8UC3);
	// ORB Features
	Ptr<ORB> orb = ORB::create(ORB_MAX_KPTS, ORB_SCALE_FACTOR, ORB_PYRAMID_LEVELS,
		ORB_EDGE_THRESHOLD, ORB_FIRST_PYRAMID_LEVEL, ORB_WTA_K, ORB::HARRIS_SCORE,
		ORB_PATCH_SIZE);
	orb->detectAndCompute(img1, noArray(), kpts1_orb, desc1_orb, false);

	while (true)
	{
		capture >> img2_input;
		cvtColor(img2_input, img2, COLOR_RGB2GRAY);
		// Convert the images to float
		img2.convertTo(img2_32, CV_32F, 1.0 / 255.0, 0);


		t1 = getTickCount();
		orb->detectAndCompute(img2, noArray(), kpts2_orb, desc2_orb, false);
		matcher_l1->knnMatch(desc1_orb, desc2_orb, dmatches_orb, 2);
		matches2points_nndr(kpts1_orb, kpts2_orb, dmatches_orb, matches_orb, DRATIO);
		compute_inliers_ransac(matches_orb, inliers_orb, MIN_H_ERROR, false);
		nkpts1_orb = kpts1_orb.size();
		nkpts2_orb = kpts2_orb.size();
		nmatches_orb = matches_orb.size() / 2;
		ninliers_orb = inliers_orb.size() / 2;
		noutliers_orb = nmatches_orb - ninliers_orb;
		ratio_orb = 100.0 * (float)(ninliers_orb) / (float)(nmatches_orb);
		t2 = cv::getTickCount();
		torb = 1000.0 * (t2 - t1) / cv::getTickFrequency();
		//cvtColor(img1, img1_rgb_orb, cv::COLOR_GRAY2BGR);
		//cvtColor(img2, img2_rgb_orb, cv::COLOR_GRAY2BGR);
		draw_keypoints(img1_input, kpts1_orb);
		draw_keypoints(img2_input, kpts2_orb);
		draw_inliers(img1_input, img2_input, img_com_orb, inliers_orb, 0);
		imshow("ORB", img_com_orb);
		kpts2_orb.clear();
		dmatches_orb.clear();
		matches_orb.clear();
		inliers_orb.clear();
		//waitKey(1);
		system("cls");
		cout << "ORB Results" << endl;
		cout << "**************************************" << endl;
		cout << "Number of Keypoints Image 1: " << nkpts1_orb << endl;
		cout << "Number of Keypoints Image 2: " << nkpts2_orb << endl;
		cout << "Number of Matches: " << nmatches_orb << endl;
		cout << "Number of Inliers: " << ninliers_orb << endl;
		cout << "Number of Outliers: " << noutliers_orb << endl;
		cout << "Inliers Ratio: " << ratio_orb << endl;
		cout << "ORB Features Extraction Time (ms): " << torb << endl; cout << endl;
		if (waitKey(5) >= 0)
		{
			break;
		}
	}

}