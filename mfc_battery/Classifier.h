#pragma once
#include <opencv.hpp>

enum TEST_FLAGS
{
	TEST_NEED = 1, // 需要测试
	TEST_SHOW_DETAILS = 2, // 显示训练细节
	TEST_SHUFFLE = 4, // 随机打乱顺序测试
};
enum RESULT_ORDER
{
	ORDER_NORMAL = 0,// 按输出顺序排列
	ORDER_ASCENDING = 1, // 按结果值升序排列，结果值小的在前
	ORDER_DECENDING = 2, // 按结果值降序排列，结果值大的在前
};

bool default_judge_func(std::vector<std::pair<float, int>> &result, int label);

class Classifier
{
public:
	Classifier(int class_num, int feat_num, int hl_num, int hu_num);
	Classifier() : Classifier(0, 0, 0, 0) {};
	~Classifier() = default;
	
	cv::Mat(*ExtractFeatureFunction)(cv::Mat); // 提取特征向量的函数指针
	bool(*JudgementFunction)(std::vector<std::pair<float, int>> &result, int label);
	int FeatureNumber; // 特征数量
	int ClassNumber; // 分类数量
	int HiddenLayerNumber;
	int HiddenUnitsNumber;
	std::string NetName;
	cv::Ptr<cv::ml::ANN_MLP> Network; // 神经网络
	cv::Ptr<cv::ml::SVM> Svm;
	void Predict(cv::Mat src, std::vector<std::pair<float, int>>& output, int order = RESULT_ORDER::ORDER_DECENDING);
	void Train(std::string path, int test_flags = 0, float test_rate = 0.0, std::vector<float> &test_result = std::vector<float>());
	void Train_SVM(std::string path, int test_flags = 0, float test_rate = 0.0, std::vector<float> &test_result = std::vector<float>());
	void Save(std::string filename);
	void Save(cv::FileStorage & fs);
	void Load(std::string filename);
	void Load(cv::FileStorage & fs);
	
	
private: 
	struct Sample
	{
		cv::Mat image;
		int label;
	};
	typedef std::vector<std::pair<float, int>> Result;
	void readSamples();
	void scanImages(std::string path, std::vector<std::string> &files);
	std::vector<std::vector<Sample>> samples_libary;
	std::string samples_root;
	
};

