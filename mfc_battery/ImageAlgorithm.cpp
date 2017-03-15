#include "stdafx.h"
#include "ImageAlgorithm.h"
#include "BatteryTopDectector.h"
#include "cv_glcm.h"
using namespace std;


/// <summary>
/// 包膜电池侧面特征提取算法
/// </summary>
/// <param name="src">电池的源图像</param>
/// <return>特征向量</return>
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

/// <summary>
/// 未包膜电池侧面特征提取算法
/// </summary>
/// <param name="src">电池的源图像</param>
/// <return>特征向量</return>
Mat feature::extractNakedFeature(Mat src)
{
	/*Mat gray;
	cvtColor(src, gray, CV_BGR2GRAY);
	resize(gray, gray, Size(200, 800));
	Mat grad_y, sobel;
	Sobel(gray, grad_y, -1, 0, 1, 3);
	convertScaleAbs(grad_y, sobel);
	Mat feat_vec;
	feat_vec.create(1, 40, CV_32FC1);
	Mat_<float>::iterator feat_vec_iter = feat_vec.begin<float>();
	for (int i = 0; i < gray.rows; i += 20)
	{
		Mat sub_mat = sobel(Rect(0, i, gray.cols, 20));
		Mat glcm_mat = GrayCoMatrix::createGLCM(sub_mat, GrayCoMatrix::GLCM_ANGLE_VERTICAL);
		Mat return_mat = GrayCoMatrix::calcGLCMFeatureVector(glcm_mat);
		*feat_vec_iter++ = return_mat.at<double>(0, 3);
	}
	return feat_vec;*/

	Mat gray;
	cvtColor(src, gray, CV_BGR2GRAY);
	resize(gray, gray, Size(200, 840));
	Mat grad_y, sobel;
	Sobel(gray, grad_y, -1, 0, 1, 3);
	convertScaleAbs(grad_y, sobel);
	Mat feat_vec;
	feat_vec.create(1, 6, CV_32FC1);
	Mat_<float>::iterator feat_vec_iter = feat_vec.begin<float>();
	int _step = 840 / 6;
	for (int i = 0; i < gray.rows; i += _step)
	{
		Mat sub_mat = sobel(Rect(0, i, gray.cols, _step));
		Mat glcm_mat = GrayCoMatrix::createGLCM(sub_mat, GrayCoMatrix::GLCM_ANGLE_VERTICAL);
		Mat return_mat = GrayCoMatrix::calcGLCMFeatureVector(glcm_mat);
		*feat_vec_iter++ = return_mat.at<double>(0, 3);
	}
	return feat_vec;
}

Mat feature::extractNakedLocalFeature(Mat src)
{
	auto entropy_gray = [](Mat src) -> float {
		float hist[256] = { 0 };
		for (int i = 0; i < src.total(); i++)
			hist[*(src.data + i)]++;
		float e = 0.0;
		for (int i = 0; i < 256; i++)
		{
			float r = hist[i] / (float)src.total();
			if (r)
				e -= r*log2f(r);
		}
		return e;
	};
	if (src.channels() != 1)
		return Mat();
	Mat sobel;
	Mat morph;
	Sobel(src, sobel, -1, 0, 1, 5);
	convertScaleAbs(sobel, sobel);
	GaussianBlur(sobel, sobel, Size(5, 5), 1.2);
	threshold(sobel, morph, 128, 255, THRESH_TOZERO);
	morphologyEx(morph, morph, MORPH_OPEN, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)), Point(-1, -1), 3);
	Mat feat_vec = Mat::zeros(1, 8, CV_32FC1);
	Mat v_glcm = GrayCoMatrix::createGLCM(sobel, GrayCoMatrix::GLCM_ANGLE_VERTICAL);
	Mat h_glcm = GrayCoMatrix::createGLCM(sobel, GrayCoMatrix::GLCM_ANGLE_HORIZATION);
	Mat v_features = GrayCoMatrix::calcGLCMFeatureVector(v_glcm);
	Mat h_features = GrayCoMatrix::calcGLCMFeatureVector(h_glcm);
	feat_vec.at<float>(0, 0) = entropy_gray(sobel);
	feat_vec.at<float>(0, 1) = v_features.at<double>(0, 2);
	feat_vec.at<float>(0, 2) = v_features.at<double>(0, 3);
	feat_vec.at<float>(0, 3) = h_features.at<double>(0, 2);
	feat_vec.at<float>(0, 4) = h_features.at<double>(0, 3);

	v_glcm = GrayCoMatrix::createGLCM(sobel, GrayCoMatrix::GLCM_ANGLE_VERTICAL);
	v_features = GrayCoMatrix::calcGLCMFeatureVector(v_glcm);
	feat_vec.at<float>(0, 5) = entropy_gray(sobel);
	feat_vec.at<float>(0, 6) = v_features.at<double>(0, 2);
	feat_vec.at<float>(0, 7) = v_features.at<double>(0, 3);
	return feat_vec;
}

/// <summary>
/// 盖板锈痕特征提取算法
/// </summary>
/// <param name="src">电池的源图像</param>
/// <return>特征向量</return>
Mat feature::extractRustyFeature(Mat src)
{
	Mat sobel;
	auto _sobel = [](Mat src) -> Mat {
		Mat grad_x;
		Mat grad_y;
		Sobel(src, grad_x, -1, 1, 0, 3);
		Sobel(src, grad_y, -1, 0, 1, 3);
		convertScaleAbs(grad_x, grad_x);
		convertScaleAbs(grad_y, grad_y);
		Mat sobel;
		addWeighted(grad_x, 0.5, grad_y, 0.5, 0, sobel);
		return sobel;
	};
	sobel = _sobel(src);
	Mat feat_vec = Mat::zeros(1, 24, CV_32FC1);
	for (int i = 0; i < sobel.rows; i++)
	{
		for (int j = 0; j < sobel.cols; j++)
		{
			uchar val = sobel.at<uchar>(i, j);
			feat_vec.at<float>(0, val / 10)++;
		}
	}
	for (int i = 0; i < feat_vec.cols; i++)
	{
		feat_vec.at<float>(0, i) /= (sobel.rows * sobel.cols);
	}
	return feat_vec;
}

/// <summary>
/// 盖板凹陷特特征提取算法
/// </summary>
/// <param name="src">电池的源图像</param>
/// <return>特征向量</return>
Mat feature::extractUpSidePitFeature(Mat src)
{
	resize(src, src, Size(200, 200));
	Point center(100, 100);
	Mat mask = Mat::zeros(200, 200, CV_8UC1);
	circle(mask, center, 45, Scalar(255), -1, LINE_AA);
	Mat sub = (src & mask)(Rect(Point(50, 50), Point(150, 150)));
	Mat glcm_mat_v = GrayCoMatrix::createGLCM(sub, GrayCoMatrix::GLCM_ANGLE_VERTICAL);
	Mat glcm_mat_h = GrayCoMatrix::createGLCM(sub, GrayCoMatrix::GLCM_ANGLE_HORIZATION);
	Mat feat_vec(1, 2, CV_32FC1);
	Mat return_mat_v = GrayCoMatrix::calcGLCMFeatureVector(glcm_mat_v);
	Mat return_mat_h = GrayCoMatrix::calcGLCMFeatureVector(glcm_mat_h);
	feat_vec.at<float>(0, 0) = return_mat_v.at<double>(0, 1);
	feat_vec.at<float>(0, 1) = return_mat_h.at<double>(0, 1);
	return feat_vec;
}

/// <summary>
/// 包膜电池侧面定位及提取ROI区域算法
/// </summary>
/// <param name="src">相机捕获的图像</param>
/// <param name="dst">电池区域图像</param>
void region::detectColourBattery(Mat & src, Mat & dst)
{
	int t1 = 100;
	int t2 = 200;
	int in = 3;
	Mat roi = src(Rect(src.cols / 2, 0, src.cols / 2, src.rows));
	Mat hsi;
	cvtColor(roi, hsi, CV_BGR2HSV);
	vector<Mat> v_hsi;
	split(hsi, v_hsi);
	Mat hue = v_hsi[2];
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

/// <summary>
/// 未包膜电池侧面定位及提取ROI区域算法
/// </summary>
/// <param name="src">相机捕获的图像</param>
/// <param name="dst">电池区域图像</param>
void region::detectNakedBattery(Mat & src, Mat & dst, Point &ru)
{
	int low_thresh = 200;
	int high_thresh = 255;
	int battery_height = 800;
	int battery_width = 200;
	int upper_line = 100;
	int lower_line = 200;
	int left_line = 500;
	int right_line = 850;
	
	Mat gray;
	if (src.channels() == 3)
		cvtColor(src, gray, CV_BGR2GRAY);
	else
	{
		gray = src;
	}

	Rect rect(Point(left_line, upper_line), Point(right_line, lower_line));
	Mat canny;
	Canny(gray, canny, low_thresh, high_thresh);

	vector<vector<Point>> contours;
	findContours(canny, contours, RetrievalModes::RETR_CCOMP, ContourApproximationModes::CHAIN_APPROX_SIMPLE);
	int highest_line_idx = -1;
	int higest_line = gray.rows;

	auto findHighestPoint = [](vector<Point> vp)->Point {
		Rect rect = boundingRect(vp);
		Point p;
		p.x = rect.x + rect.width / 2;
		p.y = rect.y;
		return p;
	};

	auto batteryBoundingRect = [](Point center, int width, int height) -> Rect {
		Rect rect;
		rect.x = center.x - width / 2;
		rect.width = width;
		rect.y = center.y;
		rect.height = height;
		return rect;
	};

	for (int i = 0; i < contours.size(); i++)
	{
		RotatedRect rotated = minAreaRect(contours[i]);
		Point center = rotated.center;
		if (rect.contains(center) && rotated.boundingRect().width > 30)
		{
			if (center.y < higest_line)
			{
				higest_line = center.y;
				highest_line_idx = i;
			}
		}
	}

	if (highest_line_idx == -1)
		return;

	Point center = findHighestPoint(contours[highest_line_idx]);

	Rect rec = batteryBoundingRect(center, battery_width, battery_height);
	dst = src(rec).clone();
	rectangle(src, rec, Scalar(0, 255, 0));
	ru.x = rec.x;
	ru.y = rec.y;
}

/// <summary>
/// 包膜电池顶面定位及提取ROI区域算法
/// </summary>
/// <param name="src">相机捕获的图像</param>
/// <param name="dst">电池区域图像</param>
void region::detectTopSideBattery(Mat &src, Mat & dst)
{
	Mat gray;
	if (src.channels() == 3)
		cvtColor(src, gray, CV_BGR2GRAY);
	else
		gray = src;
	Mat binary;
	binary = gray > 15 & gray < 255;
	vector<vector<Point>> contours;
	findContours(binary, contours, RetrievalModes::RETR_CCOMP, CHAIN_APPROX_SIMPLE);
	int max_area_idx = -1;
	float max_area = 0;
	for (int i = 0; i < contours.size(); i++)
	{
		float area = contourArea(contours[i]);
		if (area > max_area)
		{
			max_area = area;
			max_area_idx = i;
		}
	}
	Rect rect = boundingRect(contours[max_area_idx]);
	dst = src(rect).clone();
	rectangle(src, rect, Scalar(0, 255, 0));
}

/// <summary>
/// 包膜电池底面定位及提取ROI区域算法
/// </summary>
/// <param name="src">相机捕获的图像</param>
/// <param name="dst">电池区域图像</param>
void region::detectLowSideBattery(Mat & src, Mat & dst)
{

}


/// <summary>
/// 建立图像拼接映射关系
/// </summary>
/// <param name="map_x">相机捕获的图像</param>
/// <param name="map_y">电池区域图像</param>
/// <param name="_width">电池区域图像</param>
/// <param name="_height">电池区域图像</param>
/// <param name="pos">电池区域图像</param>
void joint::createJointMappingRelation(Mat & map_x, Mat & map_y, int _width, int _height, int pos)
{
	if (pos == JOINT_UL) // 左上
	{
		for (int i = 0; i < _height; i++)
		{
			for (int j = 0; j < _width; j++)
			{
				map_x.at<float>(i, j) = static_cast<float>(j);
				map_y.at<float>(i, j) = static_cast<float>(i);
			}
		}
	}
	else if (pos == JOINT_UR)
	{
		for (int i = 0; i < _height; i++)
		{
			for (int j = 0; j < _width; j++)
			{
				map_x.at<float>(i, j) = static_cast<float>(j + _width);
				map_y.at<float>(i, j) = static_cast<float>(i);
			}
		}
	}
	else if (pos == JOINT_LL)
	{
		for (int i = 0; i < _height; i++)
		{
			for (int j = 0; j < _width; j++)
			{
				map_x.at<float>(i, j) = static_cast<float>(j);
				map_y.at<float>(i, j) = static_cast<float>(i + _height);
			}
		}
	}
	else if (pos == JOINT_LR)
	{
		for (int i = 0; i < _height; i++)
		{
			for (int j = 0; j < _width; j++)
			{
				map_x.at<float>(i, j) = static_cast<float>(j + _width);
				map_y.at<float>(i, j) = static_cast<float>(i + _height);
			}
		}
	}
	
}
