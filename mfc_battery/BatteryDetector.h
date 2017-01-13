#pragma once
#include "ObjectDetector.h"

class BatteryDetector : public ObjectDetector
{
public:
	BatteryDetector();
	BatteryDetector(Mat battery_template);
	vector<RotatedRect> Detect(Mat src);
	~BatteryDetector();
	void SetBatteryNumber(int num); // ����һ�����Ҫ�����ͼ����Ŀ
	void SetPossibleRegion(vector<Rect> regions); // ���õ�ؿ��ܳ��ֵ�����
	void Reset();
private:
	void set_default_params();
	int battery_capture_number;
	int cur_battery;
	vector<Point2f> v_offset;
	vector<Rect> v_regions;
};
