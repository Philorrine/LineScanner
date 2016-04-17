
// LineScannerDlg.cpp : ʵ���ļ�
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


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CLineScannerDlg �Ի���




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


// CLineScannerDlg ��Ϣ�������

BOOL CLineScannerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CLineScannerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
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
		AfxMessageBox(L"������8BPP��24BPP��ͼ��");
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString Name, Path;
	int idx = ((CListBox*)GetDlgItem(IDC_LIST_READIMG))->GetCurSel();
	((CListBox*)GetDlgItem(IDC_LIST_READIMG))->GetText(idx, Name);
	GetDlgItemText(IDC_MFCEDITBROWSE_LOADIMG, Path);
	IplImage* img_calib = cvLoadImage(CW2A(Path + L"\\" + Name), CV_LOAD_IMAGE_GRAYSCALE);
	DrawToCDC(img_calib, GetDlgItem(IDC_SHOW_IMG)->GetDC());


	//IplImage *src_show = cvCreateImage(cvGetSize(img_calib), img_calib->depth, 3);

	//cvAddS(img_calib, cvScalar(-90), img_calib);//�����Ҷ�ֵ

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

	//feature->IPL_LineExtraction();//px py ����������������

	//										//��������ȡ�ĵ���ͼ���ϱ��Ĳ���
	//int r, c;
	//int i = 0;
	//int off = 10;
	//for (r = off; r<img_calib->height - off; r++)
	//{
	//	for (c = off; c<img_calib->width - off; c++)
	//	{
	//		if (((uchar*)(feature->img_lineExtraction->imageData + r*feature->img_lineExtraction->widthStep)[c]) != 0)
	//		{
	//			//��ʾ�м䲿��
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
	////���������ݵ��ͷ�
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
