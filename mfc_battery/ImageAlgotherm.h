#pragma once
#include <opencv.hpp>
using namespace cv;
namespace feature 
{
	Mat extractColorFeature(Mat src);
} // FEATURE ALGOTHERM

namespace region 
{
	int t1 = 60;
	int t2 = 100;
	int in = 5;
	void detectColourBattery(Mat &src, Mat &dst);
} // REGION OF INTERSTING EXTRACTION
