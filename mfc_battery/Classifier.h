#pragma once
#include <opencv.hpp>

/*
Classifier 类是基于BP算法的分类器
*/
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
	Classifier(int class_num, int feat_num, int hl_num, int hu_num); // 初始化分类器
	Classifier() : Classifier(0, 0, 0, 0) {}; 
	~Classifier() = default;
	
	cv::Mat(*ExtractFeatureFunction)(cv::Mat); // 提取特征向量的函数指针
	bool(*JudgementFunction)(std::vector<std::pair<float, int>> &result, int label);
	int FeatureNumber; // 特征数量
	int ClassNumber; // 分类数量
	int HiddenLayerNumber; // 隐含层层数
	int HiddenUnitsNumber; // 隐含层单元数（每个隐含层的单元数相同）
	std::string NetName; //网络名称
	cv::Ptr<cv::ml::ANN_MLP> Network; // 神经网络
	cv::Ptr<cv::ml::SVM> Svm; // 使用支持向量机的分类器（先暂不支持）
	void Predict(cv::Mat src, std::vector<std::pair<float, int>>& output, int order = RESULT_ORDER::ORDER_DECENDING); // 预测
	void Train(std::string path, int test_flags = 0, float test_rate = 0.0, std::vector<float> &test_result = std::vector<float>()); // 训练样本集
	void Train_SVM(std::string path, int test_flags = 0, float test_rate = 0.0, std::vector<float> &test_result = std::vector<float>()); // 使用SVM训练样本集（现暂不支持）
	void Save(std::string filename); // 保存网络结构至文件（文件格式为xml）
	void Save(cv::FileStorage & fs); // 保存网络结构至FileStorage，该保存方法为兼容多个分类器保存至同一个文件
	void Load(std::string filename); // 从文件读取网络结构
	void Load(cv::FileStorage & fs); // 从FileStroage读取网络结构
	
	
private: 
	// 样本
	struct Sample
	{
		cv::Mat image; // 样本的源图片
		int label; // 样本的标签
	};
	typedef std::vector<std::pair<float, int>> Result; // 预测结果
	void readSamples(); // 从文件建立样本库
	void scanImages(std::string path, std::vector<std::string> &files); // 获取目录中全部图片路径
	std::vector<std::vector<Sample>> samples_libary; // 样本库
	std::string samples_root; // 样本图片的根目录
	
};

