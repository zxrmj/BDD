#include "Classifier.h"
#include <io.h>
using std::vector;
using std::string;
using std::cout;
using std::endl;
using namespace cv;

Classifier::Classifier()
{
	this->Network->create();
	this->ClassNumber = 0;
	this->FeatureNumber = 0;
	this->HiddenLayerNumber = 0;
	this->HiddenUnitsNumber = 0;
	this->ExtractFeatureFunction = nullptr;
}


Classifier::~Classifier()
{
}

void Classifier::Predict(cv::Mat src, std::vector<std::pair<float, int>>& output)
{
}
/// <summary>
/// ѵ������
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
	}
	bool flag_test = false, flag_show_details = false, flag_shuffle = false;
	if (test_flags & 1)
	{
		flag_test = true;
	}
	if (test_flags & 2)
	{
		cout << "test2" << endl;
	}
	if (test_flags & 4)
	{
		cout << "test4" << endl;
	}
	samples_root = path;
	readSamples();
}

void Classifier::Save(std::string filename)
{
}

void Classifier::Load(std::string filename)
{
}


void Classifier::readSamples()
{
	for (int i = 0; i < ClassNumber; i++)
	{

	}
}

void Classifier::scanImages(std::string path,std::vector<string> files)
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
