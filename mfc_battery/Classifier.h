#pragma once
#include <opencv.hpp>

enum TEST_FLAGS
{
	TEST_NEED = 1, // ��Ҫ����
	TEST_SHOW_DETAILS = 2, // ��ʾѵ��ϸ��
	TEST_SHUFFLE = 4, // �������˳�����
};
enum RESULT_ORDER
{
	ORDER_NORMAL = 0,// �����˳������
	ORDER_ASCENDING = 1, // �����ֵ�������У����ֵС����ǰ
	ORDER_DECENDING = 2, // �����ֵ�������У����ֵ�����ǰ
};

bool default_judge_func(std::vector<std::pair<float, int>> &result, int label);

class Classifier
{
public:
	Classifier(int class_num, int feat_num, int hl_num, int hu_num);
	Classifier() : Classifier(0, 0, 0, 0) {};
	~Classifier() = default;
	
	cv::Mat(*ExtractFeatureFunction)(cv::Mat); // ��ȡ���������ĺ���ָ��
	bool(*JudgementFunction)(std::vector<std::pair<float, int>> &result, int label);
	int FeatureNumber; // ��������
	int ClassNumber; // ��������
	int HiddenLayerNumber;
	int HiddenUnitsNumber;
	std::string NetName;
	cv::Ptr<cv::ml::ANN_MLP> Network; // ������
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

