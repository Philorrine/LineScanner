
// LineScanner.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CLineScannerApp:
// �йش����ʵ�֣������ LineScanner.cpp
//

class CLineScannerApp : public CWinApp
{
public:
	CLineScannerApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CLineScannerApp theApp;