#pragma once
#include "VisualFilter.h"
#include <complex>


//struct StructMETA
//{
//	BYTE			*data;			//meta����
//	unsigned int	length;			//���ݳ���
//	ENUM_DATA_TYPE	type;			//��������
//	unsigned int	size;				//ÿ�����ݴ�С
//};

//Ԫ���ݽṹ��
struct StructMeta
{
	
	BYTE		*data;				//ͼ��Byte��ʾ����		
	CRect		rect;				//ͼ��(��Դ������)λ�ü���С
	float		scale;				//���ű�
};

class CVisualBand
{
public:
	CVisualBand(CRMGImage *rmgImage,UINT bIndex);
	~CVisualBand(void);


public:
	static const UINT CONST_MAX_DIM=1000;
	
	CString m_lpBandName;				//��������
	CRMGImage *m_oImage;				//ͼ�����
	UINT m_nBandIndex;					//���κ�
	BOOL m_bIsLoad;						//�Ƿ��Ѿ�����

	//BYTE* m_byteThumb;				//����ͼ����
	//CSize m_csThumb;					//����ͼ��С

	DOUBLE m_lfBoundUpper;				///<�Ҷ���������
	DOUBLE m_lfBoundLower;				///<�Ҷ���������	
	DOUBLE m_lfBoundFactor;				///<�Ҷ���������(���ڼ��ټ���)

	//float m_fScale;	
	
	StructMeta META_BAND_SCR;	
	StructMeta META_BAND_IZC; //Image Zoom Compbied

	//StructMeta META_BAND_IMG;
	//StructMeta META_BAND_ZOM;
	
		
public:

	/*
	*��������ͼ �������÷���
	*/
	void CreateScroll();


	/*
	*����Ԫ����
	*/
	void ResetMeta(CRect imgRect);
	BYTE* Load(CRect rect);
	void Pixel(CPoint p,CString &lpPxValue);


private:
	
	/*
	*��������ͼ -- ˽�з��ͷ���
	*/
	template<class T>
	void _CreateScroll()
	{
		long length=META_BAND_SCR.rect.Width()*META_BAND_SCR.rect.Height();
		bool isCpx=this->m_oImage->m_oHeader.DataType>=ENUM_DATA_TYPE::eCINT16?true:false;
		int dataSize=isCpx?2:1;
		double *data=new double[length];
		T val1;
		T val2;
		double fnum=0;
		FILE *fp=NULL;
		fp=fopen(this->m_oImage->m_lpFullname,"rb+");						
		
		CString lpPgrsPrompt;
		lpPgrsPrompt.Format("���ڼ��ز��� %s",this->m_lpBandName);
		CProgressBar dlgPrgsBar(META_BAND_SCR.rect.Height(),lpPgrsPrompt);		
		
		//�����ʼƫ��		
		long long bandOffset=m_oImage->m_oHeader.Sample*m_oImage->m_oHeader.Line*this->m_nBandIndex;
		long long offset=0+bandOffset;
		//��λ����ʼλ��
		fseek(fp,offset*sizeof(T),SEEK_SET);
		
		//��ƫ�ƣ����ڼ��ټ��㣬�м����
		long long lineOffset=0;
		for(int i=0;i<META_BAND_SCR.rect.Height();i++)
		{
			dlgPrgsBar.StepIt();
			lineOffset=int(i/META_BAND_SCR.scale)*m_oImage->m_oHeader.Sample;
			for(int j=0;j<META_BAND_SCR.rect.Width();j++) 
			{				
				offset=(lineOffset+int(j/META_BAND_SCR.scale)+bandOffset)*sizeof(T)*dataSize;				
				_fseeki64(fp,offset,SEEK_SET);
				fnum=0;
				fread(&val1,sizeof(T),1,fp);
				fnum=(double)val1;
				if(isCpx)
				{
					fread(&val2,sizeof(T),1,fp);
					fnum=sqrt(fnum*fnum+val2*val2);
				}
				if(_isnan(fnum)) fnum=0;
				data[i*this->META_BAND_SCR.rect.Width()+j]=fnum;
			}//for j			
		}//for i		
		fclose(fp);
		
		double sum=0,sumSquare=0,mean=0,var=0,stdev=0;
		//����������ֵ
		for(int i=0;i<length;i++)
		{
			sum+=data[i];
			sumSquare+=data[i]*data[i];
		}//for i
		mean=sum/length;
		var=sumSquare/length-mean*mean;
		stdev=sqrt(var);

		this->m_lfBoundLower=mean-stdev;
		this->m_lfBoundUpper=mean+stdev;
		this->m_lfBoundFactor=255.0/(m_lfBoundUpper-m_lfBoundLower);
		//m_byteThumb=new BYTE[thumbLength];
		META_BAND_SCR.data=new BYTE[length];
		//���쵽�Ҷ�		
		for(int i=0;i<length;i++)		
			META_BAND_SCR.data[i]=CVisualFilter::Convert2Grayscale(data[i],m_lfBoundLower,m_lfBoundUpper,m_lfBoundFactor);
		
		delete[] data;		
	}//_CreateThumb
	

	template<class T>
	void _Load(CRect rect,BYTE** bytes)
	{		
		bool isCpx=this->m_oImage->m_oHeader.DataType>=ENUM_DATA_TYPE::eCINT16?true:false;
		int cpxBase=isCpx?2:1;
		FILE *fp=fopen(this->m_oImage->m_lpFullname,"rb+");		
		T* data=new T[rect.Width()*cpxBase];
		*bytes=new BYTE[rect.Width()*rect.Height()];
		//��ʼƫ��
		long long offset=this->m_nBandIndex*this->m_oImage->m_oHeader.Sample*this->m_oImage->m_oHeader.Line+			//���㲨��ƫ��
			rect.top*this->m_oImage->m_oHeader.Sample+rect.left;														//����ƫ��
		_fseeki64(fp,offset*sizeof(T)*cpxBase,SEEK_SET);
		for(int i=0;i<rect.Height();i++)
		{
			fread(data,sizeof(T),rect.Width()*cpxBase,fp);			
			BytesConvertor<T>(*bytes+i*rect.Width(),data,rect.Width(),isCpx);
			_fseeki64(fp,(this->m_oImage->m_oHeader.Sample-rect.Width())*sizeof(T)*cpxBase,SEEK_CUR);
		}//for i	
		fclose(fp);
		delete[] data;
	}



	/*
	* ��һ��T������ת��ΪBYTE
	*/
	template<class T>
	void BytesConvertor(BYTE* bytes,T* data,unsigned int length,bool isCpx)
	{		
		
		double val1,val2;
		int index=0;		
		for(int i=0;i<length;i++)
		{
			val1=(double)data[index++];			
			if(isCpx)val1=sqrt((double)(val1*val1+data[index]*data[index++]));			
			bytes[i]=CVisualFilter::Convert2Grayscale(val1,m_lfBoundLower,m_lfBoundUpper,m_lfBoundFactor);
		}//for i		
	}


	template<class T>
	void _Pixel(CPoint p,CString &lpPxValue)
	{
		bool isCpx=this->m_oImage->m_oHeader.DataType>=ENUM_DATA_TYPE::eCINT16?true:false;
		int cpxBase=isCpx?2:1;

		FILE *fp=fopen(this->m_oImage->m_lpFullname,"rb+");		
		long long offset=this->m_nBandIndex*this->m_oImage->m_oHeader.Sample*this->m_oImage->m_oHeader.Line+			
			p.y*this->m_oImage->m_oHeader.Sample+p.x;		//ƫ����		
		_fseeki64(fp,offset,SEEK_SET);
		T *data=new T[cpxBase];
		fread(data,sizeof(T),cpxBase,fp);
		fclose(fp);
		if(!isCpx) lpPxValue.Format("%f",(double)data[0]);
		else lpPxValue.Format("%f+i%f",(double)data[0],(double)data[1]);

		delete[] data;
	}

	
	
};

