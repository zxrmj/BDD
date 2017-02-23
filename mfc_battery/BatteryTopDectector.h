#pragma once
#include "ObjectDetector.h"
class BatteryTopDetector :
	public ObjectDetector
{
public:
	BatteryTopDetector();
	void Detect(Mat &src, Mat &dst);
	~BatteryTopDetector();
};

