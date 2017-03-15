
// mfc_batteryDlg.h : ͷ�ļ�
//

#pragma once
#include <opencv.hpp>
#include <thread>
#include "CvvImage.h"
using namespace std;
using namespace cv;

// Cmfc_batteryDlg �Ի���
class Cmfc_batteryDlg : public CDialogEx
{
// ����
public:
	Cmfc_batteryDlg(CWnd* pParent = NULL);	// ��׼���캯��
	int used_cam = 0; // ʹ�������λ
// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFC_BATTERY_DIALOG };
#endif
	int battery_count;
	long long start_time;
	bool is_colorBattery;
	bool capture_running;
	// ���ڲ�����ı���
	int frame_cur_count; // ��ǰ֡
	int frame_max_count; // ������֡
	float naked_pit_rate;
	HANDLE* camera_handle;
	CComboBox* trigger_mode;
	volatile int sides_processed; // ��ر�����Ѵ��������������Ϊ0ʱ�����Խ�����һ����ؼ�⣻������Ϊ1��2ʱ���������е�ر���δ�����ϣ�������Ϊ3ʱ��������ر�����Ѿ������ϣ�������������
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
	CMenu *dlg_menu; // �Ի���˵�
	CFont font;
	CFont font_s;
	HDC hDC[3];
	CRect rect[3];
	CvvImage cimg;
	
	
// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	void InitHDC();
	void DrawPicToHDC(Mat mat, UINT ID);
	void SetFontFormat(UINT ID);
	void SetFontSFormat(UINT ID);
	void DefaultScreen();
	void showStatic(CStatic* st, const char* str);
	void showStatic(UINT nID, const char* str);
	void ChangeCamera(int no);
	afx_msg
		
	void ChangeToCam1();
	afx_msg void ChangeToCam2();
	afx_msg void ChangeToCam3();
	afx_msg void ChangeToCam4();
	afx_msg void OnBnClickedRaCam1();
	afx_msg void OnBnClickedRaCam2();
	afx_msg void OnBnClickedRaCam3();
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedRecam();
	afx_msg void OnCbnSelchangeTrimode();
	afx_msg void OnBnClickedSwping();
	afx_msg void OnBnClickedSinglecam();
	afx_msg void OnBnClickedMuiltcam();
	afx_msg void OnCbnSelchangeDft2rt();
};
