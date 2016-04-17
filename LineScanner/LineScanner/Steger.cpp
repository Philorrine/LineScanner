#include "stdafx.h"
#include "Steger.h"
#include "cv.h"
#include "highgui.h"

#define PI 3.1415926
CSteger::CSteger()
{
}
CSteger::~CSteger()
{
	if (img != NULL)
		cvReleaseImage(&img);
	if (img_org != NULL)
		cvReleaseImage(&img_org);
	if (img_lineExtraction != NULL)
		cvReleaseImage(&img_lineExtraction);
}
int CSteger::GetWidth()       //被处理图像的宽
{
	return img->width;
}
int CSteger::GetHeight()     //被处理图像的gao
{
	return img->height;
}
void CSteger::Jacobi2by2(float *H11, float *H12, float *H22, float *d1, float *d2, float *v11, float *v12, float *v21, float *v22)
{
	if (*H12 == 0)
	{//已经是对角阵
		*d1 = *H11;
		*d2 = *H22;
		*v11 = 1;
		*v12 = 0;
		*v21 = 0;
		*v22 = 1;
		return;
	}
	float theta = (*H22 - *H11) / (2 * (*H12));
	float t = (float)(1 / (fabs(theta) + sqrt(theta*theta + 1)));
	if (theta<0)
		t = -t;
	float c = (float)(1 / sqrt(t*t + 1));
	float s = t*c;
	//a11=a(1,1)-t*a(1,2)
	//a22=a(2,2)+t*a(1,2)
	*d1 = *H11 - t*(*H12);
	*d2 = *H22 + t*(*H12);
	*v11 = c;
	*v12 = s;
	*v21 = -s;
	*v22 = c;
	//v(1,1)=c;
	//v(1,2)=s;
	//v(2,1)=-s;
	//v(2,2)=c;

}
float CSteger::g_n(int n, float Sigma)
{
	return (float)(exp(-n*n / (2 * Sigma*Sigma)) / (sqrt(2 * PI)*Sigma));
}
float CSteger::g_n_first(int n, float Sigma) 
{
	return (float)(-n*exp(-n*n / (2 * Sigma*Sigma)) / (sqrt(2 * PI)*powf(Sigma, 3)));
}
float CSteger::g_n_second(int n, float Sigma)
{
	return (float)((n*n - Sigma*Sigma) * exp(-n*n / (2 * Sigma*Sigma)) / (sqrt(2 * PI)*powf(Sigma, 5)));
}

void CSteger::CInitial(IplImage* image, IplImage *image_org,  int th_line, float sigma, float length, BOOL ifshow1)//int th_line,float sigma
{
	Th_LineExtraction = th_line;

	m_Sigma = sigma;

	img = cvCreateImage(cvGetSize(image), image->depth, 1);
	img = cvCloneImage(image);

	img_org = cvCreateImage(cvGetSize(image_org), image_org->depth, 1);
	img_org = cvCloneImage(image_org);

	px = new float[img->width*img->height];
	py = new float[img->width*img->height];

	ifshow = ifshow1;
}

void CSteger::IPL_LineExtraction() //CSteger类下光条提取的成员函数
{

	IplImage *img1 = 0;
	img1 = cvCreateImage(cvGetSize(img), 8, 1);
	img1 = cvCloneImage(img);

	int n = 2 * (int)(3 * m_Sigma + 0.5) + 1;//mask size of convolution kernel,17  高斯参数m_Sigma=2.5、这里有强制装换 ????
	int i, j = 0;
	double ma, mi;
	int width = GetWidth();
	int height = GetHeight();
	float * g = new float[n];
	float * g_first = new float[n];
	float * g_second = new float[n];
	float * nog_first = new float[n];
	for (i = -(n - 1) / 2; i <= (n - 1) / 2; i++)
	{
		g[j] = g_n(i, m_Sigma);
		g_first[j] = g_n_first(i, m_Sigma);
		g_second[j] = g_n_second(i, m_Sigma);
		nog_first[j] = -1 * g_first[j];
		j = j + 1;
	}
	IplImage* img32F = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_32F, 1);
	cvSubS(img1, cvRealScalar(1), img1, NULL);
	cvConvertScale(img1, img32F, 1, 0);


	IplImage* imgrx = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_32F, 1);
	IplImage* imgry = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_32F, 1);
	IplImage* imgrxx = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_32F, 1);
	IplImage* imgrxy = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_32F, 1);
	IplImage* imgryy = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_32F, 1);


	CvMat* x_kernel;
	CvMat* x_first_kernel;
	CvMat* x_second_kernel;
	CvMat* y_kernel;
	CvMat* y_first_kernel;
	CvMat* y_second_kernel;
	x_kernel = cvCreateMatHeader(1, n, CV_32FC1); x_kernel->data.fl = g;
	x_first_kernel = cvCreateMatHeader(1, n, CV_32FC1); x_first_kernel->data.fl = nog_first;
	x_second_kernel = cvCreateMatHeader(1, n, CV_32FC1); x_second_kernel->data.fl = g_second;
	y_kernel = cvCreateMatHeader(n, 1, CV_32FC1); y_kernel->data.fl = g;
	y_first_kernel = cvCreateMatHeader(n, 1, CV_32FC1); y_first_kernel->data.fl = nog_first;
	y_second_kernel = cvCreateMatHeader(n, 1, CV_32FC1); y_second_kernel->data.fl = g_second;

	cvFilter2D(img32F, imgrx, y_kernel, cvPoint(-1, -1));         //
	cvFilter2D(imgrx, imgrx, x_first_kernel, cvPoint(-1, -1));    //gx

	cvFilter2D(img32F, imgry, y_first_kernel, cvPoint(-1, -1));   //
	cvFilter2D(imgry, imgry, x_kernel, cvPoint(-1, -1));          //gy

	cvFilter2D(img32F, imgrxx, x_second_kernel, cvPoint(-1, -1));  //
	cvFilter2D(imgrxx, imgrxx, y_kernel, cvPoint(-1, -1));         //gxx

	cvFilter2D(img32F, imgrxy, x_first_kernel, cvPoint(-1, -1));   //
	cvFilter2D(imgrxy, imgrxy, y_first_kernel, cvPoint(-1, -1));   //gxy

	cvFilter2D(img32F, imgryy, x_kernel, cvPoint(-1, -1));         //
	cvFilter2D(imgryy, imgryy, y_second_kernel, cvPoint(-1, -1));  //gyy

	cvSetZero(img32F);

	float temp_nx, temp_ny, temp_px, temp_py;

	int location, width_i_j;
	float max_eigenvalue, fenmu, t;
	float H11, H12, H22, d1, d2, v11, v12, v21, v22;
	int widthstep = imgrx->widthStep / 4;//780
										 //	float * nx=new float[width*height];
										 //	float * ny=new float[width*height];

										 //	DWORD dwstartjacobi = GetTickCount();
	int count = 0;
	for (i = height - 1; i >= 0; i--)
	{
		for (j = 0; j<width; j++)
		{
			location = i*widthstep + j;
			H11 = ((float*)imgrxx->imageData)[location];

			H12 = ((float*)imgrxy->imageData)[location];
			H22 = ((float*)imgryy->imageData)[location];
			Jacobi2by2(&H11, &H12, &H22, &d1, &d2, &v11, &v12, &v21, &v22);

			//	找出负的极大特征值及其对应特征向量
			if (d1<0 && d1 <= d2)
			{
				max_eigenvalue = -d1;
				temp_nx = v11;//负极大特征值对应的特征向量
				temp_ny = v21;
			}
			else if (d2<0 && d2 <= d1)
			{
				max_eigenvalue = -d2;
				temp_nx = v12;
				temp_ny = v22;
			}
			else
				continue;

			width_i_j = width*i + j;
			//			nx[width_i_j]=temp_nx;
			//			ny[width_i_j]=temp_ny;
			fenmu = temp_nx*temp_nx*((float*)imgrxx->imageData)[location]//r_xx[width_i_j]
				+ 2 * temp_nx*temp_ny*((float*)imgrxy->imageData)[location]//r_xy[width_i_j]
				+ temp_ny*temp_ny*((float*)imgryy->imageData)[location];//r_yy[width_i_j];
			t = -(temp_nx*((float*)imgrx->imageData)[location]
				+ temp_ny*((float*)imgry->imageData)[location]) / fenmu;
			temp_px = t*temp_nx;//distance from the center of current pixel to line point, i.e. the relative distace
			temp_py = t*temp_ny;

			if (fabs(temp_px) <= 0.5 && fabs(temp_py) <= 0.5)
			{
				((float*)img32F->imageData)[location] = max_eigenvalue;
				// 					if (max_eigenvalue>5)
				// 					{		printf("%f,",((float*)imgrx->imageData)[location]);
				//					}				
				px[width_i_j] = temp_px + j;// the real location of line points, i.e. the absolute coordinate of line points
				py[width_i_j] = temp_py + i;
				count++;
			}
		}
	}

	cvSetZero(img1);
	cvMinMaxLoc(img32F, &mi, &ma, NULL, NULL, NULL);
	cvConvertScale(img32F, img1, 255 / (ma - mi), 0);//为减少变量，将img1复用为imgI2	
													 // 	cvSetZero( img1 );
	widthstep = img->widthStep;

	img_lineExtraction = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_8U, 1);
	cvThreshold(img1, img_lineExtraction, Th_LineExtraction, 255, CV_THRESH_BINARY);//Th_LineExtraction 这是第一个阈值
																					//	int a=cvCountNonZero(img_lineExtraction);//输出非零点的个数，其矩阵须为单通道图像  
	int nn = 0;

	//以下是提取点所保存的txt文档
#ifdef savedata
	std::fstream datafile(filedata, std::ios::app | std::ios::out);
	for (int t1 = 0; t1<img_lineExtraction->height; t1++)
	{
		for (int t2 = 0; t2<img_lineExtraction->width; t2++)
		{
			if ((*(img_lineExtraction->imageData + t1*img_lineExtraction->width + t2)) != 0)
			{
				//截取中间一部分
				if (py[t1*img_lineExtraction->width + t2]<dline&&py[t1*img_lineExtraction->width + t2]>uline&&px[t1*img_lineExtraction->width + t2]<rway&&px[t1*img_lineExtraction->width + t2]>lway)
				{
					nn++;
					datafile << px[t1*img_lineExtraction->width + t2] << "," << py[t1*img_lineExtraction->width + t2] << endl;
					//		printf("%d,",(unsigned char)(*(imgclone->imageData+t1*imgclone->width+t2)));				
				}
			}
		}
	}
	datafile.close();
	printf("%d ", nn);
#endif
	cvReleaseImage(&img32F); cvReleaseImage(&imgrx); cvReleaseImage(&imgry);
	cvReleaseImage(&imgrxx); cvReleaseImage(&imgrxy); cvReleaseImage(&imgryy);
	cvReleaseImage(&img1);
	cvReleaseMat(&x_kernel); cvReleaseMat(&x_first_kernel);
	cvReleaseMat(&x_second_kernel); cvReleaseMat(&y_kernel); cvReleaseMat(&y_first_kernel); cvReleaseMat(&y_second_kernel);

	free(g); free(g_first); free(g_second); free(nog_first);

}




