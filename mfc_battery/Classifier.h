#pragma once
#include <opencv.hpp>

enum TEST_FLAGS
{
	TEST_NEED = 1, // 需要测试
	TEST_SHOW_DETAILS = 2, // 显示训练细节
	TEST_SHUFFLE = 4, // 随机打乱顺序测试
};

class Classifier
{
public:
	Classifier();
	~Classifier();
	cv::Mat(*ExtractFeatureFunction)(cv::Mat); // 提取特征向量的函数指针
	int FeatureNumber; // 特征数量
	int ClassNumber; // 分类数量
	int HiddenLayerNumber;
	int HiddenUnitsNumber;
	cv::Ptr<cv::ml::ANN_MLP> Network; // 神经网络
	void Predict(cv::Mat src,std::vector<std::pair<float,int>> &output);
	void Train(std::string path, int test_flags = 0, float test_rate = 0.0, std::vector<float> &test_result = std::vector<float>());
	void Save(std::string filename);
	void Load(std::string filename);
	
	
private:
	struct Sample
	{
		cv::Mat image;
		int label;
	};
	void readSamples();
	void scanImages(std::string path, std::vector<std::string> files);
	std::vector<Sample> samples_libary;
	std::string samples_root;
};

