
// mfc_batteryDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "mfc_battery.h"
#include "mfc_batteryDlg.h"
#include "afxdialogex.h"
#include "xiApi.h"
#pragma comment(lib,"m3apiX64.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

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
}

void Cmfc_batteryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

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
	font.CreatePointFont(240, _T("Arial"));
	SetFontFormat(IDC_DFT1);
	SetFontFormat(IDC_DFT2);
	SetFontFormat(IDC_DFT3);
	SetFontFormat(IDC_DFT4);
	SetFontFormat(IDC_DFT5);
	SetFontFormat(IDC_DFT6);
	SetFontFormat(IDC_DFT7);
	SetFontFormat(IDC_DFT8);
	SetFontFormat(IDC_BTY_SPD);
	SetFontFormat(IDC_BTY_CNT);
	// Start thread:
	thread t1(onlineCaptureImage, this);
	t1.detach();
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



void onlineCaptureImage(Cmfc_batteryDlg *dlg)
{
	XI_IMG image; // Xi camera structure
	memset(&image, 0, sizeof(image)); // memory set all 0
	image.size = sizeof(XI_IMG);

	// Camera's Handle
	HANDLE xiH = NULL;
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
	stat = xiSetParamInt(xiH, XI_PRM_EXPOSURE, 10000);
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
	stat = xiStartAcquisition(xiH);
	if (stat != XI_OK)
	{
		dlg->MessageBoxW(_T("Error 205:\nAcquest image failed,please contact to sofeware supplier."));
		return;
	}
	Mat splash = imread("moe2.jpeg");
	dlg->DrawPicToHDC(splash, 0);
	dlg->DrawPicToHDC(splash, 1);
	dlg->DrawPicToHDC(splash, 2);
	dlg->showStatic(IDC_STATBAR, "Camera Online.");
	dlg->start_time = getTickCount();
	int current_speed = 0;
	CStatic* st_count = (CStatic*)dlg->GetDlgItem(IDC_BTY_CNT);
	CStatic* st_speed = (CStatic*)dlg->GetDlgItem(IDC_BTY_SPD);
	dlg->InitHDC();
	for (;;)
	{
		stat = xiGetImage(xiH, 1000, &image);
		if (stat != XI_OK)
		{
			dlg->MessageBoxW(_T("Error 206:\nCamera lose connection!\n"));
			dlg->showStatic(IDC_STATBAR, to_string(stat).c_str());
			return;
		}
		Mat img(Size(image.width, image.height), CV_8UC3);
		memcpy(img.data, image.bp, image.width*image.height * 3);
		dlg->battery_count++;
		current_speed = dlg->battery_count / ((getTickCount() - dlg->start_time) / getTickFrequency());
		dlg->showStatic(st_count, string("Battery No. " + to_string(dlg->battery_count)).c_str());
		//dlg->showStatic(st_speed, string("Current Speed. " + to_string(current_speed)).c_str());
		dlg->showStatic(st_speed, string("Current Speed. " + to_string(current_speed) + " frame per-second.").c_str());
		// ���Ƶ���Ӧ�Ŀؼ�
		switch (dlg->used_cam)
		{
		case 1:
			dlg->DrawPicToHDC(img, 0);
			break;
		case 2:
			dlg->DrawPicToHDC(img, 1);
			break;
		case 4:
			dlg->DrawPicToHDC(img, 2);
			break;
		default:
			break;
		}
	}

}



void Cmfc_batteryDlg::InitHDC()
{
	int uid[] = { IDC_SHOW1,IDC_SHOW2,IDC_SHOW3 };
	for (int i = 0; i < 3; i++)
	{
		CDC *pDC = GetDlgItem(uid[i])->GetDC();
		hDC[i] = pDC->GetSafeHdc();
		GetDlgItem(uid[i])->GetClientRect(&rect[i]);
	}
}

void Cmfc_batteryDlg::DrawPicToHDC(Mat mat, UINT ID)
{
	/*CDC *pDC = GetDlgItem(ID)->GetDC();
	HDC hDC = pDC->GetSafeHdc();
	CRect rect;
	GetDlgItem(ID)->GetClientRect(&rect);
	CvvImage cimg;*/
	cimg.CopyOf(mat); // ����ͼƬ
	cimg.DrawToHDC(hDC[ID], &rect[ID]); // ��ͼƬ���Ƶ���ʾ�ؼ���ָ��������
	//ReleaseDC(pDC);
}

void Cmfc_batteryDlg::SetFontFormat(UINT ID)
{
	CStatic* st = (CStatic*)(GetDlgItem(ID));
	st->SetFont(&font);
}

void Cmfc_batteryDlg::DefaultScreen()
{
	Mat splash = imread("moe2.jpeg");
	DrawPicToHDC(splash, 0);
	DrawPicToHDC(splash, 1);
	DrawPicToHDC(splash, 2);
}

void Cmfc_batteryDlg::ChangeToCam1()
{
	// TODO: �ڴ���������������
	this->used_cam = 1;
	DefaultScreen();
}


void Cmfc_batteryDlg::ChangeToCam2()
{
	// TODO: �ڴ���������������
	this->used_cam = 1;
	DefaultScreen();

}


void Cmfc_batteryDlg::ChangeToCam3()
{
	// TODO: �ڴ���������������
	this->used_cam = 2;
	DefaultScreen();
}


void Cmfc_batteryDlg::ChangeToCam4()
{
	// TODO: �ڴ���������������
	this->used_cam = 4;
	DefaultScreen();
}

void Cmfc_batteryDlg::showStatic(CStatic* st, const char* str)
{
	USES_CONVERSION;
	st->SetWindowTextW(A2W(str));
}

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
	this->used_cam = 1;
	DefaultScreen();
}


void Cmfc_batteryDlg::OnBnClickedRaCam2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	this->used_cam = 2;
	DefaultScreen();
}


void Cmfc_batteryDlg::OnBnClickedRaCam3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	this->used_cam = 4;
	DefaultScreen();
}
