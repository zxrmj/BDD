#pragma once
#include <opencv.hpp>

enum TEST_FLAGS
{
	TEST_NEED = 1, // ��Ҫ����
	TEST_SHOW_DETAILS = 2, // ��ʾѵ��ϸ��
	TEST_SHUFFLE = 4, // �������˳�����
};

class Classifier
{
public:
	Classifier();
	~Classifier();
	cv::Mat(*ExtractFeatureFunction)(cv::Mat); // ��ȡ���������ĺ���ָ��
	int FeatureNumber; // ��������
	int ClassNumber; // ��������
	int HiddenLayerNumber;
	int HiddenUnitsNumber;
	cv::Ptr<cv::ml::ANN_MLP> Network; // ������
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

