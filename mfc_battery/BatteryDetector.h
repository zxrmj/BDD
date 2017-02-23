#pragma once
#include "ObjectDetector.h"
/*
用于定位侧面放置的电池
该功能已使用ImageAlgorithm.h中的以下两函数代替
void detectColourBattery(Mat &src, Mat &dst);
void detectNakedBattery(Mat &src,Mat &dst);
*/
class BatteryDetector : public ObjectDetector
{
public:
	BatteryDetector();
	BatteryDetector(Mat battery_template); // 使用一个模板图片初始化类
	vector<RotatedRect> Detect(Mat src); // 检测输入图像中模板物体的位置
	~BatteryDetector();
	void SetBatteryNumber(int num); // 设置一个电池要捕获的图像数目
	void SetPossibleRegion(vector<Rect> regions); // 设置电池可能出现的区域
	void Reset();
private:
	void set_default_params();
	int battery_capture_number;
	int cur_battery;
	vector<Point2f> v_offset;
	vector<Rect> v_regions;
};
