
// mfc_battery.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// Cmfc_batteryApp: 
// �йش����ʵ�֣������ mfc_battery.cpp
//

class Cmfc_batteryApp : public CWinApp
{
public:
	Cmfc_batteryApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern Cmfc_batteryApp theApp;