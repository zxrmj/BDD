#pragma once
#include <opencv.hpp>
#include <iostream>
#include <math.h>
using namespace std;
using namespace cv;



class GrayCoMatrix
{
public:
	GrayCoMatrix();
	~GrayCoMatrix();
	static int gray_distance;
	static int gray_level;
	static Mat createGLCM(Mat srcImage, int16_t angle);
	static Mat calcGLCMFeatureVector(Mat glcm);
	static enum GLCM
	{
		GLCM_ANGLE_HORIZATION = 0,       //水平方向
		GLCM_ANGLE_VERTICAL = 90,        //垂直方向
		GLCM_ANGLE_DIGONAL = 45,         //对角线方向
		GLCM_ANGLE_BACKDIGONAL = 135,    //反对角线方向
	};
};

