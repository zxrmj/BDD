#include "stdafx.h"
#include "BatteryDetector.h"

BatteryDetector::BatteryDetector()
{
	set_default_params();
}

BatteryDetector::BatteryDetector(Mat battery_template) : BatteryDetector()
{
	this->SetTemplate(battery_template);
}

vector<RotatedRect> BatteryDetector::Detect(Mat src)
{
	if (cur_battery >= battery_capture_number)
	{
		cur_battery = 0;
	}
	src = src(v_regions[cur_battery]);
	auto v_rect = ObjectDetector::Detect(src);
	for (int i = 0; i < v_rect.size(); i++)
	{
		v_rect[i].center += v_offset[cur_battery];
		Point2f points[4];
		v_rect[i].points(points);
		for (int j = 0; j < 4; j++)
		{
			if (!points[j].inside(v_regions[cur_battery]))
			{
				v_rect.erase(v_rect.begin() + i);
				i--;
				break;
			}
		}
	}
	cur_battery++;
	return v_rect;
}

BatteryDetector::~BatteryDetector()
{

}

void BatteryDetector::SetBatteryNumber(int num)
{
	this->battery_capture_number = num;
	return;
}

void BatteryDetector::SetPossibleRegion(vector<Rect> regions)
{
	this->v_regions = regions;
	v_offset.clear();
	for (int i = 0; i < v_regions.size(); i++)
	{
		v_offset.push_back(Point2f(v_regions[i].x, v_regions[i].y));
	}
}

void BatteryDetector::Reset()
{
	cur_battery = 0;
}

void BatteryDetector::set_default_params()
{
	this->InitDetector(true);
	this->SetDp(3);
	this->SetLevels(360);
	this->SetMaxBufferSize(1000);
	this->SetMinDist(400);
	this->SetVotesThreshold(60);
	this->battery_capture_number = 1;
	this->cur_battery = 0;
}
