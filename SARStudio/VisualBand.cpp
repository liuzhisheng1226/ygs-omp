#include "StdAfx.h"
#include "VisualBand.h"
#include <stdint.h>

CVisualBand::CVisualBand(CRMGImage *rmgImage,UINT bIndex)
{
	this->META_BAND_IZC.data=NULL;

	m_oImage=rmgImage;
	m_nBandIndex=bIndex;
	m_lpBandName.Format("Band %d",m_nBandIndex+1);

	this->m_lfBoundLower=0;
	this->m_lfBoundUpper=255;
	
	META_BAND_SCR.scale=max(rmgImage->m_oHeader.Sample,rmgImage->m_oHeader.Line);
	META_BAND_SCR.scale=CONST_MAX_DIM/META_BAND_SCR.scale;
	if(META_BAND_SCR.scale>1) META_BAND_SCR.scale=1;	
	
	META_BAND_SCR.rect.SetRect(
		0,
		0,
		rmgImage->m_oHeader.Sample*META_BAND_SCR.scale,
		rmgImage->m_oHeader.Line*META_BAND_SCR.scale);

	m_bIsLoad=FALSE;	
}


CVisualBand::~CVisualBand(void)
{
	
}
 


/*
*	创建缩略图
*/
void CVisualBand::CreateScroll()
{
	if(!m_bIsLoad)
		MACRO_GENERIC_INVOKE(this->m_oImage->m_oHeader.DataType,_CreateScroll);
	m_bIsLoad=TRUE;	
}

BYTE* CVisualBand::Load(CRect rect)
{
	BYTE *data;
	MACRO_GENERIC_INVOKE(this->m_oImage->m_oHeader.DataType,_Load,rect,&data);
	return data;
}


/*
*
*@name: CreateMeta
*@func: 创建波段元数据
*@para: imgRect 	图像在原图中的位置及大小
*@para:	zomRect 	缩放图在img图中的位置及大小 
*@para:	zomScale	缩放图的缩放比例
*/
void CVisualBand::ResetMeta(CRect imgRect)
{
	//IMAGE数据 调整
	if(META_BAND_IZC.rect!=imgRect)
	{
		META_BAND_IZC.rect=imgRect;
		if(META_BAND_IZC.data!=NULL) delete[]META_BAND_IZC.data;
		META_BAND_IZC.data=Load(imgRect);
	}
}

void CVisualBand::Pixel(CPoint p,CString &lpPxValue)
{
	MACRO_GENERIC_INVOKE(this->m_oImage->m_oHeader.DataType,_Pixel,p,lpPxValue);
}


	



