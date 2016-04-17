#pragma once
#include "highgui.h"

class CSteger
{
public:
	CSteger();
	~CSteger();
public:
	IplImage *img, *img_org;
	IplImage *img_lineExtraction;

	float * px;//用于保存亚象素坐标
	float * py;
	double m_Sigma;
	int Th_LineExtraction;//阈值
	CvSize pattern;
	BOOL ifshow;

public:
	void CInitial(IplImage* image, IplImage*image_org,  int th_line,  float sigma, float length, BOOL ifshow1);
	void IPL_LineExtraction();
	int GetWidth();
	int GetHeight();

private:
	void Jacobi2by2(float *H11, float *H12, float *H22, float *d1,
		float *d2, float *v11, float *v12, float *v21, float *v22);
	float g_n(int n, float Sigma);
	float g_n_first(int n, float Sigma);
	float g_n_second(int n, float Sigma);
};

