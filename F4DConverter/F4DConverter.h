
// F4DConverter.h : F4DConverter ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"       // �� ��ȣ�Դϴ�.


// CF4DConverterApp:
// �� Ŭ������ ������ ���ؼ��� F4DConverter.cpp�� �����Ͻʽÿ�.
//

class CF4DConverterApp : public CWinApp
{
public:
	CF4DConverterApp();


// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CF4DConverterApp theApp;
