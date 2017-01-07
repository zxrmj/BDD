#include "stdafx.h"
#include "Classifier.h"
#define _SCL_SECURE_NO_WARNINGS
#include <algorithm> 
#include <io.h>
#if _MSC_VER >= 1800
#include <random> 
#include <chrono> 
#include <numeric>
#endif // C++11 Support Check

using namespace std;
using namespace cv;

Classifier::Classifier(int class_num, int feat_num, int hl_num, int hu_num)
{
	this->ClassNumber = class_num;
	this->FeatureNumber = feat_num;
	this->HiddenLayerNumber = hl_num;
	this->HiddenUnitsNumber = hu_num;
	this->ExtractFeatureFunction = nullptr;
	this->JudgementFunction = default_judge_func;
}

/// <summary>
/// ʹ��һ��������������Ԥ��
/// </summary>
/// <param name="src">�������������</param>
/// <param name="output">����Ľ��</param>
/// <param name="order">�����������ʽ���μ�RESULT_ORDER</param>
void Classifier::Predict(cv::Mat src, std::vector<std::pair<float, int>>& output,int order)
{
	Mat response;
	response.create(1, ClassNumber ,CV_32FC1);
	Network->predict(src,response);
	output.clear();
	for (int i = 0; i < response.cols; i++)
	{
		output.push_back(pair<float, int>(response.at<float>(0, i), i));
	}
	if (order == RESULT_ORDER::ORDER_ASCENDING)
	{
		sort(output.begin(), output.end(), [](pair<float, int> &r1, pair<float, int> &r2) {return r1.first < r2.first; });
	}
	else if (order == RESULT_ORDER::ORDER_DECENDING)
	{
		sort(output.begin(), output.end(), [](pair<float, int> &r1, pair<float, int> &r2) {return r1.first > r2.first; });
	}
	return;
}

/// <summary>
/// ѵ��������
/// </summary>
/// <param name="path">ѵ�������洢�ĸ�Ŀ¼</param>
/// <param name="test_flags">������������ѡ��μ�TEST_FLAGS�������Կɲ���</param>
/// <param name="test_rate">�������������������ı��ʣ������Կɲ���</param>
/// <param name="test_result">���ԵĽ���������Կɲ���</param>
void Classifier::Train(std::string path, int test_flags, float test_rate, std::vector<float> &test_result)
{
	if (ClassNumber < 1 || FeatureNumber < 1 || HiddenLayerNumber < 1 || HiddenUnitsNumber < 1)
	{
		throw std::exception("ѵ��ǰ����������ȷ�ķ���������������������������ÿ�㵥Ԫ����");
		return;
	}
	bool flag_test = false, flag_show_details = false, flag_shuffle = false;
	// ����if����ΪλУ�飬��test_flags = 5��������Ʊ�ʾΪ101
	//ʹ�����5 & 1 ��101 & 001�� �ý�� 1��5 & 2 ��101 & 010�� �ý��0��5 & 4 ��101 & 100�� �ý��4���Ӷ��жϺ���ʹ������������ЩFlags��
	if (test_flags & 1)
	{
		flag_test = true;
	}
	if (test_flags & 2)
	{
		flag_show_details = true;
	}
	if (test_flags & 4)
	{
		flag_shuffle = true;
	}
	// ������Ŀ¼
	samples_root = path;
	readSamples();
	if (flag_shuffle) // �����������������
	{
#if _MSC_VER >= 1800
		// ȡ����ʱ����ص��������
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		for (int i = 0; i < samples_libary.size(); i++)
		{
			// �������������������
			shuffle(samples_libary[i].begin(), samples_libary[i].end(), default_random_engine(seed));
		}

#else
		// �������
		srand(time(NULL));
		for (int i = 0; i < samples_libary.size(); i++)
		{
			for (int j = 0; j < samples_libary[i].size() * 3; j++)
			{
				int n = rand() % samples_libary[i].size();
				int m = rand() % samples_libary[i].size();
				Sample temp = samples_libary[i][n];
				samples_libary[i][n] = samples_libary[i][m];
				samples_libary[i][m] = temp;
			}
		}
#endif // C++11 Support Check
	}
	// ����ͼƬ����
#if _MSC_VER >= 1800
	int images_count = std::accumulate(samples_libary.begin(), samples_libary.end(), 0, [](int _val, vector<Sample> &samples) { return _val + (int)samples.size(); });
#else
	int images_count = 0;
	for (int i = 0; i < samples_libary.size(); i++)
	{
		images_count += samples_libary[i].size();
	}
#endif // C++11 Support Check
	Mat features = Mat::zeros(images_count, FeatureNumber, CV_32FC1);
	Mat labels = Mat::zeros(images_count, ClassNumber, CV_32FC1);
	int k = 0;
	for (int i = 0; i < samples_libary.size(); i++)
	{
		for (int j = 0; j < samples_libary[i].size(); j++)
		{
			Mat feat_vec = ExtractFeatureFunction(samples_libary[i].at(j).image);
			_Copy_no_deprecate(feat_vec.begin<float>(), feat_vec.end<float>(), features.begin<float>() + k*features.cols);
			labels.at<float>(k, samples_libary[i].at(j).label) = 1;
			k++;
		}
	}
	// ��������ṹ
	Mat layer_setting = Mat_<int>(1, 4, 0);
	layer_setting.at<int>(0, 0) = FeatureNumber;
	for (int i = 1; i <= HiddenLayerNumber; i++)
	{
		layer_setting.at<int>(0, i) = HiddenUnitsNumber;
	}
	layer_setting.at<int>(0, 1 + HiddenLayerNumber) = ClassNumber;
	Network = ml::ANN_MLP::create();
	Network->setLayerSizes(layer_setting);
	Network->setTrainMethod(ml::ANN_MLP::TrainingMethods::BACKPROP);
	Network->setBackpropWeightScale(0.1);
	Network->setBackpropMomentumScale(0.1);
	Network->setActivationFunction(ml::ANN_MLP::ActivationFunctions::SIGMOID_SYM, 1, 1);
	// ����
	if (flag_test)
	{
		float train_rate = 1.0 - test_rate;
		// ��ѵ������д�뵽��Ӧ������
		vector<int> train_numbers;
		for (int i = 0; i < ClassNumber; i++)
		{
			train_numbers.push_back(samples_libary[i].size() * train_rate);
		}
		int train_count = std::accumulate(train_numbers.begin(), train_numbers.end(), 0);
		Mat train_features;
		train_features.create(train_count, FeatureNumber, CV_32FC1);
		Mat train_labels;
		train_labels.create(train_count, ClassNumber, CV_32FC1);
		k = 0;
		int l = 0;
		for (int i = 0; i < ClassNumber; i++)
		{
			_Copy_no_deprecate(features.begin<float>() + k*features.cols, features.begin<float>() + (k + train_numbers[i])*features.cols, train_features.begin<float>() + l*train_features.cols);
			_Copy_no_deprecate(labels.begin<float>() + k*labels.cols, labels.begin<float>() + (k + train_numbers[i])*labels.cols, train_labels.begin<float>() + l*train_labels.cols);
			k += samples_libary[i].size();
			l += train_numbers[i];
		}
		// ����������д�뵽��Ӧ�ľ�����
		vector<int> test_numbers;
		for (int i = 0; i < ClassNumber; i++)
		{
			test_numbers.push_back(samples_libary[i].size() - train_numbers[i]);
		}
		int test_count = images_count - train_count;
		Mat test_features;
		test_features.create(test_count, FeatureNumber, CV_32FC1);
		Mat test_labels;
		test_labels.create(test_count, ClassNumber, CV_32FC1);
		k = 0;
		l = 0;
		for (int i = 0; i < ClassNumber; i++)
		{
			_Copy_no_deprecate(features.begin<float>() + (k + train_numbers[i])*features.cols, features.begin<float>() + (k + samples_libary[i].size())*features.cols, test_features.begin<float>() + l*test_features.cols);
			_Copy_no_deprecate(labels.begin<float>() + (k + train_numbers[i])*labels.cols, labels.begin<float>() + (k + samples_libary[i].size())*labels.cols, test_labels.begin<float>() + l*test_labels.cols);
			k += samples_libary[i].size();
			l += test_numbers[i];
		}
		Ptr<ml::TrainData> train_data = ml::TrainData::create(train_features, ml::ROW_SAMPLE, train_labels);
		cout << train_labels << endl;
		Network->train(train_data);
		cout << Network->isTrained() << endl;

		// ����
		Result result;
		int test_passed = 0;
		int cur_class = 0;
		int cur_number = train_numbers[0];
		for (int i = 0; i < test_features.rows; i++)
		{
			int label = 0;
			for (int j = 1; j < test_labels.cols; j++)
			{
				if (test_labels.at<float>(i, j) == 1)
				{
					label = j;
					break;
				}
			}
			//cout << test_features.row(i) << endl;
			Mat test_feature;
			test_feature.create(1, FeatureNumber, CV_32FC1);
			for (int k = 0; k < FeatureNumber; k++)
			{
				test_feature.at<float>(0, k) = test_features.at<float>(i, k);
			}
			Predict(test_feature, result, RESULT_ORDER::ORDER_DECENDING);
			bool b = (int)(JudgementFunction(result, label));
			test_passed += b;
			if (flag_show_details)
			{			
				cout << "Real Label:" << samples_libary[cur_class][cur_number].label << endl;
				cout << "Predict Label:" << result[0].second << endl;
				cout << "Current Class:" << cur_class << endl;
				cout << "Current Image Number: " << cur_number << endl;
				cout << "============================" << endl;
				imshow("Input Image", samples_libary[cur_class][cur_number].image);
				waitKey(0);
				cur_number++;
				if (cur_number >= samples_libary[cur_class].size())
				{
					cur_class++;
					if(cur_class < ClassNumber)
						cur_number = train_numbers[cur_class];
				}
				
			}
		}
		cout << "Test Over." << endl << "Pass Rate : " << (float)(test_passed) *100 / (float)(test_count) << "%" << endl;
	}
	Ptr<ml::TrainData> train_data = ml::TrainData::create(features, ml::ROW_SAMPLE, labels);
	Network->train(train_data);
	return;
}

/// <summary>
/// ѵ��֧��������
/// </summary>
/// <param name="path">ѵ�������洢�ĸ�Ŀ¼</param>
/// <param name="test_flags">���Ե����ѡ��μ�TEST_FLAGS�������Կɲ���</param>
/// <param name="test_rate">�������������������ı��ʣ������Կɲ���</param>
/// <param name="test_result">���ԵĽ���������Կɲ���</param>
void Classifier::Train_SVM(std::string path, int test_flags, float test_rate, std::vector<float>& test_result)
{
	/*�˺��������һ�����ԣ�SVM->predict��������һ���ܴ��ֵ*/
	return;

	if (ClassNumber < 1 || FeatureNumber < 1 || HiddenLayerNumber < 1 || HiddenUnitsNumber < 1)
	{
		throw std::exception("ѵ��ǰ����������ȷ�ķ���������������������������ÿ�㵥Ԫ����");
		return;
	}
	bool flag_test = false, flag_show_details = false, flag_shuffle = false;
	// ����if����ΪλУ�飬��test_flags = 5��������Ʊ�ʾΪ101
	//ʹ�����5 & 1 ��101 & 001�� �ý�� 1��5 & 2 ��101 & 010�� �ý��0��5 & 4 ��101 & 100�� �ý��4���Ӷ��жϺ���ʹ������������ЩFlags��
	if (test_flags & 1)
	{
		flag_test = true;
	}
	if (test_flags & 2)
	{
		flag_show_details = true;
	}
	if (test_flags & 4)
	{
		flag_shuffle = true;
	}
	samples_root = path;
	readSamples();
	int k = 0;
	if (flag_shuffle) // �����������������
	{
#if _MSC_VER >= 1800
		// ȡ����ʱ����ص��������
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		for (int i = 0; i < samples_libary.size(); i++)
		{
			// �������������������
			shuffle(samples_libary[i].begin(), samples_libary[i].end(), default_random_engine(seed));
		}

#else
		// �������
		srand(time(NULL));
		for (int i = 0; i < samples_libary.size(); i++)
		{
			for (int j = 0; j < samples_libary[i].size() * 3; j++)
			{
				int n = rand() % samples_libary[i].size();
				int m = rand() % samples_libary[i].size();
				Sample temp = samples_libary[i][n];
				samples_libary[i][n] = samples_libary[i][m];
				samples_libary[i][m] = temp;
			}
		}
#endif // C++11 Support Check
	}
	// ����ͼƬ����
#if _MSC_VER >= 1800
	int images_count = std::accumulate(samples_libary.begin(), samples_libary.end(), 0, [](int _val, vector<Sample> &samples) { return _val + (int)samples.size(); });
#else
	int images_count = 0;
	for (int i = 0; i < samples_libary.size(); i++)
	{
		images_count += samples_libary[i].size();
	}
#endif // C++11 Support Check
	Mat features = Mat::zeros(images_count, FeatureNumber, CV_32FC1);
	Mat labels = Mat::zeros(images_count, 1, CV_32SC1);
	for (int i = 0; i < samples_libary.size(); i++)
	{
		for (int j = 0; j < samples_libary[i].size(); j++)
		{
			Mat feat_vec = ExtractFeatureFunction(samples_libary[i].at(j).image);
			_Copy_no_deprecate(feat_vec.begin<float>(), feat_vec.end<float>(), features.begin<float>() + k*features.cols);
			labels.at<float>(k, 0) = samples_libary[i].at(j).label;
			k++;
		}
	}

	Svm = ml::SVM::create();
	Svm->setType(ml::SVM::C_SVC);
	Svm->setKernel(ml::SVM::LINEAR);
	Svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 10000, 1e-6));
	if (flag_test)
	{
		float train_rate = 1.0 - test_rate;
		// ��ѵ������д�뵽��Ӧ������
		vector<int> train_numbers;
		for (int i = 0; i < ClassNumber; i++)
		{
			train_numbers.push_back(samples_libary[i].size() * train_rate);
		}
		int train_count = std::accumulate(train_numbers.begin(), train_numbers.end(), 0);
		Mat train_features;
		train_features.create(train_count, FeatureNumber, CV_32FC1);
		Mat train_labels;
		train_labels.create(train_count, 1, CV_32SC1);
		k = 0;
		int l = 0;
		for (int i = 0; i < ClassNumber; i++)
		{
			_Copy_no_deprecate(features.begin<float>() + k*features.cols, features.begin<float>() + (k + train_numbers[i])*features.cols, train_features.begin<float>() + l*train_features.cols);
			_Copy_no_deprecate(labels.begin<float>() + k*labels.cols, labels.begin<float>() + (k + train_numbers[i])*labels.cols, train_labels.begin<float>() + l*train_labels.cols);
			k += samples_libary[i].size();
			l += train_numbers[i];
		}
		// ����������д�뵽��Ӧ�ľ�����
		vector<int> test_numbers;
		for (int i = 0; i < ClassNumber; i++)
		{
			test_numbers.push_back(samples_libary[i].size() - train_numbers[i]);
		}
		int test_count = images_count - train_count;
		Mat test_features;
		test_features.create(test_count, FeatureNumber, CV_32FC1);
		Mat test_labels;
		test_labels.create(test_count, 1, CV_32SC1);
		k = 0;
		l = 0;
		for (int i = 0; i < ClassNumber; i++)
		{
			_Copy_no_deprecate(features.begin<float>() + (k + train_numbers[i])*features.cols, features.begin<float>() + (k + samples_libary[i].size())*features.cols, test_features.begin<float>() + l*test_features.cols);
			_Copy_no_deprecate(labels.begin<float>() + (k + train_numbers[i])*labels.cols, labels.begin<float>() + (k + samples_libary[i].size())*labels.cols, test_labels.begin<float>() + l*test_labels.cols);
			k += samples_libary[i].size();
			l += test_numbers[i];
		}
		Ptr<ml::TrainData> train_data = ml::TrainData::create(train_features, ml::ROW_SAMPLE, train_labels);
		cout << test_features << endl;
		system("pause");
		Svm->train(train_data);
		//cout << Svm->isTrained() << endl;

		// ����
		Result result;
		int test_passed = 0;
		int cur_class = 0;
		int cur_number = train_numbers[0];
		for (int i = 0; i < test_features.rows; i++)
		{
			int label = 0;
			for (int j = 1; j < test_labels.cols; j++)
			{
				if (test_labels.at<float>(i, j) == 1)
				{
					label = j;
					break;
				}
			}
			//cout << test_features.row(i) << endl;
			Mat test_feature;
			test_feature.create(1, 24, CV_32FC1);
			for (int k = 0; k < 24; k++)
			{
				test_feature.at<float>(0, k) = test_features.at<float>(i, k);
			}
			int result = Svm->predict(test_feature);
			bool b = (test_labels.at<float>(i, 0) == result);
			test_passed += b;
			if (flag_show_details)
			{
				cout << "Real Label:" << samples_libary[cur_class][cur_number].label << endl;
				cout << "Predict Label:" << result << endl;
				cout << "Current Class:" << cur_class << endl;
				cout << "Current Image Number: " << cur_number << endl;
				cout << "============================" << endl;
				imshow("Input Image", samples_libary[cur_class][cur_number].image);
				waitKey(20);
				cur_number++;
				if (cur_number >= samples_libary[cur_class].size())
				{
					cur_class++;
					if (cur_class < ClassNumber)
						cur_number = train_numbers[cur_class];
				}

			}
		}
		cout << "Test Over." << endl << "Pass Rate : " << (float)(test_passed) * 100 / (float)(test_count) << "%" << endl;
	}
}

/// <summary>
/// ��������д�����ļ�
/// </summary>
/// <param name="filename">����·��</param>
void Classifier::Save(std::string filename)
{
	FileStorage fs(filename, FileStorage::WRITE);
	Save(fs);
	fs.release();
	return;
}

/// <summary>
/// ��������д����FileStorage
/// </summary>
/// <param name="fs">Ҫд���FileStorage����</param>
void Classifier::Save(FileStorage &fs)
{
	string classifier_name = (NetName.empty() ? "Classifier" : NetName);
	fs << classifier_name << "{";
	fs << "ClassNumber" << ClassNumber;
	fs << "FeatureNumber" << FeatureNumber;
	fs << "Network" << "{";
	Network->write(fs);
	fs << "}";
	fs << "}";
}

/// <summary>
/// ���ļ���ȡ������
/// </summary>
/// <param name="filename">����·��</param>
void Classifier::Load(std::string filename)
{
	FileStorage fs(filename, FileStorage::READ);
	Load(fs);
	return;
}

/// <summary>
/// ��FileStorage��ȡ������
/// </summary>
/// <param name="fs">Ҫ��ȡ��FileStorage����</param>
void Classifier::Load(cv::FileStorage &fs)
{
	FileNode node = fs[(NetName.empty() ? "Classifier" : NetName)];
	ClassNumber = (int)node["ClassNumber"];
	FeatureNumber = (int)node["FeatureNumber"];
	Network = ml::ANN_MLP::create();
	Network->read(node["Network"]);
	return;
}


void Classifier::readSamples()
{
	for (int i = 0; i < ClassNumber; i++)
	{
		string sub_path = this->samples_root + "//" + std::to_string(i + 1) + "//";
		vector<string> images_path;
		scanImages(sub_path, images_path);
		vector<Sample> samples;
		for (int j = 0; j < images_path.size(); j++)
		{
			Sample sample;
			sample.image = imread(images_path[j], IMREAD_ANYCOLOR | IMREAD_ANYDEPTH);
			sample.label = i;	
			samples.push_back(sample);
		}
		samples_libary.push_back(samples);
	}
}

void Classifier::scanImages(std::string path,std::vector<string> &files)
{
	long long hFile = 0;
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//�����Ŀ¼,����֮
			//�������,�����б�
			if (!(fileinfo.attrib &  _A_SUBDIR))
			{
				//cout << "Found: " << fileinfo.name << endl; 
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}

bool default_judge_func(std::vector<std::pair<float, int>>& result, int label)
{
	if (result.empty())
	{
		throw(exception("����1Ϊ��"));
	}
	int max_index = 0;
	float max_value = result[0].first;
	for (int i = 1; i < result.size(); i++)
	{
		if (result[i].first > max_value)
		{
			max_value = result[i].first;
			max_index = i;
		}
	}
	return result[max_index].second == label;
}
