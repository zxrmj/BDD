#pragma once
#include "ObjectDetector.h"

class BatteryDetector : public ObjectDetector
{
public:
	BatteryDetector();
	BatteryDetector(Mat battery_template);
	vector<RotatedRect> Detect(Mat src);
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
