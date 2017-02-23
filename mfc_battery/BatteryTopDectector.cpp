#include "stdafx.h"
#include "BatteryTopDectector.h"



BatteryTopDetector::BatteryTopDetector()
	:ObjectDetector()
{
	this->InitDetector(true);
	this->SetDp(3);
	this->SetLevels(360);
	this->SetMaxBufferSize(1000);
	this->SetMinDist(400);
	this->SetVotesThreshold(60);
}


void BatteryTopDetector::Detect(Mat & src, Mat & dst)
{
	vector<RotatedRect> v_rect;
	if (src.channels() == 3)
	{
		Mat gray;
		cvtColor(src, gray, CV_BGR2GRAY);
		v_rect = ObjectDetector::Detect(gray);
	}
	else
	{
		v_rect = ObjectDetector::Detect(src);
	}
	if (v_rect.size() == 1)
	{
		RotatedRect rr = v_rect[0];
		rr.size.height *= 1.2;
		rr.size.width *= 1.2;
		Mat roi = src(rr.boundingRect());
		Mat binary;
		threshold(roi, binary, 128, 255, THRESH_BINARY | THRESH_OTSU);
		Rect rect;
		vector<Point> points;
		for (int i = 0; i < binary.rows; i++)
		{
			for (int j = 0; j < binary.cols; j++)
			{
				if (binary.at<uchar>(i, j) == 255)
				{
					points.push_back(Point(j, i));
				}
			}
		}
		rect = boundingRect(points);
		rect.height = rect.width;
		if (rect.height + rect.y > roi.rows)
		{
			rect.height = roi.rows - rect.y;
		}
		if (rect.width + rect.x > roi.cols)
		{
			rect.width = roi.cols - rect.x;
		}
		dst = roi(rect);
	}
}


BatteryTopDetector::~BatteryTopDetector()
{
}
