// AppProfile.h : main header file for the AppProfile application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CAppProfileApp:
// See AppProfile.cpp for the implementation of this class
//

class CAppProfileApp : public CWinApp
{
public:
	CAppProfileApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CAppProfileApp theApp;