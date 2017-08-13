
// ImageReFormatDlg.cpp : ��@��
//

#include "stdafx.h"
#include "ImageReFormat.h"
#include "ImageReFormatDlg.h"
#include "afxdialogex.h"

#include "opencv2\highgui.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int g_CurrentOrder;

CRITICAL_SECTION g_cs;
HANDLE g_ThreadHandle[D_MAX_THREAD_NUMBER];
unsigned int g_ThreadID[D_MAX_THREAD_NUMBER];
HANDLE g_ThreadEvent[D_MAX_THREAD_NUMBER];

int CImageReFormatDlg::GetNumberFiles()
{ return this->m_NumberFiles; }
int CImageReFormatDlg::GetIsThreadStatus()
{ return this->m_IsThreadStatus; }
CString CImageReFormatDlg::GetSrcFolderPath()
{ return this->m_SrcFolderPath; }
CString CImageReFormatDlg::GetDesFolderPath()
{ return this->m_DesFolderPath; }
CString CImageReFormatDlg::GetFileName(int Index)
{ return this->m_FilesName[Index]; }


unsigned int __stdcall DoReFormatImageFile(LPVOID Param)
{
	int tOrder = 0 ;
	CString tSrcFilePath;
	CString tFileNameWithoutExtension, tExtension;
	CString tReFormatType;
	CString tDesFilePath;
	CString tStrProgress;
	IplImage * tImage;
	while(true)
	{
		S_ThreadStruct * tThreadStruct = (S_ThreadStruct*) Param;
		CImageReFormatDlg *This = (CImageReFormatDlg*)tThreadStruct->sImageReFormatDlg;
		int tStatus = This->GetIsThreadStatus();

		switch(tStatus)
		{
		case D_THREAD_STATUS_RUN:
			//--------------------------------------------------//
			//�B�z�@�P�O����
			::EnterCriticalSection(&g_cs);
			tOrder = g_CurrentOrder;
			if(tOrder >= This->GetNumberFiles())
			{ 
				::LeaveCriticalSection(&g_cs);
				return TRUE;
			}
			g_CurrentOrder++;
			::LeaveCriticalSection(&g_cs);
			//--------------------------------------------------//
			//���o�}�ϸ��|
			tSrcFilePath.Format("%s\\%s", This->GetSrcFolderPath(), This->GetFileName(tOrder));
			//--------------------------------------------------//
			//���o�s�ϸ��|
			::ParseFileNameWithExtension(This->GetFileName(tOrder), tFileNameWithoutExtension, tExtension);
			This->m_Combo_TransformFormat.GetLBText(This->m_Combo_TransformFormat.GetCurSel(), tReFormatType);
			tDesFilePath.Format("%s\\%s.%s", This->GetDesFolderPath(), tFileNameWithoutExtension, tReFormatType);
			//--------------------------------------------------//
			//�}�ҹϤ��A�çP�_�O�_��ܻP�R����l�Ϥ�
			tImage = cvLoadImage(tSrcFilePath);

			if(This->m_Check_ShowImage.GetCheck() == (int)true)
			{
				int PictureID = tOrder % 4;
				This->ShowText(PictureID, tFileNameWithoutExtension);
				This->ShowImage(PictureID , tImage->height, tImage->width, tImage->widthStep, tImage->nChannels, (unsigned char*)tImage->imageDataOrigin);
			}
			if(This->m_Check_DeleteOriginalImage.GetCheck() == (int)true)
			{ ::DeleteFile(tSrcFilePath); }
			//--------------------------------------------------//
			cvSaveImage(tDesFilePath, tImage);
			cvReleaseImage(&tImage);
			This->m_ProgressCtrl.StepIt();
			//--------------------------------------------------//
			tStrProgress.Format("%d/%d", This->m_ProgressCtrl.GetPos(), This->GetNumberFiles());
			This->GetDlgItem(IDC_EDIT_PROGRESS)->SetWindowText(tStrProgress);
			break;
		case D_THREAD_STATUS_STOP:
			::SetEvent(g_ThreadEvent[tThreadStruct->sThreadOrder]);
			break;
		case D_THREAD_STATUS_IDLE:
			break;
		case D_THREAD_STATUS_BREAK:
			return true;
			break;
		default :
			break;
		}
	}
	return TRUE;

}

// �� App About �ϥ� CAboutDlg ��ܤ��

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ܤ�����
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �䴩

// �{���X��@
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CImageReFormatDlg ��ܤ��




CImageReFormatDlg::CImageReFormatDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CImageReFormatDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CImageReFormatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS, m_ProgressCtrl);
	DDX_Control(pDX, IDC_CHECK_SHOW_IMAGE, m_Check_ShowImage);
	DDX_Control(pDX, IDC_CHECK_DELETE_ORIGINAL_IMAGE, m_Check_DeleteOriginalImage);
	DDX_Control(pDX, IDC_COMBO_TRANSFORM_FOAMAT, m_Combo_TransformFormat);
	
}

BEGIN_MESSAGE_MAP(CImageReFormatDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_BROWSER_SOURCE, &CImageReFormatDlg::OnBnClickedButtonBrowserSource)
	ON_BN_CLICKED(IDC_BUTTON_BROWSER_DESTINATION, &CImageReFormatDlg::OnBnClickedButtonBrowserDestination)
	ON_BN_CLICKED(IDC_BUTTON_EXECUTE, &CImageReFormatDlg::OnBnClickedButtonExecute)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CImageReFormatDlg::OnBnClickedButtonStop)
	
	ON_EN_KILLFOCUS(IDC_EDIT_SOURCE_FOLDER_PATH, &CImageReFormatDlg::OnEnKillfocusEditSourceFolderPath)
	ON_EN_KILLFOCUS(IDC_EDIT_DESTINATION_FOLDER_PATH, &CImageReFormatDlg::OnEnKillfocusEditDestinationFolderPath)
	ON_BN_CLICKED(IDC_BUTTON_GET_NUMBER_OF_FILES, &CImageReFormatDlg::OnBnClickedButtonGetNumberOfFiles)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CImageReFormatDlg �T���B�z�`��

BOOL CImageReFormatDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �N [����...] �\���[�J�t�Υ\���C

	// IDM_ABOUTBOX �����b�t�ΩR�O�d�򤧤��C
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// �]�w����ܤ�����ϥܡC�����ε{�����D�������O��ܤ���ɡA
	// �ج[�|�۰ʱq�Ʀ��@�~
	SetIcon(m_hIcon, TRUE);			// �]�w�j�ϥ�
	SetIcon(m_hIcon, FALSE);		// �]�w�p�ϥ�

	// TODO: �b���[�J�B�~����l�]�w
	g_CurrentOrder = 0;
	//--------------------------------------------------//
	//�]�wComboBox of Transform Format�����e
	CComboBox * tComboBox = (CComboBox*)this->GetDlgItem(IDC_COMBO_TRANSFORM_FOAMAT);
	tComboBox->InsertString(0, "bmp");
	tComboBox->InsertString(1, "jpg");
	tComboBox->InsertString(2, "jpeg");
	tComboBox->InsertString(3, "png");
	tComboBox->SetCurSel(0);
	//--------------------------------------------------//
	this->CheckDlgButton(IDC_CHECK_SHOW_IMAGE, true);
	//--------------------------------------------------//
	//��l�ƶi�ױ�
	CProgressCtrl * tProgressCtrl = (CProgressCtrl*)this->GetDlgItem(IDC_PROGRESS);
	tProgressCtrl->SetRange(0,1);
	tProgressCtrl->SetPos(1);
	//--------------------------------------------------//
	//�����Handle��l��
	for(int i = 0 ; i < D_MAX_THREAD_NUMBER ; i ++)
	{
		g_ThreadHandle[i] = NULL;
		CString tStr;
		tStr.Format("%d", i);
		g_ThreadEvent[i] = ::CreateEvent(NULL, FALSE, FALSE, tStr);
		::ResetEvent(g_ThreadEvent[i]);
	}
	//--------------------------------------------------//
	InitializeCriticalSection(&g_cs);
	//--------------------------------------------------//
	g_CurrentOrder = 0;
	//--------------------------------------------------//
	this->m_IsThreadStatus = D_THREAD_STATUS_IDLE;
	for(int i = 0 ; i < D_MAX_THREAD_NUMBER ; i ++)
	{
		m_ThreadStruct[i].sImageReFormatDlg = this;
		m_ThreadStruct[i].sThreadOrder = i;
		g_ThreadHandle[i] = (HANDLE)::_beginthreadex(NULL, 0, &DoReFormatImageFile, &m_ThreadStruct[i], 0, &g_ThreadID[i]);
		::SetThreadPriority(g_ThreadHandle[i], THREAD_PRIORITY_LOWEST);
	}
	
	return TRUE;  // �Ǧ^ TRUE�A���D�z�ﱱ��]�w�J�I
}

void CImageReFormatDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �p�G�N�̤p�ƫ��s�[�J�z����ܤ���A�z�ݭn�U�C���{���X�A
// �H�Kø�s�ϥܡC���ϥΤ��/�˵��Ҧ��� MFC ���ε{���A
// �ج[�|�۰ʧ������@�~�C

void CImageReFormatDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ø�s���˸m���e

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// �N�ϥܸm����Τ�ݯx��
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �yø�ϥ�
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ��ϥΪ̩즲�̤p�Ƶ����ɡA
// �t�ΩI�s�o�ӥ\����o�����ܡC
HCURSOR CImageReFormatDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
//--------------------------------------------------//
//���Source Folder��Ƨ�
void CImageReFormatDlg::OnBnClickedButtonBrowserSource()
{
	if(ChoiseFolder(this->m_SrcFolderPath) == false)
	{ 
		::AfxMessageBox("Please Set Source Folder Path");
		return ;
	}
	this->GetDlgItem(IDC_EDIT_SOURCE_FOLDER_PATH)->SetWindowText(this->m_SrcFolderPath);
	//--------------------------------------------------//
}
//--------------------------------------------------//
//���Source Folder��Ƨ�
void CImageReFormatDlg::OnEnKillfocusEditSourceFolderPath()
{
	this->GetDlgItem(IDC_EDIT_SOURCE_FOLDER_PATH)->GetWindowText(this->m_SrcFolderPath);
	if(this->m_SrcFolderPath.IsEmpty() == true)
	{
		::AfxMessageBox("Please Set Source Folder Path");
		return ;
	}
}
//--------------------------------------------------//
//���Destination Folder��Ƨ�
void CImageReFormatDlg::OnBnClickedButtonBrowserDestination()
{
	if(ChoiseFolder(this->m_DesFolderPath) == false)
	{ 
		::AfxMessageBox("Please Set Destination Folder Path");
		return ;
	}
	this->GetDlgItem(IDC_EDIT_DESTINATION_FOLDER_PATH)->SetWindowText(this->m_DesFolderPath);
}
//--------------------------------------------------//
//���Destination Folder��Ƨ�
void CImageReFormatDlg::OnEnKillfocusEditDestinationFolderPath()
{
	this->GetDlgItem(IDC_EDIT_DESTINATION_FOLDER_PATH)->GetWindowText(this->m_DesFolderPath);
	if(this->m_DesFolderPath.IsEmpty() == true)
	{
		::AfxMessageBox("Please Set Destination Folder Path");
		return ;
	}
}
//--------------------------------------------------//
//���oSource Folder�����Ϥ����
void CImageReFormatDlg::OnBnClickedButtonGetNumberOfFiles()
{
	//�M���ɮצC��
	this->m_FilesName.clear();
	//���o��Ƨ����Ϥ��ƶq
	CString tFileFindPath;
	tFileFindPath.Format("%s\\%s", this->m_SrcFolderPath, "*");
	CFileFind tFileFind;
	tFileFind.FindFile(tFileFindPath);
	while(tFileFind.FindNextFile())
	{
		if(tFileFind.IsDirectory() == TRUE)
		{ continue; }
		CString tFileName;
		tFileName.Format("%s", tFileFind.GetFileName());
		int tTokenStart = 0;
		tFileName.Tokenize(".", tTokenStart);
		tFileName.Delete(0, tTokenStart - 1);
		bool tIsImage = false;

		IsImageExtension(tFileName,tIsImage);
		if(tIsImage == true)
		{ this->m_FilesName.push_back(tFileFind.GetFileName()); }
	}
	//--------------------------------------------------//
	//�]�w�ɮ׼ƶq
	this->m_NumberFiles = this->m_FilesName.size();
	this->SetDlgItemInt(IDC_EDIT_NUMBER_OF_FILES, this->m_NumberFiles);
}
//--------------------------------------------------//
void CImageReFormatDlg::OnBnClickedButtonExecute()
{
	//--------------------------------------------------//
	//�]�w�i�ױ��d��
	this->m_ProgressCtrl.SetRange(0, this->m_FilesName.size());
	this->m_ProgressCtrl.SetStep(1);
	this->m_ProgressCtrl.SetPos(g_CurrentOrder);
	//--------------------------------------------------//
	
	this->m_IsThreadStatus = D_THREAD_STATUS_RUN;
	this->GetDlgItem(IDC_EDIT_STATUS)->SetWindowText("Thread Run");
	for(int i = 0 ; i < D_MAX_THREAD_NUMBER ; i++)
	{
		::SetThreadPriority(g_ThreadHandle[i], THREAD_PRIORITY_NORMAL);
	}
}


void CImageReFormatDlg::OnBnClickedButtonStop()
{
	this->m_IsThreadStatus = D_THREAD_STATUS_STOP;
	::WaitForMultipleObjects(D_MAX_THREAD_NUMBER, g_ThreadEvent, true, INFINITE);
	for(int i = 0 ; i < D_MAX_THREAD_NUMBER ; i++)
	{
		::SetThreadPriority(g_ThreadHandle[i], THREAD_PRIORITY_LOWEST);
	}

	g_CurrentOrder = m_ProgressCtrl.GetPos();
	this->GetDlgItem(IDC_EDIT_STATUS)->SetWindowText("Thread Stop");
}

//��ܹ��ɨ줶��
void CImageReFormatDlg::ShowText(int PictureID, CString Text)
{
	switch(PictureID)
	{
	case 0:
		this->GetDlgItem(IDC_EDIT_THREAD_1)->SetWindowText(Text);
		break;
	case 1:
		this->GetDlgItem(IDC_EDIT_THREAD_2)->SetWindowText(Text);
		break;
	case 2:
		this->GetDlgItem(IDC_EDIT_THREAD_3)->SetWindowText(Text);
		break;
	case 3:
		this->GetDlgItem(IDC_EDIT_THREAD_4)->SetWindowText(Text);
		break;
	}
}
void CImageReFormatDlg::ShowImage(int PictureID, int Height, int Width, int WidthStep, int Channels, unsigned char * Data)
{
	if(PictureID < 0 || PictureID > 4)
	{ return ; }
	BITMAPINFO tBmpInfo;
	tBmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	tBmpInfo.bmiHeader.biWidth = Width;      
    tBmpInfo.bmiHeader.biHeight = -Height;
	tBmpInfo.bmiHeader.biPlanes = 1;
    tBmpInfo.bmiHeader.biBitCount    = Channels * 8;
    tBmpInfo.bmiHeader.biCompression = 0;      
    tBmpInfo.bmiHeader.biSizeImage   = WidthStep * Height;     
    tBmpInfo.bmiHeader.biXPelsPerMeter = 0;
	tBmpInfo.bmiHeader.biYPelsPerMeter = 0;
	tBmpInfo.bmiHeader.biClrUsed = 0;
	tBmpInfo.bmiHeader.biClrImportant = 0;
	if(tBmpInfo.bmiHeader.biBitCount == 8 )
	{
		for(int i = 0 ; i < 256 ; i++)   
		{      
			tBmpInfo.bmiColors[i].rgbBlue = i;       
			tBmpInfo.bmiColors[i].rgbGreen = i;   
			tBmpInfo.bmiColors[i].rgbRed = i;        
			tBmpInfo.bmiColors[i].rgbReserved = 0;    
		}
	}
	CStatic * PictureBox = NULL;
	switch(PictureID)
	{
	case 0:
		PictureBox = (CStatic*)this->GetDlgItem(IDC_PICTUREBOX_THREAD_1);
		break;
	case 1:
		PictureBox = (CStatic*)this->GetDlgItem(IDC_PICTUREBOX_THREAD_2);
		break;
	case 2:
		PictureBox = (CStatic*)this->GetDlgItem(IDC_PICTUREBOX_THREAD_3);
		break;
	case 3:
		PictureBox = (CStatic*)this->GetDlgItem(IDC_PICTUREBOX_THREAD_4);
		break;
	}
	
	if(PictureBox->GetSafeHwnd() == NULL)
	{ return ; }
	
	CClientDC tdc(PictureBox);
	HDC tHdc = tdc.GetSafeHdc();
	if(tHdc == NULL)
	{ return ;}

	RECT tRect;
	PictureBox->GetClientRect(&tRect);

	HDC tBKDC;
	HBITMAP tBKHBitmap, tOldBKHBitmap;
	tBKDC = ::CreateCompatibleDC(tHdc);
	tBKHBitmap = ::CreateCompatibleBitmap(tHdc, tRect.right, tRect.bottom);
	tOldBKHBitmap = (HBITMAP)::SelectObject(tBKDC, tBKHBitmap);

	::IntersectClipRect(tHdc, tRect.left, tRect.top, tRect.right, tRect.bottom);

	int OldMode = ::SetStretchBltMode(tBKDC, HALFTONE);
	::StretchDIBits(tBKDC, 0, 0, tRect.right, tRect.bottom, 0, 0, Width, Height, Data, &tBmpInfo, BI_RGB, SRCCOPY); 
	::BitBlt(tHdc, 0,0, tRect.right, tRect.bottom,tBKDC, 0,0,SRCCOPY);
	::SetStretchBltMode(tBKDC, OldMode);

	::SelectObject(tBKDC, tOldBKHBitmap);
	::DeleteObject(tBKHBitmap);
	tBKHBitmap = NULL;
	::DeleteDC(tBKDC);
	tBKDC =NULL;

}

void CImageReFormatDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	this->m_IsThreadStatus = D_THREAD_STATUS_BREAK;
}
