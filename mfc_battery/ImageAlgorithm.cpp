#include "stdafx.h"
#include "ImageAlgorithm.h"
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
	Mat gray;
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
	return feat_vec;
}

/// <summary>
/// 盖板锈痕特征提取算法
/// </summary>
/// <param name="src">电池的源图像</param>
/// <return>特征向量</return>
Mat feature::extractRustyFeature(Mat src)
{
	Mat hsv;
	cvtColor(src, hsv, CV_BGR2HSV);
	vector<Mat> v_hsv;
	split(hsv, v_hsv);
	Mat hue_img = v_hsv[0];
	Mat feat_vec = Mat::zeros(1, 24, CV_32FC1);
	for (int i = 0; i < hue_img.rows; i++)
	{
		for (int j = 0; j < hue_img.cols; j++)
		{
			uchar val = hue_img.at<uchar>(i, j);
			feat_vec.at<float>(0, val / 10)++;
		}
	}
	for (int i = 0; i < feat_vec.cols; i++)
	{
		feat_vec.at<float>(0, i) /= (hue_img.rows * hue_img.cols);
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
	cvtColor(src, src, CV_BGR2GRAY);
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
void region::detectNakedBattery(Mat & src, Mat & dst)
{
	/*int t1 = 140;
	int t2 = 940;
	Mat roi = src(Rect(src.cols / 2, 0, src.cols / 2, src.rows));
	Mat gray;
	cvtColor(roi, gray, CV_BGR2GRAY);
	Mat grad_y, grad_abs_y;
	Sobel(gray, grad_y, -1, 0, 1, 3);
	convertScaleAbs(grad_y, grad_abs_y);
	Mat binary;
	cout << threshold(grad_abs_y, binary, 128, 255, ThresholdTypes::THRESH_BINARY | ThresholdTypes::THRESH_OTSU) << endl;
	binary = binary(Rect(0, binary.rows * 5 / 6, binary.cols, binary.rows / 6));
	morphologyEx(binary, binary, MORPH_CLOSE, getStructuringElement(MORPH_RECT, Size(20, 2)), Point(-1, -1), 2);
	vector<vector<Point>> contours;
	findContours(binary, contours, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
	int lower_y = 0;
	Rect lower_rect = Rect(0, 0, 0, 0);
	for (int i = 0; i < contours.size(); i++)
	{
		if (contourArea(contours[i]) > 500)
		{
			Rect rect = boundingRect(contours[i]);
			if (rect.y > lower_rect.y)
			{
				lower_rect = rect;
				lower_y = i;
			}
		}
	}
	Point lower_pt = Point(0, 0);
	for (int i = 0; i < contours[lower_y].size(); i++)
	{
		if (contours[lower_y][i].y > lower_pt.y)
		{
			lower_pt = contours[lower_y][i];
		}
	}
	circle(binary, lower_pt, 5, Scalar(255), -1, LINE_AA);
	try {
		roi(Rect(lower_pt.x - t1, lower_pt.y + roi.rows * 5 / 6 - t2, t1 * 2, t2)).copyTo(dst);
		rectangle(roi, Rect(lower_pt.x - t1, lower_pt.y + roi.rows * 5 / 6 - t2, t1 * 2, t2), Scalar(0, 255, 0), 1, LINE_AA);
	}
	catch (cv::Exception)
	{
		;
	}*/
	//battery = dstImage.clone();
	vector<Mat> v_rgb;
	split(src, v_rgb);
	Mat &blue = v_rgb[2];
	Canny(blue, blue, 100, 200, 3);
	vector<vector<Point>> contours;
	//color = Mat::zeros(dstImage.rows, dstImage.cols, CV_8UC3);
	findContours(blue, contours, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
	Rect max_rect;
	int max_idx = -1;
	int max_y = 0;
	for (int i = 0; i < contours.size(); i++)
	{
		Rect rect = minAreaRect(contours[i]).boundingRect();
		double len = rect.width;
		if (len > 80)
		{
			//drawContours(color, contours, i, Scalar(/*rand() & 255, rand() & 255, rand() & 255*/0, 255, 0), 1, LINE_AA);

			if (rect.y > 850)
			{
				if (rect.y > max_y)
				{
					max_y = rect.y;
					max_idx = i;
					max_rect = rect;
				}

				//rectangle(battery, rect, Scalar(0, 255, 0), 1, LINE_AA);
			}
		}

	}
	if (max_idx == -1)
		return;
	Point center = Point(max_rect.x + max_rect.width / 2, max_rect.y + max_rect.height);
	Rect r = Rect(center.x - 200 / 2, center.y - 850, 200, 850);
	Point points[] = { Point(r.x,r.y),Point(r.x + r.width,r.y),Point(r.x,r.y + r.height),Point(r.x + r.width,r.y + r.height) };
	bool _inside = true;
	for (int i = 0;i < 4; i++)
	{
		if (points[i].x < 0 || points[i].y < 0 || points[i].y >= src.rows || points[i].x >= src.cols)
			_inside = false;
	}
	if (_inside)
	{
		src(r).copyTo(dst);
	}
	
	rectangle(src, r, Scalar(0, 255, 0), 1, LINE_AA);
	return;
	
}

/// <summary>
/// 包膜电池顶面定位及提取ROI区域算法
/// </summary>
/// <param name="src">相机捕获的图像</param>
/// <param name="dst">电池区域图像</param>
void region::detectFloorSideBattery(Mat & src, Mat & dst)
{
	src = src(Range(250, 700), Range(400, 850));
	Mat gray;
	cvtColor(src, gray, CV_BGR2GRAY);

	Mat binary;
	threshold(gray, binary, 128, 255, ThresholdTypes::THRESH_BINARY | ThresholdTypes::THRESH_OTSU);
	vector<Point> pts;
	for (int i = 0; i < binary.rows; i++) {
		for (int j = 0; j < binary.cols; j++) {
			if (binary.at<uchar>(Point(j, i)) == 255) {
				pts.push_back(Point(j, i));
			}
		}
	}
	Rect rect = boundingRect(pts);
	dst = src(rect);
	return;
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
