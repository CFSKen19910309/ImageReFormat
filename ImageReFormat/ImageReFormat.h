
// ImageReFormat.h : PROJECT_NAME ���ε{�����D�n���Y��
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�� PCH �]�t���ɮ׫e���]�t 'stdafx.h'"
#endif

#include "resource.h"		// �D�n�Ÿ�


// CImageReFormatApp:
// �аѾ\��@�����O�� ImageReFormat.cpp
//

class CImageReFormatApp : public CWinApp
{
public:
	CImageReFormatApp();

// �мg
public:
	virtual BOOL InitInstance();

// �{���X��@

	DECLARE_MESSAGE_MAP()
};

extern CImageReFormatApp theApp;