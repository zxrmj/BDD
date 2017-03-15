
// mfc_batteryDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "mfc_battery.h"
#include "mfc_batteryDlg.h"
#include "afxdialogex.h"
#include "xiApi.h"
#include "Classifier.h"
#include "ImageAlgorithm.h"
#include "BatteryTopDectector.h"
#include <mutex>
#pragma comment(lib,"m3apiX64.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
HANDLE xiH = NULL;
void onlineCaptureImage(Cmfc_batteryDlg *dlg);

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// Cmfc_batteryDlg �Ի���



Cmfc_batteryDlg::Cmfc_batteryDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MFC_BATTERY_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	battery_count = 0;
	start_time = 0;
	is_colorBattery = true;
	capture_running = false;
	camera_handle = nullptr;
	trigger_mode = 0;
	sides_processed = 3;
	naked_pit_rate = 0.75;
}

void Cmfc_batteryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

// ��Ϣӳ��
BEGIN_MESSAGE_MAP(Cmfc_batteryDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(ID_32771, &Cmfc_batteryDlg::ChangeToCam1)
	ON_COMMAND(ID_32772, &Cmfc_batteryDlg::ChangeToCam2)
	ON_COMMAND(ID_32773, &Cmfc_batteryDlg::ChangeToCam3)
	ON_COMMAND(ID_32774, &Cmfc_batteryDlg::ChangeToCam4)
	ON_BN_CLICKED(IDC_RA_CAM1, &Cmfc_batteryDlg::OnBnClickedRaCam1)
	ON_BN_CLICKED(IDC_RA_CAM2, &Cmfc_batteryDlg::OnBnClickedRaCam2)
	ON_BN_CLICKED(IDC_RA_CAM3, &Cmfc_batteryDlg::OnBnClickedRaCam3)
	ON_BN_CLICKED(IDC_CHECK1, &Cmfc_batteryDlg::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_RECAM, &Cmfc_batteryDlg::OnBnClickedRecam)
	ON_BN_CLICKED(IDC_RECAM, &Cmfc_batteryDlg::OnBnClickedRecam)
	ON_CBN_SELCHANGE(IDC_TRIMODE, &Cmfc_batteryDlg::OnCbnSelchangeTrimode)
	ON_BN_CLICKED(IDC_SWPING, &Cmfc_batteryDlg::OnBnClickedSwping)
	ON_BN_CLICKED(IDC_SINGLECAM, &Cmfc_batteryDlg::OnBnClickedSinglecam)
	ON_BN_CLICKED(IDC_MUILTCAM, &Cmfc_batteryDlg::OnBnClickedMuiltcam)
	ON_CBN_SELCHANGE(IDC_DFT2RT, &Cmfc_batteryDlg::OnCbnSelchangeDft2rt)
END_MESSAGE_MAP()


// Cmfc_batteryDlg ��Ϣ�������

BOOL Cmfc_batteryDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	font.CreatePointFont(240, _T("Arial")); // ��ͨ����
	font_s.CreatePointFont(160, _T("Arial")); // С������
	SetFontSFormat(IDC_DFT1);
	SetFontSFormat(IDC_DFT2);
	SetFontSFormat(IDC_DFT3);
	SetFontSFormat(IDC_DFT4);
	SetFontSFormat(IDC_DFT5);
	SetFontSFormat(IDC_DFT6);
	SetFontSFormat(IDC_DFT7);
	SetFontSFormat(IDC_DFT8);
	SetFontFormat(IDC_BTY_SPD);
	SetFontFormat(IDC_BTY_CNT);

	trigger_mode = (CComboBox*)GetDlgItem(IDC_TRIMODE);
	trigger_mode->SetCurSel(0);

	DWORD numbersOfCamera = 0;
	xiGetNumberDevices(&numbersOfCamera);
	showStatic(IDC_STATBAR, ("�������: " + to_string(numbersOfCamera)).c_str());

	
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void Cmfc_batteryDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void Cmfc_batteryDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR Cmfc_batteryDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// ����߳�0������ز���
void thread_camera0(Cmfc_batteryDlg *dlg, HANDLE handle,bool is_trigger)
{
	if (!handle)
	{
		dlg->MessageBox(_T("���0δ����"));
		return;
	}
	XI_IMG image; 
	memset(&image, 0, sizeof(image)); 
	image.size = sizeof(XI_IMG);

	// ������1������Ѱ�Ĥ��ػ���ȱ��
	Classifier classifier_1;
	classifier_1.Load("net1.xml");
	classifier_1.ExtractFeatureFunction = feature::extractColorFeature;
	// ������2�����δ��Ĥ��ذ���ȱ��
	Classifier classifier_2;
	classifier_2.Load("net2.xml");
	classifier_2.ExtractFeatureFunction = feature::extractNakedFeature;

	// ������2-2 ��δ��Ĥ��ذ���
	Ptr<ml::ANN_MLP> network22;
	network22->create();
	network22->load("network-roundside-local0315.xml");

	// ���ڲ�����֡�����ı���
	int &frame_cur_count = dlg->frame_cur_count;
	int &frame_max_count = dlg->frame_max_count;
	
	// ����ģʽ�£���Ҫ��������4֡���Ǵ���ģʽ����Ҫ����1֡
	if (is_trigger)
	{
		frame_max_count = 4;
	}
	else
	{
		frame_max_count = 1;
	}

	int color_battery_result = -1; // ˵���Ѱ�Ĥ���ȱ�����ڵڼ�֡
	int naked_battery_result = -1; // ˵��δ��Ĥ���ȱ�����ڵڼ�֡
	Mat battery_image_buffer; // ����ȱ��ͼƬ

	TickMeter tm; // ��ʱ����ͳ�ƴ���ǰ���������ʱ��
	// ��ر�ż���

	dlg->battery_count = 0;

	XI_RETURN stat = XI_OK;
	for (;;)
	{
		// ���3̨���δ��׼��������������ȴ�
		if (is_trigger && dlg->sides_processed)
		{
			Sleep(20);
			continue;
		}
		else
		{
			// �Ѱ�Ĥ���
			if (dlg->is_colorBattery)
			{
				Mat img;

				for (frame_cur_count = 0; frame_cur_count < frame_max_count; frame_cur_count++)
				{
					stat = xiGetImage(handle, 1000, &image);
					// ��ȡ��ͼƬ
					if (stat == XI_OK)
					{
						img.create(image.height, image.width, CV_8UC3);
						memcpy(img.data, image.bp, image.width*image.height * 3);
						
						Mat battery;
						region::detectColourBattery(img, battery); // ��ض�λ�㷨
						// ��λ�����
						if (battery.data)
						{
							// �ⲿ�ִ�����ִ�з���
							vector<pair<float, int>> result; // �洢���������صĽ��
							Mat feat_vec = classifier_1.ExtractFeatureFunction(battery); // ��ȡ��������
							classifier_1.Predict(feat_vec, result, RESULT_ORDER::ORDER_NORMAL); // ִ�з���

							 // �ⲿ�ִ������ڽ�������ʾ�����Ԫ�����
							ostringstream ss;
							ss << "[";
							for (int k = 0; k < result.size(); k++)
							{
								ss << result[k].second << ":" << result[k].first << "\t";
							}
							ss << "]";
							string str = ss.str();
							dlg->showStatic(IDC_DFT8, str.c_str()); // �����Խ�����������

							// �ⲿ�ִ����жϷ�����
							if (result[0].first > 0.75)
							{
								color_battery_result = frame_cur_count; // ��ǰ֡��ȱ��
								battery_image_buffer = img;
							}

						}
						else
						{
							dlg->showStatic(IDC_DFT1, "�޵��");
						}
					}
					// ��ȡͼƬ��ʱ
					else if (stat == XI_TIMEOUT)
					{
						dlg->showStatic(IDC_DFT1, "��ʱ");
					}
				} // for loop end

				if (color_battery_result > -1) // ������ֵ��Ŵ���-1��˵������ȱ��
				{
					dlg->showStatic(IDC_DFT1, "���滮��ȱ��"); // ��ʾȱ��
					dlg->DrawPicToHDC(battery_image_buffer, 0);
				}
				else
				{
					dlg->showStatic(IDC_DFT1, "����δ��ȱ��");
					dlg->DrawPicToHDC(img, 0); // ����ȱ�ݣ���ʾ���һ֡�ĵ��ͼ��
				}
			}
			// δ��Ĥ���
			else
			{
				Mat img;

				for (frame_cur_count = 0; frame_cur_count < frame_max_count; frame_cur_count++)
				{
					stat = xiGetImage(handle, 1000, &image);
					// ��ȡ��ͼƬ
					if (stat == XI_OK)
					{
						img.create(image.height, image.width, CV_8UC3);
						memcpy(img.data, image.bp, image.width*image.height * 3);
						cvtColor(img, img, CV_BGR2GRAY);
						Mat battery;
						Point ru;
						region::detectNakedBattery(img, battery, ru);// ��ض�λ�㷨
																   // ��λ�����
						if (battery.data)
						{
							if (battery.channels() == 3)
								cvtColor(battery, battery, CV_BGR2GRAY);
							int _step = battery.rows / 12;
							auto crop_battery = [](Mat src, int step) -> vector<Mat> {

								vector<Mat> v;
								for (int i = 1; i < 11; i++)
								{
									Mat sub_image = src(Rect(0, i*step, src.cols, step));
									v.push_back(sub_image);
								}
								return v;
							};
							vector<Mat> sub_mat = crop_battery(battery, _step);
							int defect_local = -1;
							float defect_max = -1;
							for (int i = 0; i < sub_mat.size(); i++)
							{
								Mat feat_vec = feature::extractNakedLocalFeature(battery);
								Mat response;
								network22->predict(feat_vec, response);
								if (response.at<float>(0, 1) > 0.75 && response.at<float>(0, 1) > defect_max)
								{
									defect_local = i + 1;
									defect_max = response.at<float>(0, 1);
								}
							}

							ostringstream ss;
							ss << defect_max << endl;
							string str = ss.str();
							if (defect_local != -1)
								rectangle(img, Rect(ru.x + 30, ru.y + defect_local*_step, battery.cols - 30, _step), Scalar(0, 0, 255), 2);
							dlg->showStatic(IDC_DFT8, str.c_str()); // �����Խ�����������

							// �ⲿ�ִ����жϷ�����
							if (defect_local != -1)
							{
								naked_battery_result = frame_cur_count; // ��ǰ֡��ȱ��
								battery_image_buffer = img;
							}
						}
						else
						{
							dlg->showStatic(IDC_DFT1, "�޵��");
						}
					}
					// ��ȡͼƬ��ʱ
					else if (stat == XI_TIMEOUT)
					{
						dlg->showStatic(IDC_DFT1, "��ʱ");
					}
				} // for loop end

				if (naked_battery_result > -1) // ������ֵ��Ŵ���-1��˵������ȱ��
				{
					dlg->showStatic(IDC_DFT1, "���氼��ȱ��"); // ��ʾȱ��
					dlg->DrawPicToHDC(battery_image_buffer, 0);
				}
				else
				{
					dlg->showStatic(IDC_DFT1, "����δ��ȱ��");
					dlg->DrawPicToHDC(img, 0); // ����ȱ�ݣ���ʾ���һ֡�ĵ��ͼ��
				}
			}

			// �Ըõ�ر����Ѵ������
			mutex locker;
			locker.lock();
			dlg->sides_processed += 1; // ��ر����Ѵ������+1
			if (dlg->sides_processed == 3) // ��������Դ���
			{
				dlg->sides_processed = 0; // ����Ϊ0����ʾ������һѭ���󽫿�ʼ�����µĵ��
				dlg->battery_count = 0;
			}

			locker.unlock();
		}
	}
}

// ����߳�1������ض���
void thread_camera1(Cmfc_batteryDlg *dlg, HANDLE handle, bool is_trigger)
{
	if (!handle)
	{
		dlg->MessageBox(_T("���1δ����"));
		return;
	}
	XI_IMG image;
	memset(&image, 0, sizeof(image));
	image.size = sizeof(XI_IMG);

	// ������3 : �������
	Classifier classifier_3;
	classifier_3.Load("net3.xml");
	classifier_3.ExtractFeatureFunction = feature::extractRustyFeature;
	// ������4 : ��������
	Classifier classifier_4;
	classifier_4.Load("net4.xml");
	classifier_4.ExtractFeatureFunction = feature::extractUpSidePitFeature;

	// ���涨λ��������
	/*BatteryTopDetector detector;
	Mat detector_template = imread("temp.bmp", IMREAD_GRAYSCALE);
	detector.SetTemplate(detector_template);*/

	// ȱ�ݱ�ʶ��
	bool top_side_result_rusty = false;
	bool top_side_result_pit = false;

	XI_RETURN stat = XI_OK;
	for (;;)
	{
		if (is_trigger && dlg->sides_processed)
		{
			Sleep(20);
			continue;
		}
		else
		{
			Mat img;
			stat = xiGetImage(handle, 1000, &image);

			if (stat == XI_OK)
			{
				img.create(image.height, image.width, CV_8UC3);
				memcpy(img.data, image.bp, image.width*image.height * 3);
				cvtColor(img, img, CV_BGR2GRAY);
				Mat battery;
				region::detectTopSideBattery(img, battery); // ��ض�λ�㷨
				// ��λ�����
				if (battery.data)
				{
					// �ⲿ�ִ�����ִ�з���
					vector<pair<float, int>> result; // �洢���������صĽ��
					Mat feat_vec = classifier_3.ExtractFeatureFunction(battery); // ��ȡ��������
					classifier_3.Predict(feat_vec, result, RESULT_ORDER::ORDER_NORMAL); // ִ�з���
					if (result[1].first > 0.75)
					{
						top_side_result_rusty = true;
					}

					feat_vec = classifier_4.ExtractFeatureFunction(battery);
					classifier_4.Predict(feat_vec, result, RESULT_ORDER::ORDER_NORMAL);
					if (result[1].first > 0.75)
					{
						top_side_result_pit = true;
					}

					string str_result;
					// ���ܽ��
					if (top_side_result_pit || top_side_result_rusty)
					{
						str_result.append(top_side_result_pit ? "����ȱ��," : "");
						str_result.append(top_side_result_rusty ? "���ȱ��," : "");
					}
					else
					{
						str_result = "����δ��ȱ��";
					}

					dlg->showStatic(IDC_DFT2, str_result.c_str());
					dlg->DrawPicToHDC(img, 1);
				}
				else
				{
					dlg->showStatic(IDC_DFT1, "�޵��");
				}
			}
			// ��ȡͼƬ��ʱ
			else if (stat == XI_TIMEOUT)
			{
				dlg->showStatic(IDC_DFT1, "��ʱ");
			}

			// �Ըõ�ر����Ѵ������
			mutex locker;
			locker.lock();
			dlg->sides_processed += 1; // ��ر����Ѵ������+1
			if (dlg->sides_processed == 3) // ��������Դ���
			{
				dlg->sides_processed = 0; // ����Ϊ0����ʾ������һѭ���󽫿�ʼ�����µĵ��
				dlg->battery_count = 0;
			}

			locker.unlock();
		}
	}
}

void thread_camera2(Cmfc_batteryDlg *dlg, HANDLE handle, bool is_trigger)
{
	if (!handle)
	{
		dlg->MessageBox(_T("���2δ����"));
		return;
	}
	XI_IMG image;
	memset(&image, 0, sizeof(image));
	image.size = sizeof(XI_IMG);
	Classifier classifer_5;

	XI_RETURN stat = XI_OK;
	for (;;)
	{
		if (is_trigger && dlg->sides_processed)
		{
			Sleep(20);
			continue;
		}
		else
		{
			Mat img;
			stat = xiGetImage(handle, 1000, &image);
			if (stat == XI_OK)
			{
				img.create(image.height, image.width, CV_8UC3);
				memcpy(img.data, image.bp, image.width*image.height * 3);
				if (img.data) {
					dlg->DrawPicToHDC(img, 2); 
				}
			}
			else if (stat == XI_TIMEOUT)
			{
				dlg->showStatic(IDC_DFT3, "�޵��");
			}
			else
			{
				dlg->showStatic(IDC_DFT3, "��ͼ��");
			}

		}
	}
}

// �����ͬʱ����
void onlineMultiCaptureImage(Cmfc_batteryDlg* dlg)
{
	Mat splash = imread("moe2.jpeg"); // ������ڴ���ͼ��

	// ���ƴ���ͼƬ
	dlg->InitHDC();
	for (int i = 0; i < 3; i++)
		dlg->DrawPicToHDC(splash, i);

	// �������״̬
	XI_RETURN stat = XI_OK;
	// ��ȡ�豸����
	DWORD DevNum = 0;
	stat = xiGetNumberDevices(&DevNum);
	// �������������Ч״̬�򷵻��豸������Ϊ3������ʾʧ��
	if (stat != XI_OK || DevNum != 3)
	{
		dlg->MessageBoxW(_T("Error:\n�豸���ϻ��豸������������ȷ��������"));
	}
	HANDLE handles[3] = { 0 }; // ������
	XI_IMG_FORMAT formats[3] = { XI_RGB24,XI_RGB24,XI_RAW8 }; // ���ɫ������
	int _trigger = dlg->trigger_mode->GetCurSel();

	int camidx[] = { 1,0 };

	// ���ο����豸
	for (int _dev = 0; _dev < DevNum; _dev++)
	{
		// ��������豸
		stat = xiOpenDevice(camidx[_dev], &handles[_dev]);
		if (stat != XI_OK)
		{
			dlg->MessageBoxW(_T("Error:\n�޷��������"));
			return;
		}
		
		stat = xiSetParamInt(handles[_dev], XI_PRM_TRG_SOURCE, _trigger);
		if (stat != XI_OK)
		{
			dlg->MessageBoxW(_T("Error:\n�޷������������ģʽ"));
			return;
		}
		// �����ع���� 
		stat = xiSetParamInt(handles[_dev], XI_PRM_EXPOSURE, 1500);
		if (stat != XI_OK)
		{
			dlg->MessageBoxW(_T("Error:\n��������ع����ʧ��"));
			return;
		}
		// ����ͼ�����ݸ�ʽ
		stat = xiSetParamInt(handles[_dev], XI_PRM_IMAGE_DATA_FORMAT, formats[_dev]);
		if (stat != XI_OK)
		{
			dlg->MessageBoxW(_T("Error:\n����������ݸ�ʽʧ��"));
			return;
		}
		stat = xiStartAcquisition(handles[_dev]);
		if (stat != XI_OK)
		{
			dlg->MessageBoxW(_T("Error:\n�����ʼ��ȡͼƬʧ��"));
			return;
		}
		else
		{
			dlg->sides_processed -= 1; // �ñ�����ʼ��Ϊ3��ÿ��һ�����׼���ú󣬼�����1���������׼���ú󣬼���Ϊ0����ʾ���Խ��е�һ�μ��
			switch (_dev)
			{
			case 0:
			{
				thread t1(thread_camera0, dlg, handles[0], _trigger);
				t1.detach();
				break;
			}
			case 1:
			{
				thread t2(thread_camera1, dlg, handles[1], _trigger);
				t2.detach();
				break;
			}
			case 2:
			{
				thread t3(thread_camera2, dlg, handles[2], _trigger);
				t3.detach();
				break;
			}
				
			default:
				break;
			}
		}
	}

	dlg->capture_running = true;
	for (;;)
		;
}

void onlineCaptureImage(Cmfc_batteryDlg *dlg)
{
	XI_IMG image; // Xi camera structure
	memset(&image, 0, sizeof(image)); // memory set all 0
	image.size = sizeof(XI_IMG);

	// Camera's Handle
	
	dlg->camera_handle = &xiH;
	XI_RETURN stat = XI_OK; // status of cam
	// get number of devices.
	DWORD dwNumberOfDevices = 0;
	stat = xiGetNumberDevices(&dwNumberOfDevices);
	if (stat != XI_OK)
	{
		dlg->MessageBoxW(_T("Error 201:\nNo camera driver detected,please reinstall or repair them."));
		return;
	}

	// open cam
	stat = xiOpenDevice(0, &xiH);
	if (stat != XI_OK)
	{
		dlg->MessageBoxW(_T("Error 202:\nConnect to camera failed,please insure camera(s) is(are) already linked to this PC."));
		return;
	}
	stat = xiSetParamInt(xiH, XI_PRM_EXPOSURE, 1500);
	if (stat != XI_OK)
	{
		dlg->MessageBoxW(_T("Error 203:\nSet exposure time failed,please contact to sofeware supplier."));
		return;
	}
	stat = xiSetParamInt(xiH, XI_PRM_IMAGE_DATA_FORMAT, XI_RGB24);
	if (stat != XI_OK)
	{
		dlg->MessageBoxW(_T("Error 204:\nSet data format failed,please contact to sofeware supplier."));
		return;
	}
#define TRIGGER
#ifdef TRIGGER
	int _trigger = dlg->trigger_mode->GetCurSel();
	stat = xiSetParamInt(xiH, XI_PRM_TRG_SOURCE, _trigger);
	if (stat != XI_OK)
	{
		dlg->MessageBoxW(_T("Error 204:\nSet data format failed,please contact to sofeware supplier."));
		return;
	}
#endif // TRIGGER
	stat = xiStartAcquisition(xiH);
	if (stat != XI_OK)
	{
		dlg->MessageBoxW(_T("Error 205:\nAcquest image failed,please contact to sofeware supplier."));
		return;
	}
	Mat splash = imread("moe2.jpeg");
	Mat splash_wt_signal = imread("moe3.jpeg");
	dlg->InitHDC();
	dlg->DrawPicToHDC(splash, 0);
	dlg->DrawPicToHDC(splash, 1);
	dlg->DrawPicToHDC(splash, 2);
	dlg->showStatic(IDC_STATBAR, "Camera Online.");
	dlg->start_time = getTickCount();
	int current_speed = 0;
	CStatic* st_count = (CStatic*)dlg->GetDlgItem(IDC_BTY_CNT);
	CStatic* st_speed = (CStatic*)dlg->GetDlgItem(IDC_BTY_SPD);
	dlg->capture_running = true;
	// ������1 : ��ɫ��Ĥ���
	Classifier classifier1;
	classifier1.Load("net1.xml");
	classifier1.ExtractFeatureFunction = feature::extractColorFeature;
	// ������2 : δ��Ĥ���
	Classifier classifier2(2, 6, 2, 24);
	classifier2.ExtractFeatureFunction = feature::extractNakedFeature;
	classifier2.Train("C://Users//Zengx//Documents//Visual Studio 2015//Projects//cv_battery//cv_ml_battery//white_light3");
	if (!classifier2.Network->isTrained())
		return;
	// ������3 : �������
	Classifier classifier3;
	classifier3.Load("topside-rutsy-0315.xml");
	classifier3.ExtractFeatureFunction = feature::extractRustyFeature;
	// ������4 : ��������
	Classifier classifier4;
	classifier4.Load("net4.xml");
	classifier4.ExtractFeatureFunction = feature::extractUpSidePitFeature;

	// ������2-2 ��δ��Ĥ��ذ���
	Classifier classifier22;
	classifier22.Load("network-roundside-local0315.xml");
	classifier22.ExtractFeatureFunction = feature::extractNakedLocalFeature;



	// ���涨λ��������
	BatteryTopDetector detector;
	Mat detector_template = imread("temp.bmp",IMREAD_GRAYSCALE);
	detector.SetTemplate(detector_template);

	int frame_count = 0;
	int frame_max_count = 0;
	if (_trigger)
	{
		frame_max_count = 3;
	}
	
	bool naked_battery_results = false;
	Mat naked_battery_image;
	dlg->battery_count = 0;
	for (;;)
	{
		stat = xiGetImage(xiH, 1000, &image);
		if (stat == XI_OK)
		{
			Mat img(Size(image.width, image.height), CV_8UC3);
			memcpy(img.data, image.bp, image.width*image.height * 3);
			// ���Ƶ���Ӧ�Ŀؼ�
			TickMeter tm;
			tm.start();
			switch (dlg->used_cam)
			{
			case 1:
			{
				if (dlg->is_colorBattery)
				{


					// legacies
					
					Mat battery;
					region::detectColourBattery(img, battery);
					if (battery.data)
					{
						vector<pair<float, int>> result;
						Mat feat_vec = classifier1.ExtractFeatureFunction(battery);
						classifier1.Predict(feat_vec, result, RESULT_ORDER::ORDER_DECENDING);
						if (result[0].second == 0)
						{
							dlg->showStatic(IDC_DFT1, "���滮��ȱ��");
						}
						else
						{
							dlg->showStatic(IDC_DFT1, "��ȱ��");
						}
					}
					else
					{
						dlg->showStatic(IDC_DFT1, "�޵��");
					}
					

					
				}
				else
				{
					Mat battery;
					Point ru;
					region::detectNakedBattery(img, battery,ru);
					
					if (battery.cols && battery.rows)
					{
						if (battery.channels() == 3)
							cvtColor(battery, battery, CV_BGR2GRAY);
						int _step = battery.rows / 12;
						auto crop_battery = [](Mat src,int step) -> vector<Mat> {
							
							vector<Mat> v;
							for (int i = 1; i < 11; i++)
							{
								Mat sub_image = src(Rect(0, i*step, src.cols, step));
								v.push_back(sub_image);
							}
							return v;
						};
						vector<Mat> sub_mat = crop_battery(battery, _step);
						int defect_local = -1;
						float defect_max = -1;
						for (int i = 0; i < sub_mat.size(); i++)
						{
							Mat feat_vec = feature::extractNakedLocalFeature(sub_mat[i]);
							Mat response;
							classifier22.Network->predict(feat_vec, response);
							if (response.at<float>(0, 1) > dlg->naked_pit_rate)
							{
								rectangle(img, Rect(ru.x + 30, ru.y + (i + 1)*_step, battery.cols - 60, _step), Scalar(0, 0, 255), 2);
								ostringstream ss;
								ss << response << endl;
								string str = ss.str();
								dlg->showStatic(IDC_DFT8, str.c_str());
								defect_local = 1;
							}
						}
#ifdef TRIGGER
						// ʹ�ô�����
						if (_trigger)
						{
							if (defect_local != -1)
							{
								naked_battery_results = frame_count;
								naked_battery_image = img;
							}
							frame_count++;
							if (frame_count > frame_max_count)
							{							
								if (naked_battery_results)
								{
									dlg->showStatic(IDC_DFT2, "���氼��ȱ��");
									dlg->DrawPicToHDC(naked_battery_image, 0);
								}
								else
								{
									dlg->showStatic(IDC_DFT2, "��ȱ��");
									dlg->DrawPicToHDC(img, 0);
								}
								naked_battery_results = false;
							}
							else
							{
								dlg->DrawPicToHDC(img, 0);
							}
						}
						// ��ʹ�ô�����
						else
						{
							if (defect_local != -1)
							{
								dlg->showStatic(IDC_DFT1, "���氼��ȱ��");
							}
							else
							{
								dlg->showStatic(IDC_DFT1, "��ȱ��");
							}
						}

						
#else
						if (result[0].second != 0)
						{
							dlg->showStatic(IDC_DFT2, "���氼��ȱ��");
						}
						else
						{
							dlg->showStatic(IDC_DFT2, "��ȱ��");
						}
#endif // TRIGGER
						
						
					}
					else
					{
						dlg->showStatic(IDC_DFT1, "�޵��");
					}
				}
				if (!_trigger)
				{
					dlg->DrawPicToHDC(img, 0);
				}
				
				
				break;
			}
			case 2:
			{
				Mat battery;
				region::detectTopSideBattery(img, battery);
				if (battery.data)
				{
					Mat response;
					Mat feat_vec = feature::extractRustyFeature(battery);
				/*	imshow("Battery", battery);
					waitKey();*/
					classifier3.Network->predict(feat_vec, response);
					ostringstream ss;
					ss << response << endl;
					string str = ss.str();
					dlg->showStatic(IDC_DFT8, str.c_str());
					if (response.at<float>(0, 1) > 0.75)
					{
						dlg->showStatic(IDC_DFT2, "�������ȱ��");
						dlg->showStatic(IDC_DFT5, ("PROP:" + to_string(response.at<float>(0, 1))).c_str());
					}
					else
					{

						dlg->showStatic(IDC_DFT2, "��ȱ��");
						dlg->showStatic(IDC_DFT5, ("PROP:" + to_string(response.at<float>(0, 0))).c_str());
					}
					/*result.clear();
					classifier4.Predict(feat_vec, result, RESULT_ORDER::ORDER_DECENDING);
					if (result[0].second != 0)
					{
						dlg->showStatic(IDC_DFT2, "���氼��ȱ��");
					}
					else
					{
						dlg->showStatic(IDC_DFT2, "��ȱ��");
					}*/

				}

				dlg->DrawPicToHDC(img, 1);
				break;
			}
				
			case 4:
				dlg->DrawPicToHDC(img, 2);
				break;
			default:
				break;
			}
			tm.stop();
			dlg->showStatic(st_count, string("Battery " + to_string(dlg->battery_count) + " Frame " + to_string(frame_count)).c_str());
			dlg->showStatic(st_speed, string("Current Speed: " + to_string(1.0 / tm.getTimeSec()) + "fps").c_str());
			if (frame_count > frame_max_count)
			{
				dlg->battery_count++;
				frame_count = 0;
			}
			
		}
		else if (stat == XI_TIMEOUT)
		{
			switch (dlg->used_cam)
			{
			case 1:
				dlg->DrawPicToHDC(splash_wt_signal, 0);
				break;
			case 2:
				dlg->DrawPicToHDC(splash_wt_signal, 1);
				break;
			case 4:
				dlg->DrawPicToHDC(splash_wt_signal, 2);
				break;
			default:
				break;
			}
			
		}
		else
		{
			dlg->MessageBoxW(_T("Error 206:\nCamera lose connection!\n"));
			dlg->showStatic(IDC_STATBAR, to_string(stat).c_str());
			dlg->capture_running = false;
			return;
		}
		
	}

}


// ��ʼ��HDC���������ͼ��Ļ���
void Cmfc_batteryDlg::InitHDC()
{
	// �ؼ�ID
	int UID[] = { 
		IDC_SHOW1,
		IDC_SHOW2,
		IDC_SHOW3 };
	for (int i = 0; i < 3; i++)
	{
		CDC *pDC = GetDlgItem(UID[i])->GetDC();
		hDC[i] = pDC->GetSafeHdc();
		GetDlgItem(UID[i])->GetClientRect(&rect[i]);
	}
}

void Cmfc_batteryDlg::DrawPicToHDC(Mat mat, UINT ID)
{
	cimg.CopyOf(mat);
	cimg.DrawToHDC(hDC[ID], &rect[ID]);
}

void Cmfc_batteryDlg::SetFontFormat(UINT ID)
{
	CStatic* st = (CStatic*)(GetDlgItem(ID));
	st->SetFont(&font);
}

void Cmfc_batteryDlg::SetFontSFormat(UINT ID)
{
	CStatic* st = (CStatic*)(GetDlgItem(ID));
	st->SetFont(&font_s);
}

void Cmfc_batteryDlg::DefaultScreen()
{
	Mat splash = imread("moe2.jpeg");
	DrawPicToHDC(splash, 0);
	DrawPicToHDC(splash, 1);
	DrawPicToHDC(splash, 2);
}

// �������
void Cmfc_batteryDlg::ChangeCamera(int no)
{
	this->used_cam = no;
	DefaultScreen();
}

void Cmfc_batteryDlg::ChangeToCam1()
{
	// TODO: �ڴ���������������
	ChangeCamera(1);
}


void Cmfc_batteryDlg::ChangeToCam2()
{
	// TODO: �ڴ���������������
	ChangeCamera(1);

}


void Cmfc_batteryDlg::ChangeToCam3()
{
	// TODO: �ڴ���������������
	ChangeCamera(2);
}


void Cmfc_batteryDlg::ChangeToCam4()
{
	// TODO: �ڴ���������������
	ChangeCamera(4);
}

// ���ľ�̬�ؼ�������
void Cmfc_batteryDlg::showStatic(CStatic* st, const char* str)
{
	USES_CONVERSION;
	st->SetWindowTextW(A2W(str));
}

// ���ľ�̬�ؼ�������
void Cmfc_batteryDlg::showStatic(UINT nID, const char * str)
{
	CStatic* st = (CStatic*)(this->GetDlgItem(nID));
	if (st != nullptr)
	{
		showStatic(st, str);
		return;
	}
}

void Cmfc_batteryDlg::OnBnClickedRaCam1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	ChangeCamera(1);
}


void Cmfc_batteryDlg::OnBnClickedRaCam2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	ChangeCamera(2);
}


void Cmfc_batteryDlg::OnBnClickedRaCam3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	ChangeCamera(4);
}

// ���ĵ�ǰ�Ƿ�Ϊ��Ĥ���
void Cmfc_batteryDlg::OnBnClickedCheck1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	is_colorBattery = !is_colorBattery;
	frame_cur_count = 0;
}

// ���³�ʼ�����
void Cmfc_batteryDlg::OnBnClickedRecam()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
		if (capture_running)
		{
			MessageBox(_T("����Ѿ���������"));
		}
		else
		{
			thread t1(onlineCaptureImage, this);
			t1.detach();
		}
}


void Cmfc_batteryDlg::OnCbnSelchangeTrimode()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (xiH != nullptr)
	{
#define TRIGGER
#ifdef TRIGGER
		XI_RETURN stat = xiSetParamInt(xiH, XI_PRM_TRG_SOURCE, trigger_mode->GetCurSel());
		if (stat != XI_OK)
		{
			this->MessageBoxW(_T("Error:\n ���ô���ģʽʧ�ܣ�������"));
			return;
		}
#endif // TRIGGER
	}
}


void Cmfc_batteryDlg::OnBnClickedSwping()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

}


void Cmfc_batteryDlg::OnBnClickedSinglecam()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	// Start thread:
	if (capture_running)
	{
		MessageBox(_T("�Ѿ���������"));
		return;
	}
	thread t1(onlineCaptureImage, this);
	t1.detach();
}


void Cmfc_batteryDlg::OnBnClickedMuiltcam()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (capture_running)
	{
		MessageBox(_T("�Ѿ���������"));
		return;
	}
	thread t1(onlineMultiCaptureImage, this);
	t1.detach();
}


void Cmfc_batteryDlg::OnCbnSelchangeDft2rt()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CComboBox* combo = (CComboBox*)GetDlgItem(IDC_DFT2RT);
	switch (combo->GetCurSel())
	{
	case 0:
		this->naked_pit_rate = 0.75;
		break;
	case 1:
		this->naked_pit_rate = 0.6;
		break;
	case 2:
		this->naked_pit_rate = 0.45;
		break;
	default:
		break;
	}
}
