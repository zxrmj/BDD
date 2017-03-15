#include "stdafx.h"
#include "cv_glcm.h"

int GrayCoMatrix::gray_distance = 1;
int GrayCoMatrix::gray_level = 16;
GrayCoMatrix::GrayCoMatrix()
{
}

GrayCoMatrix::~GrayCoMatrix()
{
}

Mat GrayCoMatrix::createGLCM(Mat srcImage, int16_t angle)
{
	srcImage = srcImage.clone();
	if (!srcImage.data)
		return Mat();
	if ((angle % 45) || angle > 135)
		return Mat();
	if (srcImage.channels() == 3)
		cvtColor(srcImage, srcImage, CV_BGR2GRAY);
	uchar lut[256] = { 0 };
	for (int i = 0; i < 256; i++)
	{
		lut[i] = i* gray_level / 256;
	}
	for (int i = 0; i < srcImage.rows; i++)
	{
		for (int j = 0; j < srcImage.cols; j++)
		{
			uchar ch = srcImage.at<uchar>(i, j);
			srcImage.at<uchar>(i, j) = lut[ch];
		}
	}

	Mat glcm(gray_level, gray_level, CV_32FC1);
	fill(glcm.begin<float>(), glcm.end<float>(), (float)0);
	
	//生成共生矩阵
	//水平方向
	if (angle == GLCM_ANGLE_HORIZATION)
	{
		// 旧版本代码
		/*for (size_t i = 0; i < srcImage.rows; i++)
		{
			for (size_t j = 0; j < srcImage.cols; j++)
			{
				if ((i + gray_distance) >= 0 && (i + gray_distance) < srcImage.rows)
				{
					glcm.at<float>(srcImage.at<uint_least8_t>(i, j), srcImage.at<uint_least8_t>(i + gray_distance, j)) += 1;
				}
				if ((i - gray_distance) >= 0 && (i - gray_distance) < srcImage.rows)
				{
					glcm.at<float>(srcImage.at<uint_least8_t>(i, j), srcImage.at<uint_least8_t>(i - gray_distance, j)) += 1;
				}
			}
		}*/
		// 新版本代码：
		uchar upt1 = 0, upt2 = 0;
		for (size_t i = 0; i < srcImage.rows; i++)
		{
			upt1 = srcImage.at<uchar>(i, 0);
			upt2 = 0;
			for (size_t j = 0; j < srcImage.cols - gray_distance; j++)
			{
				upt2 = srcImage.at<uchar>(i, j + gray_distance);
				//cout << "Up1," << (int)upt1 << "Up2," << (int)upt2 << endl;
				//system("pause");
				glcm.at<float>(upt1, upt2) += 1;
				upt1 = upt2;
			}
		}
		float *fpt1, *fpt2;
		float f_sum = 0;
		for (size_t i = 0; i < glcm.rows; i++)
		{
			for (size_t j = 0; j <= i; j++)
			{
				fpt1 = &(glcm.at<float>(i, j));
				fpt2 = &(glcm.at<float>(j, i));
				f_sum = *fpt1 + *fpt2;
				*fpt1 = f_sum;
				*fpt2 = f_sum;
			}
		}
	/*	cout << glcm << endl;
		system("pause");*/
		//计算P(i,j) = 1/R * η(i,j)
		float R = 2 * (srcImage.rows * (srcImage.cols - gray_distance));
		for (size_t i = 0; i < glcm.rows; i++)
		{
			for (size_t j = 0; j < glcm.cols; j++)
			{
				glcm.at<float>(i, j) /= R;
			}
		}
	}
	//垂直方向
	else if (angle == GLCM_ANGLE_VERTICAL)
	{
		//计算η(i,j)
		// 旧版本代码
		/*for (size_t i = 0; i < srcImage.rows; i++)
		{
			for (size_t j = 0; j < srcImage.cols; j++)
			{
				if ((j + gray_distance) >= 0 && (j + gray_distance) < srcImage.cols)
				{
					glcm.at<float>(srcImage.at<uint_least8_t>(i, j), srcImage.at<uint_least8_t>(i, j + gray_distance)) += 1;
				}
				if ((j - gray_distance) >= 0 && (j - gray_distance) < srcImage.cols)
				{
					glcm.at<float>(srcImage.at<uint_least8_t>(i, j), srcImage.at<uint_least8_t>(i, j - gray_distance)) += 1;
				}
			}
		}*/
		// 新版本代码
		uchar upt1 = 0, upt2 = 0;
		for (size_t j = 0; j < srcImage.cols; j++)
		{
			upt1 = srcImage.at<uchar>(0, j);
			for (size_t i = 0; i < srcImage.rows - gray_distance; i++)
			{
				upt2 = srcImage.at<uchar>(i + gray_distance, j);
				glcm.at<float>(upt1, upt2) += 1;
				upt1 = upt2;
			}
		}
		float *fpt1, *fpt2;
		float f_sum = 0;
		for (size_t i = 0; i < glcm.rows; i++)
		{
			for (size_t j = 0; j <= i; j++)
			{
				fpt1 = &(glcm.at<float>(i, j));
				fpt2 = &(glcm.at<float>(j, i));
				f_sum = *fpt1 + *fpt2;
				*fpt1 = f_sum;
				*fpt2 = f_sum;
			}
		}
		//计算P(i,j) = 1/R * η(i,j)
		float R = 2 * (srcImage.rows - gray_distance) * srcImage.cols;
		for (size_t i = 0; i < glcm.rows; i++)
		{
			for (size_t j = 0; j < glcm.cols; j++)
			{
				glcm.at<float>(i, j) /= R;
			}
		}
	}
	//斜对角线方向
	else if (angle == GLCM_ANGLE_DIGONAL)
	{
		/*for (size_t i = 0; i < srcImage.rows; i++)
		{
			for (size_t j = 0; j < srcImage.cols; j++)
			{
				if ((i + gray_distance) >= 0 && (i + gray_distance) < srcImage.rows && (j + gray_distance) >= 0 && (j + gray_distance) < srcImage.cols)
				{
					glcm.at<float>(srcImage.at<uint_least8_t>(i, j), srcImage.at<uint_least8_t>(i + gray_distance, j + gray_distance)) += 1;
				}
				if ((i - gray_distance) >= 0 && (i - gray_distance) < srcImage.rows && (j - gray_distance) >= 0 && (j - gray_distance) < srcImage.cols)
				{
					glcm.at<float>(srcImage.at<uint_least8_t>(i, j), srcImage.at<uint_least8_t>(i - gray_distance, j - gray_distance)) += 1;
				}
			}
		}*/
		uchar upt1, upt2;
		for (int i = gray_distance; i < srcImage.cols; i++)
		{
			int _x = i;
			int _y = 0;
			upt1 = srcImage.at<uchar>(_y, _x);
			while (_x >= gray_distance && _y < srcImage.rows - gray_distance)
			{
				_x--;
				_y++; 
				upt2 = srcImage.at<uchar>(_y, _x);
				glcm.at<float>(upt1, upt2) += 1;
				upt1 = upt2;
			}
		}
		for (int i = gray_distance; i < srcImage.rows - gray_distance; i++)
		{
			int _x = srcImage.cols - 1;
			int _y = i;
			upt1 = srcImage.at<uchar>(_y, _x);
			while (_x >= gray_distance && _y < srcImage.rows - gray_distance)
			{
				_x--;
				_y++;
				upt2 = srcImage.at<uchar>(_y, _x);
				glcm.at<float>(upt1, upt2) += 1;
				upt1 = upt2;

			}
		}
		float *fpt1, *fpt2;
		float f_sum = 0;
		for (size_t i = 0; i < glcm.rows; i++)
		{
			for (size_t j = 0; j <= i; j++)
			{
				fpt1 = &(glcm.at<float>(i, j));
				fpt2 = &(glcm.at<float>(j, i));
				f_sum = *fpt1 + *fpt2;
				*fpt1 = f_sum;
				*fpt2 = f_sum;
			}
		}
		//计算P(i,j) = 1/R * η(i,j)
		float R = 2 * (srcImage.rows - gray_distance) * (srcImage.cols - gray_distance);
		for (size_t i = 0; i < glcm.rows; i++)
		{
			for (size_t j = 0; j < glcm.cols; j++)
			{
				glcm.at<float>(i, j) /= R;
			}
		}

	}
	else if (angle == GLCM_ANGLE_BACKDIGONAL)
	{
		for (size_t i = 0; i < srcImage.rows; i++)
		{
			for (size_t j = 0; j < srcImage.cols; j++)
			{
				if ((i + gray_distance) >= 0 && (i + gray_distance) < srcImage.rows && (j - gray_distance) >= 0 && (j - gray_distance) < srcImage.cols)
				{
					glcm.at<float>(srcImage.at<uint_least8_t>(i, j), srcImage.at<uint_least8_t>(i + gray_distance, j - gray_distance)) += 1;
				}
				if ((i - gray_distance) >= 0 && (i - gray_distance) < srcImage.rows && (j + gray_distance) >= 0 && (j + gray_distance) < srcImage.cols)
				{
					glcm.at<float>(srcImage.at<uint_least8_t>(i, j), srcImage.at<uint_least8_t>(i - gray_distance, j + gray_distance)) += 1;
				}
			}
		}
		float R = 2 * (srcImage.rows - gray_distance) * (srcImage.cols - gray_distance);
		for (size_t i = 0; i < glcm.rows; i++)
		{
			for (size_t j = 0; j < glcm.cols; j++)
			{
				glcm.at<float>(i, j) /= R;
			}
		}
	}
	return glcm;
}

Mat GrayCoMatrix::calcGLCMFeatureVector(Mat glcm)
{
	double energy = 0;  //能量（角二阶距，Angular Second Moment)
	double contrast = 0; //对比度 
	double homogenity = 0; //一致性 (倒数差分距 inverse Difference Moment)
	double entropy = 0; //熵



	for (size_t i = 0; i < glcm.rows; i++)
	{
		for (size_t j = 0; j < glcm.cols; j++)
		{
			energy += glcm.at<float>(i, j) * glcm.at<float>(i, j);
			contrast += (i - j)*(i - j)*glcm.at<float>(i, j);
			homogenity += glcm.at<float>(i, j) / (1 + (i - j)*(i - j));
			if (glcm.at<float>(i, j)>0)
				entropy += glcm.at<float>(i, j) * log2l(static_cast<double>(glcm.at<float>(i, j)));
		}
	}

	Mat feature_vector = (Mat_<double>(1, 4) << energy, contrast, homogenity, entropy);

	return feature_vector;
}