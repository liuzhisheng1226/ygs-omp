
// SARStudio.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CSARStudioApp:
// �йش����ʵ�֣������ SARStudio.cpp
//

class CSARStudioApp : public CWinApp
{
public:
	CSARStudioApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CSARStudioApp theApp;