#pragma once
#include <opencv.hpp>
#include <iostream>
#include <math.h>
using namespace std;
using namespace cv;

/*
灰度共生矩阵的实现
该类是静态类
*/

class GrayCoMatrix
{
public:
	GrayCoMatrix();
	~GrayCoMatrix();
	static int gray_distance; // 灰度距离
	static int gray_level; // 灰度级 一般为16
	static Mat createGLCM(Mat srcImage, int16_t angle); // 创建灰度级共生矩阵
	static Mat calcGLCMFeatureVector(Mat glcm); // 从灰度级共生矩阵提取特征向量
	static enum GLCM
	{
		GLCM_ANGLE_HORIZATION = 0,       //水平方向
		GLCM_ANGLE_VERTICAL = 90,        //垂直方向
		GLCM_ANGLE_DIGONAL = 45,         //对角线方向
		GLCM_ANGLE_BACKDIGONAL = 135,    //反对角线方向 （该方向暂未支持）
	};
};

