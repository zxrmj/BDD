#pragma once
#include <opencv.hpp>
using namespace cv;
using namespace std;

class ObjectDetector
{
public:
	ObjectDetector();
	void InitDetector(bool useGpu = true);
	void SetMinDist(double minDist);
	void SetDp(double dp);
	void SetMaxBufferSize(int maxBufferSize);
	void SetLevels(int levels);
	void SetVotesThreshold(int votesThreshold);
	void SetTemplate(Mat temp);
	void SetCannyLowThreshold(int val);
	void SetCannyHighThreshold(int val);
	vector<RotatedRect> Detect(Mat src);
	~ObjectDetector();
private:
	Ptr<GeneralizedHoughBallard> ballard;
	Mat template_image;
	bool use_gpu;
};

