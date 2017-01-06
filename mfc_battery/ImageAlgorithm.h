#pragma once
#include <opencv.hpp>
using namespace cv;
namespace feature 
{
	Mat extractColorFeature(Mat src);
	Mat extractNakedFeature(Mat src);
	Mat extractRustyFeature(Mat src);
	Mat extractUpSidePitFeature(Mat src);
} // FEATURE ALGOTHERM

namespace region 
{
	void detectColourBattery(Mat &src, Mat &dst);
	void detectNakedBattery(Mat &src,Mat &dst);
	void detectFloorSideBattery(Mat &src, Mat &dst);
	void detectLowSideBattery(Mat &src, Mat &dst);
} // REGION OF INTERSTING EXTRACTION
