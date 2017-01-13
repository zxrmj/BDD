#include "stdafx.h"
#include "ObjectDetector.h"



ObjectDetector::ObjectDetector()
{
	use_gpu = false;
}

void ObjectDetector::InitDetector(bool useGpu)
{
	if (useGpu)
	{
		ballard = cuda::createGeneralizedHoughBallard();
	}
	else
	{
		ballard = cuda::createGeneralizedHoughBallard();
	}
	use_gpu = useGpu;
}

void ObjectDetector::SetMinDist(double minDist)
{
	ballard->setMinDist(minDist);
}

void ObjectDetector::SetDp(double dp)
{
	ballard->setDp(dp);
}

void ObjectDetector::SetMaxBufferSize(int maxBufferSize)
{
	ballard->setMaxBufferSize(maxBufferSize);
}

void ObjectDetector::SetLevels(int levels)
{
	ballard->setLevels(levels);
}

void ObjectDetector::SetVotesThreshold(int votesThreshold)
{
	ballard->setVotesThreshold(votesThreshold);
}

void ObjectDetector::SetTemplate(Mat temp)
{
	
	if (use_gpu)
	{
		cuda::GpuMat g_temp(temp);
		ballard->setTemplate(g_temp);
	}
	else
	{
		ballard->setTemplate(temp);
	}
	template_image = temp;
}

void ObjectDetector::SetCannyLowThreshold(int val)
{
	this->ballard->setCannyLowThresh(val);
}

void ObjectDetector::SetCannyHighThreshold(int val)
{
	this->ballard->setCannyHighThresh(val);
}

vector<RotatedRect> ObjectDetector::Detect(Mat src)
{
	TickMeter tm;
	vector<Vec4f> position;
	if (use_gpu)
	{
		cuda::GpuMat d_image(src);
		cuda::GpuMat d_position;
		tm.start();
		ballard->detect(d_image, d_position);
		tm.stop();
		if (d_position.data)
		{
			d_position.download(position);
		}
	}
	else
	{
		tm.start();
		ballard->detect(src, position);
		tm.stop();
	}
	cout << "Found : " << position.size() << " objects" << endl;
	cout << "Detection time : " << tm.getTimeMilli() << " ms" << endl;
	vector<RotatedRect> v_rect;
	for (int i = 0; i< position.size(); i++)
	{
		Point2f pos(position[i][0], position[i][1]);
		float scale = position[i][2];
		float angle = position[i][3];
		RotatedRect rect;
		rect.center = pos;
		rect.size = Size2f(template_image.cols * scale, template_image.rows * scale);
		rect.angle = angle;
		v_rect.push_back(rect);
	}
	return v_rect;
}

ObjectDetector::~ObjectDetector()
{
	ballard.release();
}
