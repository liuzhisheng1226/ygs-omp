#pragma once
#include "VisualFilter.h"
#include <complex>


//struct StructMETA
//{
//	BYTE			*data;			//meta数据
//	unsigned int	length;			//数据长度
//	ENUM_DATA_TYPE	type;			//数据类型
//	unsigned int	size;				//每个数据大小
//};

//元数据结构体
struct StructMeta
{
	
	BYTE		*data;				//图像Byte显示数据		
	CRect		rect;				//图像(在源数据中)位置及大小
	float		scale;				//缩放比
};

class CVisualBand
{
public:
	CVisualBand(CRMGImage *rmgImage,UINT bIndex);
	~CVisualBand(void);


public:
	static const UINT CONST_MAX_DIM=1000;
	
	CString m_lpBandName;				//波段名称
	CRMGImage *m_oImage;				//图像对象
	UINT m_nBandIndex;					//波段号
	BOOL m_bIsLoad;						//是否已经加载

	//BYTE* m_byteThumb;				//缩略图数据
	//CSize m_csThumb;					//缩略图大小

	DOUBLE m_lfBoundUpper;				///<灰度拉伸上限
	DOUBLE m_lfBoundLower;				///<灰度拉伸下限	
	DOUBLE m_lfBoundFactor;				///<灰度拉伸因子(用于减少计算)

	//float m_fScale;	
	
	StructMeta META_BAND_SCR;	
	StructMeta META_BAND_IZC; //Image Zoom Compbied

	//StructMeta META_BAND_IMG;
	//StructMeta META_BAND_ZOM;
	
		
public:

	/*
	*创建缩略图 公共调用方法
	*/
	void CreateScroll();


	/*
	*创建元数据
	*/
	void ResetMeta(CRect imgRect);
	BYTE* Load(CRect rect);
	void Pixel(CPoint p,CString &lpPxValue);


private:
	
	/*
	*创建缩略图 -- 私有泛型方法
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
		lpPgrsPrompt.Format("正在加载波段 %s",this->m_lpBandName);
		CProgressBar dlgPrgsBar(META_BAND_SCR.rect.Height(),lpPgrsPrompt);		
		
		//计算初始偏移		
		long long bandOffset=m_oImage->m_oHeader.Sample*m_oImage->m_oHeader.Line*this->m_nBandIndex;
		long long offset=0+bandOffset;
		//定位到起始位置
		fseek(fp,offset*sizeof(T),SEEK_SET);
		
		//行偏移，用于减少计算，中间变量
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
		//计算拉伸阈值
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
		//拉伸到灰度		
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
		//初始偏移
		long long offset=this->m_nBandIndex*this->m_oImage->m_oHeader.Sample*this->m_oImage->m_oHeader.Line+			//计算波段偏移
			rect.top*this->m_oImage->m_oHeader.Sample+rect.left;														//数据偏移
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
	* 将一行T型数据转化为BYTE
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
			p.y*this->m_oImage->m_oHeader.Sample+p.x;		//偏移量		
		_fseeki64(fp,offset,SEEK_SET);
		T *data=new T[cpxBase];
		fread(data,sizeof(T),cpxBase,fp);
		fclose(fp);
		if(!isCpx) lpPxValue.Format("%f",(double)data[0]);
		else lpPxValue.Format("%f+i%f",(double)data[0],(double)data[1]);

		delete[] data;
	}

	
	
};

