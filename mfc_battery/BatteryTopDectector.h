#pragma once
#include "ObjectDetector.h"
class BatteryTopDectector :
	public ObjectDetector
{
public:
	BatteryTopDectector();
	void Detect(Mat &src, Mat &dst);
	~BatteryTopDectector();
};

