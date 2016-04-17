
// LineScannerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LineScanner.h"
#include "LineScannerDlg.h"
#include "afxdialogex.h"
#include "highgui.h"
#include "cv.h"
#include "Steger.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CLineScannerDlg 对话框




CLineScannerDlg::CLineScannerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLineScannerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLineScannerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CLineScannerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_EXTRACT, &CLineScannerDlg::OnBnClickedButtonExtract)
	ON_BN_CLICKED(IDC_BUTTON_LOADIMG, &CLineScannerDlg::OnBnClickedButtonLoadimg)
END_MESSAGE_MAP()


// CLineScannerDlg 消息处理程序

BOOL CLineScannerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CLineScannerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CLineScannerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CLineScannerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

float CLineScannerDlg::DrawToCDC(IplImage* img, CDC* pDC)
{
	CRect rect;
	CImage bkgound;
	float ratio;
	BITMAPINFO bmi;
	BITMAPINFOHEADER* bmih = &(bmi.bmiHeader);
	memset(bmih, 0, sizeof(*bmih));
	IplImage* pImg = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_8U, 3);

	if ((img->nChannels * img->depth) == 8)
	{
		cvCvtColor(img, pImg, CV_GRAY2RGB);
	}

	if ((img->nChannels * img->depth) == 24)
	{
		cvCopy(img, pImg);
	}
	if ((img->nChannels * img->depth) > 24)
	{
		AfxMessageBox(L"请输入8BPP或24BPP的图像！");
		return 0;
	}

	bmih->biSize = sizeof(BITMAPINFOHEADER);
	bmih->biWidth = pImg->width;
	bmih->biHeight = -abs(pImg->height);
	bmih->biPlanes = 1;
	bmih->biBitCount = 24;
	bmih->biCompression = BI_RGB;

	pDC->GetWindow()->GetWindowRect(&rect);
	bkgound.Create(rect.Width(), rect.Height(), 24);
	if ((float)rect.Height() / (pImg->height) > (float)rect.Width() / (pImg->width))
	{
		ratio = (float)rect.Width() / (pImg->width);
		bkgound.Draw(pDC->GetSafeHdc(), 0, (int)((pImg->height)*ratio), rect.Width(), rect.Height() - (int)((pImg->height)*ratio));
	}
	else {
		ratio = (float)rect.Height() / (pImg->height);
		bkgound.Draw(pDC->GetSafeHdc(), (int)((pImg->width)*ratio), 0, rect.Width() - (int)((pImg->width)*ratio), rect.Height());
	}
	pDC->SetStretchBltMode(HALFTONE);
	::StretchDIBits(pDC->GetSafeHdc(), 0, 0, (int)((pImg->width)*ratio), (int)((pImg->height)*ratio), 0, 0, pImg->width, pImg->height, pImg->imageData, &bmi, DIB_RGB_COLORS, SRCCOPY);
	cvReleaseImage(&pImg);
	return ratio;
}

void CLineScannerDlg::OnBnClickedButtonExtract()
{
	// TODO: 在此添加控件通知处理程序代码
	CString Name, Path;
	int idx = ((CListBox*)GetDlgItem(IDC_LIST_READIMG))->GetCurSel();
	((CListBox*)GetDlgItem(IDC_LIST_READIMG))->GetText(idx, Name);
	GetDlgItemText(IDC_MFCEDITBROWSE_LOADIMG, Path);
	IplImage* img_calib = cvLoadImage(CW2A(Path + L"\\" + Name), CV_LOAD_IMAGE_GRAYSCALE);
	DrawToCDC(img_calib, GetDlgItem(IDC_SHOW_IMG)->GetDC());


	//IplImage *src_show = cvCreateImage(cvGetSize(img_calib), img_calib->depth, 3);

	//cvAddS(img_calib, cvScalar(-90), img_calib);//所减灰度值

	//cvCvtColor(img_calib, src_show, CV_GRAY2BGR);
	//CSteger *feature;
	//feature = new CSteger;

	//IplImage* edgex;
	//IplImage* edge;
	//IplImage* dedge;
	//edgex = cvCreateImage(cvGetSize(img_calib), IPL_DEPTH_16S, 1);
	//edge = cvCreateImage(cvGetSize(img_calib), IPL_DEPTH_16S, 1);
	//dedge = cvCreateImage(cvGetSize(img_calib), IPL_DEPTH_8U, 1);
	//cvSobel(img_calib, edgex, 1, 0, 3);
	//cvSobel(img_calib, edge, 0, 1, 3);   
	//cvAdd(edgex, edge, edge);    
	//cvCvtScaleAbs(edge, dedge, 1, 0);

	//feature->CInitial(dedge, img_calib, 50, 35, 0.3125, 1);

	//feature->IPL_LineExtraction();//px py 保存着亚像素坐标

	//										//以下是提取的点在图像上标红的部分
	//int r, c;
	//int i = 0;
	//int off = 10;
	//for (r = off; r<img_calib->height - off; r++)
	//{
	//	for (c = off; c<img_calib->width - off; c++)
	//	{
	//		if (((uchar*)(feature->img_lineExtraction->imageData + r*feature->img_lineExtraction->widthStep)[c]) != 0)
	//		{
	//			//显示中间部分
	//			if (feature->py[r*feature->img_lineExtraction->width + c]<dline&&feature->py[r*feature->img_lineExtraction->width + c]>uline&&feature->px[r*feature->img_lineExtraction->width + c]<rway&&feature->px[r*feature->img_lineExtraction->width + c]>lway)
	//			{
	//				cvCircle(src_show, cvPoint(cvRound(feature->px[r*feature->img_lineExtraction->width + c]), cvRound(feature->py[r*feature->img_lineExtraction->width + c])), 1, CV_RGB(255, 0, 0), -1, 8, 0);
	//				i++;
	//			}
	//		}
	//	}
	//}
	//printf(" marked%d ", i);

	//cvNamedWindow(filename, 0);
	//cvResizeWindow(filename, 1024, 768);
	//cvShowImage(filename, src_show);
	////以下是数据的释放
	//delete feature;
	//cvReleaseImage(&img_calib);
	//cvReleaseImage(&edgex); cvReleaseImage(&edge); cvReleaseImage(&dedge);
	////cvReleaseImage(&dstFsrc);
	//cvWaitKey(0);
	//cvDestroyWindow(filename);
	//cvReleaseImage(&src_show);

}


void CLineScannerDlg::OnBnClickedButtonLoadimg()
{
	// TODO: 在此添加控件通知处理程序代码
	CString dirPath;
	GetDlgItemText(IDC_MFCEDITBROWSE_LOADIMG, dirPath);
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = ::FindFirstFile(dirPath + L"\\*.bmp", &FindFileData);
	if (INVALID_HANDLE_VALUE == hFind)
	{
		return;
	}

	while (TRUE)
	{
		if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			((CListBox*)GetDlgItem(IDC_LIST_READIMG))->AddString(FindFileData.cFileName);
		}
		if (!FindNextFile(hFind, &FindFileData))    break;
	}
	FindClose(hFind);
}
