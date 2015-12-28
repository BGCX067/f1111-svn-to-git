// AppProfile.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "AppProfile.h"
#include "MainFrm.h"

#include "Profile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAppProfileApp

BEGIN_MESSAGE_MAP(CAppProfileApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CAppProfileApp::OnAppAbout)
END_MESSAGE_MAP()


// CAppProfileApp construction

CAppProfileApp::CAppProfileApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CAppProfileApp object

CAppProfileApp theApp;


// CAppProfileApp initialization

BOOL CAppProfileApp::InitInstance()
{
	CWinApp::InitInstance();


	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object
	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	// create and load the frame with its resources
	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);

	// The one and only window has been initialized, so show and update it
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand


	// We gotta initializxe COM before we can use CProfile 
	// (we call CoUnitialize in ExitInstance())
	CoInitialize(NULL);

	CProfile profile;
	profile.Load("");

	// we're going to turn autosave off so we can play without destroying the 
	// original contents of the file
	profile.SetAutoSave(false);

	AfxMessageBox("The next two dialog boxes will show contents of the list using built-in debugging functions.");
	profile.ShowContentsByRange();
	profile.ShowContentsBySection("General");

	AfxMessageBox("Now, we're going to delete the General section and re-display it's contents.");
	profile.RemoveSection("General");
	profile.ShowContentsBySection("General");

	AfxMessageBox("Now, we're going to add a new key for the general section and re-display it's contents.");
	profile.SetValue("General", "Seagull", "tastes like chicken");
	profile.ShowContentsBySection("General");

	return TRUE;
}


// CAppProfileApp message handlers




// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// App command to run the dialog
void CAppProfileApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CAppProfileApp message handlers


int CAppProfileApp::ExitInstance()
{
	// we can unitialize COM now.
	CoUninitialize();

	return CWinApp::ExitInstance();
}
