#pragma once
#include <opencv.hpp>

/*
Classifier ���ǻ���BP�㷨�ķ�����
*/
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
	Classifier(int class_num, int feat_num, int hl_num, int hu_num); // ��ʼ��������
	Classifier() : Classifier(0, 0, 0, 0) {}; 
	~Classifier() = default;
	
	cv::Mat(*ExtractFeatureFunction)(cv::Mat); // ��ȡ���������ĺ���ָ��
	bool(*JudgementFunction)(std::vector<std::pair<float, int>> &result, int label);
	int FeatureNumber; // ��������
	int ClassNumber; // ��������
	int HiddenLayerNumber; // ���������
	int HiddenUnitsNumber; // �����㵥Ԫ����ÿ��������ĵ�Ԫ����ͬ��
	std::string NetName; //��������
	cv::Ptr<cv::ml::ANN_MLP> Network; // ������
	cv::Ptr<cv::ml::SVM> Svm; // ʹ��֧���������ķ����������ݲ�֧�֣�
	void Predict(cv::Mat src, std::vector<std::pair<float, int>>& output, int order = RESULT_ORDER::ORDER_DECENDING); // Ԥ��
	void Train(std::string path, int test_flags = 0, float test_rate = 0.0, std::vector<float> &test_result = std::vector<float>()); // ѵ��������
	void Train_SVM(std::string path, int test_flags = 0, float test_rate = 0.0, std::vector<float> &test_result = std::vector<float>()); // ʹ��SVMѵ�������������ݲ�֧�֣�
	void Save(std::string filename); // ��������ṹ���ļ����ļ���ʽΪxml��
	void Save(cv::FileStorage & fs); // ��������ṹ��FileStorage���ñ��淽��Ϊ���ݶ��������������ͬһ���ļ�
	void Load(std::string filename); // ���ļ���ȡ����ṹ
	void Load(cv::FileStorage & fs); // ��FileStroage��ȡ����ṹ
	
	
private: 
	// ����
	struct Sample
	{
		cv::Mat image; // ������ԴͼƬ
		int label; // �����ı�ǩ
	};
	typedef std::vector<std::pair<float, int>> Result; // Ԥ����
	void readSamples(); // ���ļ�����������
	void scanImages(std::string path, std::vector<std::string> &files); // ��ȡĿ¼��ȫ��ͼƬ·��
	std::vector<std::vector<Sample>> samples_libary; // ������
	std::string samples_root; // ����ͼƬ�ĸ�Ŀ¼
	
};

