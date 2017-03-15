#pragma once
#include <opencv.hpp>
#include "BatteryTopDectector.h"
using namespace cv;
// 与电池定位与特征提取有关的图像处理函数
namespace feature 
{
	Mat extractColorFeature(Mat src); // 提取包膜电池的侧表面特征向量
	Mat extractNakedFeature(Mat src); // 提取未包膜电池的侧表面特征向量
	Mat extractNakedLocalFeature(Mat src); // 提取未包膜电池的侧表面局部特征向量
	Mat extractRustyFeature(Mat src); // 提取电池顶面锈痕特征向量
	Mat extractUpSidePitFeature(Mat src); // 提取电池顶面凹坑特征向量
} // FEATURE ALGOTHERM

namespace region 
{
	void detectColourBattery(Mat &src, Mat &dst);
	void detectNakedBattery(Mat &src,Mat &dst,Point &ru);
	void detectTopSideBattery(Mat & src, Mat & dst);
	//void detectFloorSideBattery(Mat &src, Mat &dst);
	void detectLowSideBattery(Mat &src, Mat &dst);
} // REGION OF INTERSTING EXTRACTION

namespace joint
{
	enum JOINT_POS
	{
		JOINT_UL = 0,// 左上角
		JOINT_UR = 1,// 右上角
		JOINT_LL = 2,// 左下角
		JOINT_LR = 3,// 右下角
	};
	void createJointMappingRelation(Mat &map_x, Mat &map_y, int _width, int _height, int pos);
}
