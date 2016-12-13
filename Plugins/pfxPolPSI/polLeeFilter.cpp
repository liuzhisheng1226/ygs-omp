#include "stdafx.h"
#include "polLeeFilter.h"
#include "DlgPolLeeFilter.h"
#include <complex>

CpolLeeFilter::CpolLeeFilter(void)
{
}


CpolLeeFilter::~CpolLeeFilter(void)
{
}

void CpolLeeFilter::Process()
{
	CDlgPolLeeFilter dlg;
	if(dlg.DoModal()!=IDOK)
		return;

	RefineLee(dlg.m_strInT11,dlg.m_strInT12,dlg.m_strInT13
			, dlg.m_strInT22,dlg.m_strInT23,dlg.m_strInT33
			, dlg.m_strOutT11,dlg.m_strOutT12,dlg.m_strOutT13
			, dlg.m_strOutT22,dlg.m_strOutT23,dlg.m_strOutT33
			, dlg.m_iWin, dlg.m_fLooks);

}

void CpolLeeFilter::Batch()
{
	int iWin = atoi(m_aryStrs.GetAt(12));
	float fLook  = atof(m_aryStrs.GetAt(13));
	RefineLee( m_aryStrs.GetAt(0),  m_aryStrs.GetAt(1),  m_aryStrs.GetAt(2)
		    ,  m_aryStrs.GetAt(3),  m_aryStrs.GetAt(4),  m_aryStrs.GetAt(5)
			,  m_aryStrs.GetAt(6),  m_aryStrs.GetAt(7),  m_aryStrs.GetAt(8)
			,  m_aryStrs.GetAt(9),  m_aryStrs.GetAt(10), m_aryStrs.GetAt(11)
			,  iWin, fLook);
}

void CpolLeeFilter::RefineLee(CString strInT11,CString strInT12,CString strInT13
			      ,CString strInT22,CString strInT23,CString strInT33
				  ,CString strOutT11,CString strOutT12,CString strOutT13
			      ,CString strOutT22,CString strOutT23,CString strOutT33
				  ,int iWin, float fLook)
{
	CRMGImage inImg(strInT11);
	int imgW = inImg.m_oHeader.Sample;
	int imgH = inImg.m_oHeader.Line;

	CFile inT11File,inT12File, inT13File, inT22File, inT23File, inT33File;
	CFile outT11File,outT12File, outT13File, outT22File, outT23File, outT33File;

	CFileException ex;
	if(!inT11File.Open(strInT11,CFile::modeRead | CFile::shareDenyWrite, &ex))
	{
		TCHAR szError[1024];
		ex.GetErrorMessage(szError, 1024);
		_tprintf_s(_T("Couldn't open source file: %1024s"), szError);
		return;
	}
	if(!inT12File.Open(strInT12,CFile::modeRead | CFile::shareDenyWrite, &ex))
	{
		TCHAR szError[1024];
		ex.GetErrorMessage(szError, 1024);
		_tprintf_s(_T("Couldn't open source file: %1024s"), szError);
		inT11File.Close();
		return;
	}
	if(!inT13File.Open(strInT13,CFile::modeRead | CFile::shareDenyWrite, &ex))
	{
		TCHAR szError[1024];
		ex.GetErrorMessage(szError, 1024);
		_tprintf_s(_T("Couldn't open source file: %1024s"), szError);
		inT11File.Close();
		inT12File.Close();
		return;
	}
	if(!inT22File.Open(strInT22,CFile::modeRead | CFile::shareDenyWrite, &ex))
	{
		TCHAR szError[1024];
		ex.GetErrorMessage(szError, 1024);
		_tprintf_s(_T("Couldn't open source file: %1024s"), szError);
		inT11File.Close();
		inT12File.Close();
		inT13File.Close();
		return;
	}
	if(!inT23File.Open(strInT23,CFile::modeRead | CFile::shareDenyWrite, &ex))
	{
		TCHAR szError[1024];
		ex.GetErrorMessage(szError, 1024);
		_tprintf_s(_T("Couldn't open source file: %1024s"), szError);
		inT11File.Close();
		inT12File.Close();
		inT13File.Close();
		inT22File.Close();
		return;
	}
	if(!inT33File.Open(strInT33,CFile::modeRead | CFile::shareDenyWrite, &ex))
	{
		TCHAR szError[1024];
		ex.GetErrorMessage(szError, 1024);
		_tprintf_s(_T("Couldn't open source file: %1024s"), szError);
		inT11File.Close();
		inT12File.Close();
		inT13File.Close();
		inT22File.Close();
		inT23File.Close();
		return;
	}

	if(!outT11File.Open(strOutT11, CFile::modeWrite | CFile::typeBinary |
		   CFile::shareExclusive | CFile::modeCreate, &ex))
	{
		TCHAR szError[1024];
		ex.GetErrorMessage(szError, 1024);
		_tprintf_s(_T("Couldn't open source file: %1024s"), szError);
		return;
	}
	if(!outT12File.Open(strOutT12, CFile::modeWrite | CFile::typeBinary |
		   CFile::shareExclusive | CFile::modeCreate, &ex))
	{
		TCHAR szError[1024];
		ex.GetErrorMessage(szError, 1024);
		_tprintf_s(_T("Couldn't open source file: %1024s"), szError);
		outT11File.Close();
		return;
	}
	if(!outT13File.Open(strOutT13, CFile::modeWrite | CFile::typeBinary |
		   CFile::shareExclusive | CFile::modeCreate, &ex))
	{
		TCHAR szError[1024];
		ex.GetErrorMessage(szError, 1024);
		_tprintf_s(_T("Couldn't open source file: %1024s"), szError);
		outT11File.Close();
		outT12File.Close();
		return;
	}
	if(!outT22File.Open(strOutT22, CFile::modeWrite | CFile::typeBinary |
		   CFile::shareExclusive | CFile::modeCreate, &ex))
	{
		TCHAR szError[1024];
		ex.GetErrorMessage(szError, 1024);
		_tprintf_s(_T("Couldn't open source file: %1024s"), szError);
		outT11File.Close();
		outT12File.Close();
		outT13File.Close();
		return;
	}
	if(!outT23File.Open(strOutT23, CFile::modeWrite | CFile::typeBinary |
		   CFile::shareExclusive | CFile::modeCreate, &ex))
	{
		TCHAR szError[1024];
		ex.GetErrorMessage(szError, 1024);
		_tprintf_s(_T("Couldn't open source file: %1024s"), szError);
		outT11File.Close();
		outT12File.Close();
		outT13File.Close();
		outT22File.Close();
		return;
	}
	if(!outT33File.Open(strOutT33, CFile::modeWrite | CFile::typeBinary |
		   CFile::shareExclusive | CFile::modeCreate, &ex))
	{
		TCHAR szError[1024];
		ex.GetErrorMessage(szError, 1024);
		_tprintf_s(_T("Couldn't open source file: %1024s"), szError);
		outT11File.Close();
		outT12File.Close();
		outT13File.Close();
		outT22File.Close();
		outT23File.Close();
		return;
	}
	float *inT11Data = new float[imgW*imgH];
	inT11File.Read(inT11Data,sizeof(float)*imgW*imgH);
	float *inT22Data = new float[imgW*imgH];
	inT22File.Read(inT22Data,sizeof(float)*imgW*imgH);
	float *inT33Data = new float[imgW*imgH];
	inT33File.Read(inT33Data,sizeof(float)*imgW*imgH);

	complex<float> *inT12Data = new complex<float>[imgW*imgH];
	inT12File.Read(inT12Data,sizeof(complex<float>)*imgW*imgH);
	complex<float> *inT13Data = new complex<float>[imgW*imgH];
	inT13File.Read(inT13Data,sizeof(complex<float>)*imgW*imgH);
	complex<float> *inT23Data = new complex<float>[imgW*imgH];
	inT23File.Read(inT23Data,sizeof(complex<float>)*imgW*imgH);

	float *outT11Data = new float[imgW];
	float *outT22Data = new float[imgW];
	float *outT33Data = new float[imgW];
	complex<float> *outT12Data = new complex<float>[imgW];
	complex<float> *outT13Data = new complex<float>[imgW];
	complex<float> *outT23Data = new complex<float>[imgW];

	inT11File.Close();	inT12File.Close();	inT13File.Close();
	inT22File.Close();	inT23File.Close();	inT33File.Close();

	float *span = new float[imgW*imgH];
	
	float *buffer = new float[iWin*imgW];

	int i,j,index;
	//得到功率图像以求得滤波系数
	for (i=0; i<imgH; i++)
	{
		for (j=0; j<imgW; j++)
		{
			index = i*imgW+j;
			span[index] = inT11Data[index]+inT22Data[index]+inT33Data[index];	
		}//for
	}

	int direct[4][3][3]=
	{
		{-1,0,1,
			-1,0,1,
			-1,0,1},
			
		{0,1,1,
		-1,0,1,
		-1,-1,0},
		
		{1,1,1,
		0,0,0,
		-1,-1,-1},
		
		{1,1,0,
		1,0,-1,
		0,-1,-1}
	};	
	
	//分配内存并初始化为0
	int ***Mask = new int**[8];
	for(i=0;i<8;i++)
	{
		Mask[i]=new int*[iWin];
		for(j=0;j<iWin;j++)
		{
			Mask[i][j]=new int[iWin];
			ZeroMemory(Mask[i][j],sizeof(int)*iWin);
		}
	}
	
	//生成MASK
	make_Mask(Mask,iWin);
	
	//滤波开始
	int k,l;
	for (i=0; i<imgH; i++)
	{
		for (j=0; j<imgW; j++)
		{
			/* Gradient window calculation parameters */
			int nwin=1;
			float grad[4]={0};
			for (k=-nwin; k<=nwin; k++)
			{
				if ((i+k)<0 || (i+k)>=imgH)
				{
					continue;
				}
				for (l=-nwin; l<=nwin; l++)
				{
					if ((j+l)<0 || (j+l)>=imgW)
					{
						continue;
					}

					index=(i+k)*imgW+(j+l);
					grad[0]+=2*span[index]*direct[0][k+nwin][l+nwin];
					grad[1]+=2*span[index]*direct[1][k+nwin][l+nwin];
					grad[2]+=2*span[index]*direct[2][k+nwin][l+nwin];
					grad[3]+=2*span[index]*direct[3][k+nwin][l+nwin];
				}
			}
			//找出梯度绝对值最大的方向
			float MaxGrad=-99999.9;
			int MaxIndex=0;
			for(k=0;k<4;k++)
			{
				if(MaxGrad<fabs(grad[k]))
				{
					MaxGrad=fabs(grad[k]);
					MaxIndex=k;
				}
			}
			
			if(grad[MaxIndex]>0) MaxIndex+=4;
			
			//滤波窗口内的统计计算
			int nCount = 0;
			float span_m1=0, span_m2=0;
			float t11=0,t22=0,t33=0;
			complex<float> t12 = complex<float>(0.0,0.0);
			complex<float> t13 = complex<float>(0.0,0.0);
			complex<float> t23 = complex<float>(0.0,0.0);
							
			for (k=-iWin/2; k<=iWin/2; k++)
			{
				if ((i+k)<0 || (i+k)>=imgH)
				{
					continue;
				}
				for (l=-iWin/2; l<=iWin/2; l++)
				{
					if (!Mask[MaxIndex][k+ iWin/2][l+ iWin/2]
						|| (j+l)<0 || (j+l)>=imgW)
					{
						continue;
					}
					
					index = (i+k)* imgW +j+l;
					float tmpspan=span[index];
					span_m1 += tmpspan;
					span_m2 += tmpspan*tmpspan;
					
					t11 +=inT11Data[index];
					t22 +=inT22Data[index];
					t33 +=inT33Data[index];

					t12 +=inT12Data[index];
					t13 +=inT13Data[index];
					t23 +=inT23Data[index];

					//t11 += pMapAddress[iSize*index+e11];
					//t22 += pMapAddress[iSize*index+e22];
					//t33 += pMapAddress[iSize*index+e33];
					//t12_re += pMapAddress[iSize*index+e12_re];
					//t12_im += pMapAddress[iSize*index+e12_im];
					//t13_re += pMapAddress[iSize*index+e13_re];
					//t13_im += pMapAddress[iSize*index+e13_im];
					//t23_re += pMapAddress[iSize*index+e23_re];
					//t23_im += pMapAddress[iSize*index+e23_im];
					
					nCount++;
				}//for l
			}//for k
			
			t11 /= nCount;	t22 /= nCount;	t33 /= nCount;
			t12 /= nCount;	t13 /= nCount;	t23 /= nCount;
			
			span_m1 /= nCount;
			span_m2 /= nCount;
			
			float sigma2 = 1.0 / fLook;
			
			//用于滤波的系数
			float cv=span_m2/(span_m1*span_m1);
			
			float coeff=(cv-sigma2)/(cv*(1+sigma2));
			
			if(coeff>1)
				coeff=1;
			else if(coeff<0)
				coeff=0;
			
			//对相干矩阵中的各元素进行滤波
			index = i*imgW+j;
			
			float tmpResult;
			//result = t11 + coeff*(pMapAddress[iSize*index+e11]-t11);
			tmpResult = t11 + coeff*(inT11Data[index]-t11);
			tmpResult = (tmpResult>0)?tmpResult:0;
			outT11Data[j] = tmpResult;
			
			tmpResult = t22 + coeff*(inT22Data[index]-t22);
			tmpResult = (tmpResult>0)?tmpResult:0;
			outT22Data[j] = tmpResult;
			
			
			tmpResult = t33 + coeff*(inT33Data[index]-t33);
			tmpResult = (tmpResult>0)?tmpResult:0;
			outT33Data[j] = tmpResult;

			outT12Data[j] = t12 + coeff*(inT12Data[index]-t12);	
			outT13Data[j] = t13 + coeff*(inT13Data[index]-t13);	
			outT23Data[j] = t23 + coeff*(inT23Data[index]-t23);	
						
		}//for j
		
		outT11File.Write(outT11Data,sizeof(float)*imgW);
		outT22File.Write(outT22Data,sizeof(float)*imgW);
		outT33File.Write(outT33Data,sizeof(float)*imgW);

		outT12File.Write(outT12Data,sizeof(complex<float>)*imgW);
		outT13File.Write(outT13Data,sizeof(complex<float>)*imgW);
		outT23File.Write(outT23Data,sizeof(complex<float>)*imgW);
	}//for i

	delete[] inT11Data;	delete[] inT22Data;	delete[] inT33Data;
	delete[] inT12Data;	delete[] inT13Data;	delete[] inT23Data;

	delete[] outT11Data;	delete[] outT22Data;	delete[] outT33Data;
	delete[] outT12Data;	delete[] outT13Data;	delete[] outT23Data;

	outT11File.Close();	outT22File.Close();	outT33File.Close();
	outT11File.Close();	outT22File.Close();	outT33File.Close();

	CRMGHeader header(inImg.m_oHeader);
	//header.Band=1;
	//header.DataType = eFLOAT32;
	strInT11.Replace(".rmg",".ldr");
	header.Save(strInT11);

	strInT22.Replace(".rmg",".ldr");
	header.Save(strInT22);
	strInT33.Replace(".rmg",".ldr");
	header.Save(strInT33);

	//header.DataType = eCFLOAT32;
	strInT12.Replace(".rmg",".ldr");
	header.Save(strInT12);
	strInT13.Replace(".rmg",".ldr");
	header.Save(strInT13);
	strInT23.Replace(".rmg",".ldr");
	header.Save(strInT23);

	return;


}



//根据窗口大小产生相应的Mask
void CpolLeeFilter::make_Mask(int ***Mask, int Nwin)
{
	int k,l,Nmax;
	
	for (k=0; k<Nwin; k++)
		for (l=0; l<Nwin; l++)
			for (Nmax=0; Nmax<8; Nmax++)
				Mask[Nmax][k][l]=0;
			
	Nmax = 0;
	for (k=0; k<Nwin; k++)
		for (l=(Nwin-1)/2; l<Nwin; l++)
			Mask[Nmax][k][l]=1;
				
	Nmax = 4;
	for (k=0; k<Nwin; k++)
		for (l=0; l<1+(Nwin-1)/2; l++)
			Mask[Nmax][k][l]=1;
					
	Nmax = 1;
	for (k=0; k<Nwin; k++)
		for (l=k; l<Nwin; l++)
			Mask[Nmax][k][l]=1;
						
	Nmax = 5;
	for (k=0; k<Nwin; k++)
		for (l=0; l<k+1; l++)
				Mask[Nmax][k][l]=1;
							
	Nmax = 2;
	for (k=0; k<1+(Nwin-1)/2; k++)
		for (l=0; l<Nwin; l++)
			Mask[Nmax][k][l]=1;
								
	Nmax = 6;
	for (k=(Nwin-1)/2; k<Nwin; k++)
		for (l=0; l<Nwin; l++)
			Mask[Nmax][k][l]=1;
									
	Nmax = 3;
	for (k=0; k<Nwin; k++)
		for (l=0; l<Nwin-k; l++)
			Mask[Nmax][k][l]=1;
										
	Nmax = 7;
	for (k=0; k<Nwin; k++)
		for (l=Nwin-1-k; l<Nwin; l++)
			Mask[Nmax][k][l]=1;
}