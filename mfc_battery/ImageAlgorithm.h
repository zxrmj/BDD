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

namespace joint
{
	enum JOINT_POS
	{
		JOINT_UL = 0,// вСио╫г
		JOINT_UR = 1,// срио╫г
		JOINT_LL = 2,// вСоб╫г
		JOINT_LR = 3,// сроб╫г
	};
	void createJointMappingRelation(Mat &map_x, Mat &map_y, int _width, int _height, int pos);
}
