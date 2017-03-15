
// mfc_batteryDlg.h : 头文件
//

#pragma once
#include <opencv.hpp>
#include <thread>
#include "CvvImage.h"
using namespace std;
using namespace cv;

// Cmfc_batteryDlg 对话框
class Cmfc_batteryDlg : public CDialogEx
{
// 构造
public:
	Cmfc_batteryDlg(CWnd* pParent = NULL);	// 标准构造函数
	int used_cam = 0; // 使用相机方位
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFC_BATTERY_DIALOG };
#endif
	int battery_count;
	long long start_time;
	bool is_colorBattery;
	bool capture_running;
	// 用于侧面检测的变量
	int frame_cur_count; // 当前帧
	int frame_max_count; // 输出结果帧
	float naked_pit_rate;
	HANDLE* camera_handle;
	CComboBox* trigger_mode;
	volatile int sides_processed; // 电池表面的已处理计数，当计数为0时，可以进行下一个电池检测；当计数为1或2时，表明还有电池表面未检测完毕；当计数为3时，表明电池表面均已经检测完毕，计数重新置零
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	CMenu *dlg_menu; // 对话框菜单
	CFont font;
	CFont font_s;
	HDC hDC[3];
	CRect rect[3];
	CvvImage cimg;
	
	
// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
