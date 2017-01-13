
// mfc_batteryDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "mfc_battery.h"
#include "mfc_batteryDlg.h"
#include "afxdialogex.h"
#include "xiApi.h"
#include "Classifier.h"
#include "ImageAlgorithm.h"
#pragma comment(lib,"m3apiX64.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
HANDLE xiH = NULL;
void onlineCaptureImage(Cmfc_batteryDlg *dlg);

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// Cmfc_batteryDlg 对话框



Cmfc_batteryDlg::Cmfc_batteryDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MFC_BATTERY_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	battery_count = 0;
	start_time = 0;
	color_battery = true;
	capture_running = false;
	camera_handle = nullptr;
	trigger_mode = 0;
}

void Cmfc_batteryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

// 消息映射
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
END_MESSAGE_MAP()


// Cmfc_batteryDlg 消息处理程序

BOOL Cmfc_batteryDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	font.CreatePointFont(240, _T("Arial")); // 普通字体
	font_s.CreatePointFont(160, _T("Arial")); // 小号字体
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

	// Start thread:
	thread t1(onlineCaptureImage, this);
	t1.detach();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void Cmfc_batteryDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR Cmfc_batteryDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
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
	stat = xiSetParamInt(xiH, XI_PRM_EXPOSURE, 800);
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
	stat = xiSetParamInt(xiH, XI_PRM_TRG_SOURCE, dlg->trigger_mode->GetCurSel());
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
	// 分类器1 : 彩色包膜电池
	Classifier classifier1;
	classifier1.Load("net1.xml");
	classifier1.ExtractFeatureFunction = feature::extractColorFeature;
	// 分类器2 : 未包膜电池
	Classifier classifier2(3, 40, 2, 48);
	classifier2.ExtractFeatureFunction = feature::extractNakedFeature;
	classifier2.Train("C://Users//Zengx//Documents//Visual Studio 2015//Projects//cv_battery//cv_ml_battery");
	if (!classifier2.Network->isTrained())
		return;
	// 分类器3 : 侧面锈痕
	Classifier classifier3;
	classifier3.Load("net3.xml");
	classifier3.ExtractFeatureFunction = feature::extractRustyFeature;
	// 分类器4 : 顶部凹坑
	Classifier classifier4;
	classifier4.Load("net4.xml");
	classifier4.ExtractFeatureFunction = feature::extractUpSidePitFeature;

	for (;;)
	{
		stat = xiGetImage(xiH, 1000, &image);
		if (stat == XI_OK)
		{
			Mat img(Size(image.width, image.height), CV_8UC3);
			memcpy(img.data, image.bp, image.width*image.height * 3);
			dlg->battery_count++;
			//current_speed = dlg->battery_count / ((getTickCount() - dlg->start_time) / getTickFrequency());
			current_speed = 1000* ((getTickCount() - dlg->start_time) / getTickFrequency()) / dlg->battery_count;
			dlg->showStatic(st_count, string("Battery No. " + to_string(dlg->battery_count)).c_str());
			//dlg->showStatic(st_speed, string("Current Speed. " + to_string(current_speed) + " frame per-second.").c_str());
			//dlg->showStatic(st_speed, string("Current Speed. " + to_string(current_speed) + " millsecond per-frame.").c_str());
			// 绘制到对应的控件
			TickMeter tm;
			tm.start();
			switch (dlg->used_cam)
			{
			case 1:
			{
				if (dlg->color_battery)
				{
					// 彩色电池旧检测方式
					/*
					Mat battery;
					region::detectColourBattery(img, battery);
					if (battery.data)
					{
						vector<pair<float, int>> result;
						Mat feat_vec = classifier1.ExtractFeatureFunction(battery);
						classifier1.Predict(feat_vec, result, RESULT_ORDER::ORDER_DECENDING);
						if (result[0].second == 0)
						{
							dlg->showStatic(IDC_DFT1, "侧面划痕缺陷");
						}
						else
						{
							dlg->showStatic(IDC_DFT1, "无缺陷");
						}
					}
					*/

					
				}
				else
				{
					Mat battery;
					region::detectNakedBattery(img, battery);
					if (battery.cols && battery.rows)
					{
						vector<pair<float, int>> result;
						Mat feat_vec = classifier2.ExtractFeatureFunction(battery);
						classifier2.Predict(feat_vec, result, RESULT_ORDER::ORDER_DECENDING);
						if (result[0].second != 0)
						{
							dlg->showStatic(IDC_DFT2, "侧面凹坑缺陷");
						}
						else
						{
							dlg->showStatic(IDC_DFT2, "无缺陷");
						}
					}
				}
				dlg->DrawPicToHDC(img, 0);
				break;
			}
			case 2:
			{
				Mat battery;
				region::detectNakedBattery(img, battery);
				if (battery.data)
				{
					vector<pair<float, int>> result;
					Mat feat_vec = classifier2.ExtractFeatureFunction(battery);
					classifier3.Predict(feat_vec, result, RESULT_ORDER::ORDER_DECENDING);
					if (result[0].second != 0)
					{
						dlg->showStatic(IDC_DFT3, "顶面锈痕缺陷");
					}
					else
					{
						dlg->showStatic(IDC_DFT3, "无缺陷");
					}
					result.clear();
					classifier4.Predict(feat_vec, result, RESULT_ORDER::ORDER_DECENDING);
					if (result[0].second != 0)
					{
						dlg->showStatic(IDC_DFT4, "顶面凹坑缺陷");
					}
					else
					{
						dlg->showStatic(IDC_DFT4, "无缺陷");
					}

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
			dlg->showStatic(st_speed, string("Current Speed: " + to_string(tm.getTimeMilli()) + " millsecond this-frame.").c_str());
			
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
		
	/*	int64 time1 = getTickCount() - time0;
		double speed =1000* time1 / getTickFrequency();
		dlg->showStatic(st_speed, string("Current Speed. " + to_string(speed)).c_str());*/
	}

}


// 初始化HDC句柄，用于图像的绘制
void Cmfc_batteryDlg::InitHDC()
{
	// 控件ID
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

// 更换相机
void Cmfc_batteryDlg::ChangeCamera(int no)
{
	this->used_cam = no;
	DefaultScreen();
}

void Cmfc_batteryDlg::ChangeToCam1()
{
	// TODO: 在此添加命令处理程序代码
	ChangeCamera(1);
}


void Cmfc_batteryDlg::ChangeToCam2()
{
	// TODO: 在此添加命令处理程序代码
	ChangeCamera(1);

}


void Cmfc_batteryDlg::ChangeToCam3()
{
	// TODO: 在此添加命令处理程序代码
	ChangeCamera(2);
}


void Cmfc_batteryDlg::ChangeToCam4()
{
	// TODO: 在此添加命令处理程序代码
	ChangeCamera(4);
}

// 更改静态控件的文字
void Cmfc_batteryDlg::showStatic(CStatic* st, const char* str)
{
	USES_CONVERSION;
	st->SetWindowTextW(A2W(str));
}

// 更改静态控件的文字
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
	// TODO: 在此添加控件通知处理程序代码
	ChangeCamera(1);
}


void Cmfc_batteryDlg::OnBnClickedRaCam2()
{
	// TODO: 在此添加控件通知处理程序代码
	ChangeCamera(2);
}


void Cmfc_batteryDlg::OnBnClickedRaCam3()
{
	// TODO: 在此添加控件通知处理程序代码
	ChangeCamera(4);
}

// 更改当前是否为包膜电池
void Cmfc_batteryDlg::OnBnClickedCheck1()
{
	// TODO: 在此添加控件通知处理程序代码
	color_battery = !color_battery;
}

// 重新初始化相机
//void Cmfc_batteryDlg::OnBnClickedRecam()
//{
//	// TODO: 在此添加控件通知处理程序代码
//	if (capture_running)
//	{
//		MessageBox(_T("相机已经在运行了"));
//	}
//	else
//	{
//		thread t1(onlineCaptureImage, this);
//		t1.detach();
//	}
//}


void Cmfc_batteryDlg::OnBnClickedRecam()
{
	// TODO: 在此添加控件通知处理程序代码
		if (capture_running)
		{
			MessageBox(_T("相机已经在运行了"));
		}
		else
		{
			thread t1(onlineCaptureImage, this);
			t1.detach();
		}
}


void Cmfc_batteryDlg::OnCbnSelchangeTrimode()
{
	// TODO: 在此添加控件通知处理程序代码
	if (xiH != nullptr)
	{
#define TRIGGER
#ifdef TRIGGER
		XI_RETURN stat = xiSetParamInt(xiH, XI_PRM_TRG_SOURCE, trigger_mode->GetCurSel());
		if (stat != XI_OK)
		{
			this->MessageBoxW(_T("Error 204:\nSet data format failed,please contact to sofeware supplier."));
			return;
		}
#endif // TRIGGER
	}
}


void Cmfc_batteryDlg::OnBnClickedSwping()
{
	// TODO: 在此添加控件通知处理程序代码

}
