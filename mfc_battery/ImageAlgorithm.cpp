#include "stdafx.h"
#include "ImageAlgorithm.h"
#include "cv_glcm.h"
using namespace std;

#pragma region 包膜电池相关算法

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

// 提取未包膜电池侧面特征的算法
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

 // 提取盖板锈痕特征
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

// 提取盖板凹陷特征
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

// 包膜电池侧面定位及提取ROI区域算法
void region::detectColourBattery(Mat & src, Mat & dst)
{
	int t1 = 60;
	int t2 = 100;
	int in = 5;
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
void region::detectNakedBattery(Mat & src, Mat & dst)
{
	int t1 = 140;
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
	}
}
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
void region::detectLowSideBattery(Mat & src, Mat & dst)
{

}
#pragma endregion

