#pragma once
#include <opencv.hpp>
#include <iostream>
#include <math.h>
using namespace std;
using namespace cv;

/*
�Ҷȹ��������ʵ��
�����Ǿ�̬��
*/

class GrayCoMatrix
{
public:
	GrayCoMatrix();
	~GrayCoMatrix();
	static int gray_distance; // �ҶȾ���
	static int gray_level; // �Ҷȼ� һ��Ϊ16
	static Mat createGLCM(Mat srcImage, int16_t angle); // �����Ҷȼ���������
	static Mat calcGLCMFeatureVector(Mat glcm); // �ӻҶȼ�����������ȡ��������
	static enum GLCM
	{
		GLCM_ANGLE_HORIZATION = 0,       //ˮƽ����
		GLCM_ANGLE_VERTICAL = 90,        //��ֱ����
		GLCM_ANGLE_DIGONAL = 45,         //�Խ��߷���
		GLCM_ANGLE_BACKDIGONAL = 135,    //���Խ��߷��� ���÷�����δ֧�֣�
	};
};

