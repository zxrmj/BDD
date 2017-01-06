#include "stdafx.h"
#include "ImageAlgotherm.h"
#include "cv_glcm.h"
using namespace std;

	// 包膜电池侧面特征提取算法
	Mat feature::extractColorFeature(Mat src)
	{
		Mat hsi;
		cvtColor(src, hsi, CV_BGR2HSV);
		resize(hsi, hsi, Size(200, 800));
		vector<Mat> v_hsi;
		split(hsi, v_hsi);
		Mat dst;
		Mat intensity;
		GaussianBlur(v_hsi[2], intensity, Size(7, 7), 1.2);
		Mat grad_x, grad_y, grad_abs_x, grad_abs_y, sobel;
		Sobel(intensity, grad_x, -1, 1, 0, 3);
		Sobel(intensity, grad_y, -1, 0, 1, 3);
		convertScaleAbs(grad_x, grad_abs_x);
		convertScaleAbs(grad_y, grad_abs_y);
		addWeighted(grad_abs_x, 1, grad_abs_y, 0, 0, sobel);
		Mat feat_vec;
		feat_vec.create(1, 40, CV_32FC1);
		Mat_<float>::iterator feat_vec_iter = feat_vec.begin<float>();
		for (int i = 0; i < hsi.rows; i += 20)
		{
			Mat sub_mat = sobel(Rect(0, i, hsi.cols, 20));
			Mat glcm_mat = GrayCoMatrix::createGLCM(sub_mat, GrayCoMatrix::GLCM_ANGLE_VERTICAL);
			Mat return_mat = GrayCoMatrix::calcGLCMFeatureVector(glcm_mat);
			*feat_vec_iter++ = return_mat.at<double>(0, 0);
		}
		return feat_vec;
	}

void region::detectColourBattery(Mat & src, Mat & dst)
{
	{
		Mat roi = src(Rect(src.cols / 2, 0, src.cols / 2, src.rows));
		Mat hsi;
		cvtColor(roi, hsi, CV_BGR2HSV);
		vector<Mat> v_hsi;
		split(hsi, v_hsi);
		Mat hue = v_hsi[0];
		Mat binary = hue > t1 & hue < t2;
		morphologyEx(binary, binary, MORPH_OPEN, getStructuringElement(MORPH_RECT, Size(9, 9)), Point(-1, -1), in);
		vector<vector<Point>> contours;
		findContours(binary, contours, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
		Mat mask = Mat::zeros(binary.rows, binary.cols, CV_8UC3);
		for (int i = 0; i < contours.size(); i++)
		{
			drawContours(mask, contours, i, Scalar(rand() & 255, rand() & 255, rand() & 255), -1, LINE_AA);
			if (contourArea(contours[i]) > 100000)
			{
				RotatedRect rr = minAreaRect(contours[i]);
				rectangle(roi, rr.boundingRect(), Scalar(0, 255, 0), 2, LINE_AA);
				Point2f srcTriangle[3], dstTriangle[3];// 0 -> 左上角 1->右上角 2->左下角
				Point2f vertices[4];
				Point2f center = rr.center;
				rr.points(vertices);
				for (int k = 0; k < 4; k++)
				{
					if (vertices[k].x < center.x) // 左上
					{
						if (vertices[k].y < center.y)
						{
							srcTriangle[0] = vertices[k];
						}
						else
						{
							srcTriangle[2] = vertices[k];
						}
					}
					else if (vertices[k].y < center.y)
					{
						srcTriangle[1] = vertices[k];
					}
				}
				float width = min(rr.size.width, rr.size.height);

				float height = max(rr.size.width, rr.size.height);
				dst = Mat::zeros(height, width, CV_8UC3);
				dstTriangle[0] = Point2f(0, 0);
				dstTriangle[1] = Point2f(width, 0);
				dstTriangle[2] = Point2f(0, height);
				Mat warpMat = getAffineTransform(srcTriangle, dstTriangle);
				warpAffine(roi, dst, warpMat, dst.size());
			}
		}
	}
}
