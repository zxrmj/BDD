
// mfc_batteryDlg.h : ͷ�ļ�
//

#pragma once


// Cmfc_batteryDlg �Ի���
class Cmfc_batteryDlg : public CDialogEx
{
// ����
public:
	Cmfc_batteryDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFC_BATTERY_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
	CMenu *dlg_menu; // �Ի���˵�

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
};
