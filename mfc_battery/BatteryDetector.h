#pragma once
#include "ObjectDetector.h"
/*
���ڶ�λ������õĵ��
�ù�����ʹ��ImageAlgorithm.h�е���������������
void detectColourBattery(Mat &src, Mat &dst);
void detectNakedBattery(Mat &src,Mat &dst);
*/
class BatteryDetector : public ObjectDetector
{
public:
	BatteryDetector();
	BatteryDetector(Mat battery_template); // ʹ��һ��ģ��ͼƬ��ʼ����
	vector<RotatedRect> Detect(Mat src); // �������ͼ����ģ�������λ��
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
