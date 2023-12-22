#include "stdafx.h"
#include "math.h"
#include<complex>

using namespace std;
#define PI 3.1415926535

LONG size = 0;
BITMAPINFO* lpBitsInfo = NULL;
BITMAPINFO*  lpDIB_FT = NULL;
BITMAPINFO* lpDIB_IFT = NULL;

complex<double>* gFD = NULL;

BOOL LoadBmpFile(char* BmpFile){
	
	FILE* fp;

	if(NULL == (fp = fopen(BmpFile,"rb")))
		return NULL;
	
	BITMAPFILEHEADER bf;
	BITMAPINFOHEADER bi;
	// �ļ�ͷ������14
	fread(&bf, 14, 1, fp);
	// ��Ϣͷ������40
	fread(&bi, 40, 1, fp);
	// һ�е��ֽ������Ѷ��룩
	DWORD LineBytes = (bi.biWidth * bi.biBitCount + 31)/32 * 4;
	// λͼ���ݴ�С
	DWORD ImgSize = LineBytes * bi.biHeight;
	// ʹ�õ���ɫ	
	DWORD NumColors;
	if(bi.biClrUsed != 0)
		NumColors = bi.biClrUsed;
	else{
		switch(bi.biBitCount) {
		case 1:
			NumColors = 2;
			break;
		case 4:
			NumColors = 16;
			break;
		case 8:
			NumColors = 256;
			break;
		case 24:
			NumColors = 0;
			break;
		}
	}

	size = sizeof(BITMAPINFOHEADER) + NumColors * sizeof(RGBQUAD) + ImgSize;
	
	if(NULL == (lpBitsInfo = (BITMAPINFO*)malloc(size)))
		return FALSE;
	
	fseek(fp,14,SEEK_SET);

	fread(lpBitsInfo,size,1,fp);
	fclose(fp);

	lpBitsInfo->bmiHeader.biClrUsed = NumColors;

	//BITMAPINFO* lpDIB = (BITMAPINFO*)malloc(size);
	

	return TRUE;
}

void gray()
{



	int w = lpBitsInfo->bmiHeader.biWidth;
	int h = lpBitsInfo->bmiHeader.biHeight;
	int LineBytes = (w * lpBitsInfo->bmiHeader.biBitCount + 31)/32 * 4;
	BYTE* lpBits = (BYTE*)&lpBitsInfo->bmiColors[lpBitsInfo->bmiHeader.biClrUsed];
	
	int LineBytes_gray = (w*8+31)/32*4;
	int size = 40 + 256 * 4 + LineBytes_gray * h;
	BITMAPINFO* gray_lpbitsinfo = (BITMAPINFO*)malloc(size);
	memcpy(gray_lpbitsinfo,lpBitsInfo,40);
	gray_lpbitsinfo->bmiHeader.biBitCount = 8;
	gray_lpbitsinfo->bmiHeader.biClrUsed = 256;

	//gray_lpbitsInfo->bmiColors = (int*)malloc(sizeof(BYTE)*256);
	

	int i,j;
	BYTE* pixel;

	for(i=0;i<256;i++){
		gray_lpbitsinfo->bmiColors[i].rgbRed = i;
		gray_lpbitsinfo->bmiColors[i].rgbGreen = i;
		gray_lpbitsinfo->bmiColors[i].rgbBlue = i;
		gray_lpbitsinfo->bmiColors[i].rgbReserved = 0;
	}
	BYTE *R,*G,*B,avg;


	BYTE* gray_lpBits = (BYTE*)&gray_lpbitsinfo->bmiColors[256];
	for(i = 0; i < h; i++)
	{
		for(j = 0; j < w; j++)
	{
			B = lpBits + LineBytes * (h -1 - i)  + j*3;
			G = B + 1;
			R = G + 1;
			avg = (*B+*G+*R)/3;
			

			pixel = gray_lpBits + LineBytes_gray * (h - 1 - i) + j;
			*pixel = avg;
		}
	}
	free(lpBitsInfo);
	lpBitsInfo = gray_lpbitsinfo;
}

BOOL IsGray(){
	int r,g,b;
	if(8 == lpBitsInfo->bmiHeader.biBitCount){
			r = lpBitsInfo->bmiColors[128].rgbRed;	
			g = lpBitsInfo->bmiColors[128].rgbGreen;	
			b = lpBitsInfo->bmiColors[128].rgbBlue;	

			if(r == b&& r==g)
				return TRUE;
	}
	return FALSE;
}

void pixel(int i,int j, char* str){
	if(NULL == lpBitsInfo)
		return;

	
	int w = lpBitsInfo->bmiHeader.biWidth;
	int h = lpBitsInfo->bmiHeader.biHeight;
	int LineBytes = (w * lpBitsInfo->bmiHeader.biBitCount + 31)/32 * 4;
	BYTE* lpBits = (BYTE*)&lpBitsInfo->bmiColors[lpBitsInfo->bmiHeader.biClrUsed];

	if(i>= h|| j>=w)
		return;

	BYTE *pixel,bv,r,g,b;
	
	switch(lpBitsInfo->bmiHeader.biBitCount){
	
	case 8:
		pixel = lpBits + LineBytes * (h - 1 - i) + j;
		if(IsGray())
			sprintf(str,"����ֵ��%d",*pixel);
		else{
			r = lpBitsInfo->bmiColors[*pixel].rgbRed;	
			g = lpBitsInfo->bmiColors[*pixel].rgbGreen;	
			b = lpBitsInfo->bmiColors[*pixel].rgbBlue;
			sprintf(str,"RGB(%d,%d,%d)",r,g,b);
		}
		break;
	case 1:
		bv = *(lpBits + LineBytes * (h - 1 - i) + j/8)&(1 << (7 - j%8));
		if(0 == bv)
			strcpy(str,"������");
		else
			strcpy(str,"ǰ����");
		break;
	case 4:
		bv = *(lpBits + LineBytes * (h - 1 - i) + j/8) & (1 << (7 - j%8));
		if(0==j%2)
			bv /= 16;
		else
			bv %= 16;
		r = lpBitsInfo->bmiColors[bv].rgbRed;
		g = lpBitsInfo->bmiColors[bv].rgbGreen;
		b = lpBitsInfo->bmiColors[bv].rgbBlue;
		sprintf(str,"RGB(%d,%d,%d)",r,g,b);
			
		break;
	case 24:
		pixel = lpBits + LineBytes * (h - 1 - i) + j * 3;
		b = *pixel;
		g = *(pixel + 1);
		b = *(pixel + 2);
		sprintf(str,"RGB(%d,%d,%d)",r,g,b);
		break;
	}

}

DWORD H[256];

void Histogram()
{
	int w = lpBitsInfo->bmiHeader.biWidth;
	int h = lpBitsInfo->bmiHeader.biHeight;
	int LineBytes = (w * lpBitsInfo->bmiHeader.biBitCount + 31)/32 * 4;
	BYTE* lpBits = (BYTE*)&lpBitsInfo->bmiColors[lpBitsInfo->bmiHeader.biClrUsed];

	int i,j;
	BYTE* pixel;

	for(i = 0;i < 256;++i){
		H[i] = 0;
	}

	for(i = 0;i < w;++i){
		for(j = 0;j < h;++j){
			pixel = lpBits + LineBytes * (h - 1 - i) + j;
			H[*pixel]++;
		}
	}

}

void LineTrans(float a,float b){

	int w = lpBitsInfo->bmiHeader.biWidth;
	int h = lpBitsInfo->bmiHeader.biHeight;
	int LineBytes = (w * lpBitsInfo->bmiHeader.biBitCount + 31)/32 * 4;
	BYTE* lpBits = (BYTE*)&lpBitsInfo->bmiColors[lpBitsInfo->bmiHeader.biClrUsed];

	int i,j;
	BYTE* pixel;
	for(i = 0;i < w;++i){
		for(j = 0;j < h;++j){
			pixel = lpBits + LineBytes * (h - 1 - i) + j;
			*pixel = *pixel * a + b;
		}
	}
}

void Equalize()
{
	int w = lpBitsInfo->bmiHeader.biWidth;
	int h = lpBitsInfo->bmiHeader.biHeight;
	int LineBytes = (w * lpBitsInfo->bmiHeader.biBitCount + 31)/32 * 4;
	BYTE* lpBits = (BYTE*)&lpBitsInfo->bmiColors[lpBitsInfo->bmiHeader.biClrUsed];

	int i,j;
	BYTE* pixel;
	int temp;
	BYTE Map[256];

	Histogram();
	for(i = 0;i < 256; i++)
	{
		temp = 0;
		for(j = 0; j <= i; j++)
			temp += H[j];
		Map[i] = (BYTE)(temp * 255 / (w * h)+0.5);
	}

	
	for(i = 0;i < w;++i){
		for(j = 0;j < h;++j){
			pixel = lpBits + LineBytes * (h - 1 - i) + j;
			*pixel = Map[*pixel];
		}
	}

}

void FT(complex<double>* TD,complex<double>* FD,int m)
{
	int x, u;
	double angle;
	for(u = 0;u < m;u++){
		FD[u] = 0;
		for(x= 0; x < m; x++){
			angle = -2 * PI * u * x/m;
			complex<double> a (cos(angle),sin(angle));
			FD[u] += TD[u] * a;
		}
		FD[u] /= m;
	}
}

void IFT(complex<double>* TD,complex<double>* FD,int m)
{
	int x, u;
	double angle;
	for(x = 0;x < m;x++){
		TD[u] = 0;
		for(u= 0; u < m; u++){
			angle = -2 * PI * u * x/m;
			complex<double> a (cos(angle),sin(angle));
			TD[u] += FD[u] * a;
		}
	}
}

BOOL FD_Available(){
	return (gFD != NULL);
}


void Fourier()
{
	int w = lpBitsInfo->bmiHeader.biWidth;
	int h = lpBitsInfo->bmiHeader.biHeight;
	int LineBytes = (w * lpBitsInfo->bmiHeader.biBitCount + 31)/32 * 4;
	BYTE* lpBits = (BYTE*)&lpBitsInfo->bmiColors[lpBitsInfo->bmiHeader.biClrUsed];
	
	complex<double>* TD =new complex<double>[w * h];
	complex<double>* FD =new complex<double>[w * h];

	int i, j;
	BYTE* pixel;
	for(i = 0;i < w;++i){
		for(j = 0;j < h;++j){
			pixel = lpBits + LineBytes * (h - 1 - i) + j;
			TD[w * i + j] = complex<double>(*pixel * pow(-1,i + j),0);
		}
	}

	for(i = 0;i < h;i ++)
		FT(&TD[w * i],&FD[w * i],w);
	for(i = 0;i < h;i ++)
		for(j = 0;j < h;++j)
			TD[h * j + i] = FD[w * i + j];
	for(i = 0;i < w;i ++)
		FT(&TD[h * i],&FD[h * i],h);

	LONG size = 40 + 1024 + LineBytes * h;

	lpDIB_FT = (BITMAPINFO*)malloc(size);
	if(NULL == lpDIB_FT)
		return;
	memcpy(lpDIB_FT,lpBitsInfo,size);
	lpBits = (BYTE*)&lpDIB_FT->bmiColors[lpDIB_FT->bmiHeader.biClrUsed];
	
	double temp;
	for(i = 0;i < h;i ++){
		for(j = 0;j < w; j++){
			pixel = lpBits + LineBytes *(h - 1 - i) + j;
			temp = sqrt(FD[j * h + i].real() * FD[j * h + i].real() + FD[j * h + i].imag()*FD[j * h + i].imag())* 2000;
			if(temp > 255)
				temp = 255;
			*pixel = (BYTE)(temp);
		}
	}

	delete TD;
	gFD = FD;
}

void IFourier(){
}


BOOL is_gFD_OK()
{
	return (gFD != NULL);
}

void FFT(complex<double> * TD, complex<double> * FD, int r)
{
	// ���㸶��Ҷ�任����
	LONG count = 1 << r;
	// �����Ȩϵ��
	int i;
	double angle;
	complex<double>* W = new complex<double>[count / 2];
	for(i = 0; i < count / 2; i++)
	{
		angle = -i * PI * 2 / count;
		W[i] = complex<double> (cos(angle), sin(angle));
	}
	// ��ʱ���д��X1
	complex<double>* X1 = new complex<double>[count];
	memcpy(X1, TD, sizeof(complex<double>) * count);
	
	// ���õ����㷨���п��ٸ���Ҷ�任�����ΪƵ��ֵX2
	complex<double>* X2 = new complex<double>[count]; 

	int k,j,p,size;
	complex<double>* temp;
	for (k = 0; k < r; k++)
	{
		for (j = 0; j < 1 << k; j++)
		{
			size = 1 << (r-k);
			for (i = 0; i < size/2; i++)
			{
				p = j * size;
				X2[i + p] = X1[i + p] + X1[i + p + size/2];
				X2[i + p + size/2] = (X1[i + p] - X1[i + p + size/2]) * W[i * (1<<k)];
			}
		}
		temp  = X1;
		X1 = X2;
		X2 = temp;
	}
	
	// ����������λ�������У�
	for (j = 0; j < count; j++)
	{
		p = 0;
		for (i = 0; i < r; i++)
		{
			if (j & (1<<i))
			{
				p += 1<<(r-i-1);
			}
		}
		FD[j]=X1[p];
		FD[j] /= count;
	}
	
	// �ͷ��ڴ�
	delete W;
	delete X1;
	delete X2;
}

void FFourier()
{
	//ͼ��Ŀ�Ⱥ͸߶�
	int width = lpBitsInfo->bmiHeader.biWidth;
	int height = lpBitsInfo->bmiHeader.biHeight;
	int LineBytes = (width * lpBitsInfo->bmiHeader.biBitCount + 31)/32 * 4;
	//ָ��ͼ������ָ��
	BYTE* lpBits = (BYTE*)&lpBitsInfo->bmiColors[lpBitsInfo->bmiHeader.biClrUsed];

	// FFT��ȣ�����Ϊ2�������η���
	int FFT_w = 1;
	// FFT��ȵ�����������������
	int wp = 0;
	while(FFT_w * 2 <= width)
	{
		FFT_w *= 2;
		wp ++;
	}

	// FFT�߶ȣ�����Ϊ2�������η���
	int FFT_h = 1;
	// FFT�߶ȵ�����������������
	int hp = 0;
	while(FFT_h * 2 <= height)
	{
		FFT_h *= 2;
		hp ++;
	}

	// �����ڴ�
	complex<double>* TD = new complex<double>[FFT_w * FFT_h];
	complex<double>* FD = new complex<double>[FFT_w * FFT_h];
	
	int i, j;
	BYTE* pixel;
	
	for(i = 0; i < FFT_h; i++)  // ��
	{
		for(j = 0; j < FFT_w; j++)  // ��
		{
			// ָ��DIB��i�У���j�����ص�ָ��
			pixel = lpBits + LineBytes * (height - 1 - i) + j;

			// ��ʱ��ֵ
			TD[j + FFT_w * i] = complex<double>(*pixel* pow(-1,i+j), 0);
		}
	}
	
	for(i = 0; i < FFT_h; i++)
	{
		// ��y������п��ٸ���Ҷ�任
		FFT(&TD[FFT_w * i], &FD[FFT_w * i], wp);
	}
	
	// �����м�任���
	for(i = 0; i < FFT_h; i++)
	{
		for(j = 0; j < FFT_w; j++)
		{
			TD[i + FFT_h * j] = FD[j + FFT_w * i];
		}
	}
	
	for(i = 0; i < FFT_w; i++)
	{
		// ��x������п��ٸ���Ҷ�任
		FFT(&TD[i * FFT_h], &FD[i * FFT_h], hp);
	}

	//����Ƶ��ͼ��
	//ΪƵ��ͼ������ڴ�
	LONG size = 40 + 1024 + LineBytes * height;
	lpDIB_FT = (LPBITMAPINFO) malloc(size);
	if (NULL == lpDIB_FT)
		return;
	memcpy(lpDIB_FT, lpBitsInfo, size);

	//ָ��Ƶ��ͼ������ָ��
	lpBits = (BYTE*)&lpDIB_FT->bmiColors[lpDIB_FT->bmiHeader.biClrUsed];

	double temp;
	for(i = 0; i < FFT_h; i++) // ��
	{
		for(j = 0; j < FFT_w; j++) // ��
		{
			// ����Ƶ�׷���
			temp = sqrt(FD[j * FFT_h + i].real() * FD[j * FFT_h + i].real() + 
				        FD[j * FFT_h + i].imag() * FD[j * FFT_h + i].imag()) *2000;
			
			// �ж��Ƿ񳬹�255
			if (temp > 255)
			{
				// ���ڳ����ģ�ֱ������Ϊ255
				temp = 255;
			}
			
			pixel = lpBits + LineBytes * (height - 1 - i) + j;

			// ����Դͼ��
			*pixel = (BYTE)(temp);
		}
	}

	delete TD;
	gFD = FD;
}

void IFFT(complex<double> * FD, complex<double> * TD, int r)
{
	// ����Ҷ�任����
	LONG	count;
	// ���㸶��Ҷ�任����
	count = 1 << r;

	// ������������洢��
	complex<double> * X = new complex<double>[count];
	// ��Ƶ���д��X
	memcpy(X, FD, sizeof(complex<double>) * count);
	
	// ����
	for(int i = 0; i < count; i++)
		X[i] = complex<double> (X[i].real(), -X[i].imag());
	
	// ���ÿ��ٸ���Ҷ�任
	FFT(X, TD, r);
	
	// ��ʱ���Ĺ���
	for(i = 0; i < count; i++)
		TD[i] = complex<double> (TD[i].real() * count, -TD[i].imag() * count);
	
	// �ͷ��ڴ�
	delete X;
}

void IFFourier()
{
	//ͼ��Ŀ�Ⱥ͸߶�
	int width = lpBitsInfo->bmiHeader.biWidth;
	int height = lpBitsInfo->bmiHeader.biHeight;
	int LineBytes = (width * lpBitsInfo->bmiHeader.biBitCount + 31)/32 * 4;

	// FFT��ȣ�����Ϊ2�������η���
	int FFT_w = 1;
	// FFT��ȵ�����������������
	int wp = 0;
	while(FFT_w * 2 <= width)
	{
		FFT_w *= 2;
		wp ++;
	}

	// FFT�߶ȣ�����Ϊ2�������η���
	int FFT_h = 1;
	// FFT�߶ȵ�����������������
	int hp = 0;
	while(FFT_h * 2 <= height)
	{
		FFT_h *= 2;
		hp ++;
	}

	// �����ڴ�
	complex<double>* TD = new complex<double>[FFT_w * FFT_h];
	complex<double>* FD = new complex<double>[FFT_w * FFT_h];
	
	int i, j;
	for(i = 0; i < FFT_h; i++)  // ��
		for(j = 0; j < FFT_w; j++)  // ��
			FD[j + FFT_w * i] = gFD[i + FFT_h*j];
	
	// ��ˮƽ������п��ٸ���Ҷ�任
	for(i = 0; i < FFT_h; i++)
		IFFT(&FD[FFT_w * i], &TD[FFT_w * i], wp);
	
	// �����м�任���
	for(i = 0; i < FFT_h; i++)
		for(j = 0; j < FFT_w; j++)
			FD[i + FFT_h * j] = TD[j + FFT_w * i];
	
	// �ش�ֱ������п��ٸ���Ҷ�任
	for(i = 0; i < FFT_w; i++)
		IFFT(&FD[i * FFT_h], &TD[i * FFT_h], hp);

	//Ϊ���任ͼ������ڴ�
	LONG size = 40 + 1024 + LineBytes * height;

	lpDIB_IFT = (LPBITMAPINFO) malloc(size);
	if (NULL == lpDIB_IFT)
		return;
	memcpy(lpDIB_IFT, lpBitsInfo, size);

	//ָ�򷴱任ͼ������ָ��
	BYTE* lpBits = (BYTE*)&lpDIB_IFT->bmiColors[256];
	BYTE* pixel;
	double temp;
	for(i = 0; i < FFT_h; i++) // ��
	{
		for(j = 0; j < FFT_w; j++) // ��
		{
			pixel = lpBits + LineBytes * (height - 1 - i) + j;
			temp= (TD[j*FFT_h + i].real() / pow(-1, i+j));
			if (temp < 0)
				temp = 0;
			else if (temp >255)
				temp = 255;
			*pixel = (BYTE)temp;
		}
	}

	// ɾ����ʱ����
	delete FD;
	delete TD;
	delete gFD;
gFD = NULL;
}

void Template(int* Array,float coef)
{
	int w = lpBitsInfo->bmiHeader.biWidth;
	int h = lpBitsInfo->bmiHeader.biHeight;
	int LineBytes = (w * lpBitsInfo->bmiHeader.biBitCount + 31)/32 * 4;
	//ָ��ͼ������ָ��
	BYTE* lpBits = (BYTE*)&lpBitsInfo->bmiColors[lpBitsInfo->bmiHeader.biClrUsed];

	LONG size = 40 + 1024 + LineBytes * h;
	BITMAPINFO* new_lpBitsInfo = (BITMAPINFO*) malloc(size);
	if (NULL == new_lpBitsInfo)
		return;
	memcpy(new_lpBitsInfo, lpBitsInfo, size);

	//ָ��Ƶ��ͼ������ָ��
	BYTE* new_lpBits = (BYTE*)&new_lpBitsInfo->bmiColors[new_lpBitsInfo->bmiHeader.biClrUsed];

	int i, j, k, l;
	BYTE *pixel, *new_pixel;
	float result;
	for(i = 0;i < h-1;i ++){
		for(j = 0;j < w-1; j++){
			new_pixel = new_lpBits + LineBytes *(h - 1 - i) + j;
			result = 0;
			
			for(k = 0;k < 3;k ++){
				for(l = 0;l < 3; l++){
					pixel = lpBits + LineBytes *(h - i - k) + j-1 + l;
					result += (*pixel)*Array[k*3 + l];

				}
			}
			result *= coef;
			if(result < 0)
				*new_pixel = 0;
			else if(result > 255)
				*new_pixel = 255;
			else
				*new_pixel = (BYTE)(result+0.5);
		}
	}
	free(lpBitsInfo);
	lpBitsInfo = new_lpBitsInfo;


}

void AvgSmooth(){
	int Array[9];
	Array[0]=1; Array[1]=1; Array[2]=1;
	Array[3]=1; Array[4]=1; Array[5]=1;
	Array[6]=1; Array[7]=1; Array[8]=1;
	Template(Array,(float)1/9);
}

BYTE GetMidNum(BYTE* Array)
{
	int i,j;
	BYTE temp;
	for(i = 0; i< 8;i ++)
	{
		for(j = 0;j < 8-i;j ++)
		{
			if(Array[j]>Array[j+1])
			{
				temp = Array[j];
				Array[j] = Array[j+1];
				Array[j+1] = temp;
			}
		}
	}
	return Array[4];
}

void MidSmooth(){
	int w = lpBitsInfo->bmiHeader.biWidth;
	int h = lpBitsInfo->bmiHeader.biHeight;
	int LineBytes = (w * lpBitsInfo->bmiHeader.biBitCount + 31)/32 * 4;
	//ָ��ͼ������ָ��
	BYTE* lpBits = (BYTE*)&lpBitsInfo->bmiColors[lpBitsInfo->bmiHeader.biClrUsed];

	LONG size = 40 + 1024 + LineBytes * h;
	BITMAPINFO* new_lpBitsInfo = (BITMAPINFO*) malloc(size);
	if (NULL == new_lpBitsInfo)
		return;
	memcpy(new_lpBitsInfo, lpBitsInfo, size);
	//ָ��Ƶ��ͼ������ָ��
	BYTE* new_lpBits = (BYTE*)&new_lpBitsInfo->bmiColors[new_lpBitsInfo->bmiHeader.biClrUsed];

	int i, j, k, l;
	BYTE *pixel, *new_pixel;
	BYTE Array[9];

	for(i = 0;i < h-1;i ++){
		for(j = 0;j < w-1; j++){
			new_pixel = new_lpBits + LineBytes *(h - 1 - i) + j;
			for(k = 0;k < 3;k ++){
				for(l = 0;l < 3; l++){
					pixel = lpBits + LineBytes *(h - i - k) + j-1 + l;
					Array[k*3 + l] = *pixel;
				}
			}
			*new_pixel = GetMidNum(Array);
		}
	}
	free(lpBitsInfo);
	lpBitsInfo = new_lpBitsInfo;



}


void GradSharp(){
	
	int w = lpBitsInfo->bmiHeader.biWidth;
	int h = lpBitsInfo->bmiHeader.biHeight;
	int LineBytes = (w * lpBitsInfo->bmiHeader.biBitCount + 31)/32 * 4;
	//ָ��ͼ������ָ��
	BYTE* lpBits = (BYTE*)&lpBitsInfo->bmiColors[lpBitsInfo->bmiHeader.biClrUsed];

	LONG size = 40 + 1024 + LineBytes * h;
	BITMAPINFO* new_lpBitsInfo = (BITMAPINFO*) malloc(size);
	if (NULL == new_lpBitsInfo)
		return;
	memcpy(new_lpBitsInfo, lpBitsInfo, size);

	//ָ��Ƶ��ͼ������ָ��
	BYTE* new_lpBits = (BYTE*)&new_lpBitsInfo->bmiColors[new_lpBitsInfo->bmiHeader.biClrUsed];

	int i, j, k, l;
	BYTE *new_pixel,*pixel, *pixel_h,*pixel_v, temp;
	for(i = 0;i < h-1;i ++){
		for(j = 0;j < w-1; j++){
			new_pixel = new_lpBits + LineBytes *(h - 1 - i) + j;
			pixel = lpBits + LineBytes *(h - 1 - i) + j;
			pixel_h = lpBits + LineBytes *(h - 1 - i) + j + 1;
			pixel_v = lpBits + LineBytes *(h - 2 - i) + j;
			*new_pixel =abs(*pixel - *pixel_h) + abs(*pixel - *pixel_v);
		}
	}
	free(lpBitsInfo);
	lpBitsInfo = new_lpBitsInfo;

}

void RaplasSharp(){
	int Array[9];
	Array[0]=-1; Array[1]=-1; Array[2]=-1;
	Array[3]=-1; Array[4]=9; Array[5]=-1;
	Array[6]=-1; Array[7]=-1; Array[8]=-1;
	Template(Array,(float)1);
}

void FFT_Filter(int D)
{
	int w = lpDIB_FT->bmiHeader.biWidth;
	int h = lpDIB_FT->bmiHeader.biHeight;
	int LineBytes = (w * lpDIB_FT->bmiHeader.biBitCount + 31)/32 * 4;
	//ָ��ͼ������ָ��
	BYTE* lpBits = (BYTE*)&lpDIB_FT->bmiColors[lpDIB_FT->bmiHeader.biClrUsed];

	complex<double>* origin_FD = new complex<double>[w * h];
	for(int i = 0; i < w; i++)
		origin_FD[i] = gFD[i];

	int j;
	double dis;

	for(i = 0; i < h; i++)
	{
		for(j = 0; j < w; j++)
		{
			dis = sqrt((i-h/2)*(i - h/2) + (j - w/2)*(j - w/2) + 1);
			if(D >0) // ditong
			{
				if(dis > D)
					gFD[i*w + j] = 0;
			}
			else {  //gaotong
				if(dis <= -D)
					gFD[i * w + j] = 0;
			}
		}
	}

	/*
	if(D > 0) //ditong
		gFD[i*w + j] *= 1/(1+pow(dis/D,4));
	else
		gFD[i*w + j] *= 1/(1+pow(-D/dis,4));
	
	*/

	double temp;
	BYTE* pixel;
	for(i = 0;i < h;i ++){
		for(j = 0;j < w; j++){
			pixel = lpBits + LineBytes *(h - 1 - i) + j;
			temp = sqrt(gFD[j * h + i].real() * gFD[j * h + i].real() + gFD[j * h + i].imag()*gFD[j * h + i].imag())* 2000;
			if(temp > 255)
				temp = 255;
			*pixel = (BYTE)(temp);
		}
	}
	IFFourier();

	delete gFD;
	gFD = origin_FD;
}