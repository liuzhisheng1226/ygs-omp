#include "StdAfx.h"
#include "VisualView.h"


CVisualView::CVisualView(CVisualBand* b)
{
	this->m_bIsRgb=false;
	this->m_oBand_R=b;
	this->m_oBand_G=b;
	this->m_oBand_B=b;
	this->m_oBand_R->CreateScroll();			//实例话的时候加载缩略图
	
	META_IMAGE_SCR.data=NULL;	
	META_IMAGE_IZC.data=NULL;
	
	META_IMAGE_SCR.rect=m_oBand_R->META_BAND_SCR.rect;
	META_IMAGE_SCR.data=Bytes2Vision(
		m_oBand_R->META_BAND_SCR.data,
		m_oBand_G->META_BAND_SCR.data,
		m_oBand_B->META_BAND_SCR.data,
		m_oBand_R->META_BAND_SCR.rect.Size());
	

	
}

CVisualView::CVisualView(CVisualBand* br,CVisualBand* bg,CVisualBand* bb)
{
	this->m_bIsRgb=true;
	this->m_oBand_R=br;
	this->m_oBand_G=bg;
	this->m_oBand_B=bb;

	this->m_oBand_R->CreateScroll();
	this->m_oBand_G->CreateScroll();
	this->m_oBand_B->CreateScroll();

	META_IMAGE_SCR.data=NULL;	
	META_IMAGE_IZC.data=NULL;

	META_IMAGE_SCR.rect=m_oBand_R->META_BAND_SCR.rect;
	META_IMAGE_SCR.data=Bytes2Vision(
		m_oBand_R->META_BAND_SCR.data,
		m_oBand_G->META_BAND_SCR.data,
		m_oBand_B->META_BAND_SCR.data,
		m_oBand_R->META_BAND_SCR.rect.Size());
	

}

CVisualView::~CVisualView(void)
{
	if(META_IMAGE_IZC.data!=NULL) delete META_IMAGE_IZC.data;
	if(META_IMAGE_SCR.data!=NULL) delete META_IMAGE_SCR.data;
}




bool CVisualView::Visualize(CRect rect)
{
	bool isChanged=false;
	m_oBand_R->ResetMeta(rect);
	if(m_bIsRgb)m_oBand_G->ResetMeta(rect);
	if(m_bIsRgb)m_oBand_B->ResetMeta(rect);

	if(META_IMAGE_IZC.rect!=rect)
	{
		META_IMAGE_IZC.rect=rect;
		if(META_IMAGE_IZC.data!=NULL) delete[] META_IMAGE_IZC.data;
		META_IMAGE_IZC.data=Bytes2Vision(
			m_oBand_R->META_BAND_IZC.data,
			m_oBand_G->META_BAND_IZC.data,
			m_oBand_B->META_BAND_IZC.data,
			m_oBand_R->META_BAND_IZC.rect.Size());
		isChanged=true;
	}	 
	return isChanged;
}


BYTE *CVisualView::Bytes2Vision(BYTE* br,BYTE *bg,BYTE *bb,CSize size)
{	
	//按4位补齐
	int span=(3*size.cx+3)/4*4;	
	int space=span-3*size.cx;		

	BYTE *data= new BYTE[span*size.cy];
	
	BYTE *pd=data;
	BYTE *pr=br,*pg=bg,*pb=bb;	
	for(int i=0;i<size.cy;i++)
	{
		for(int j=0;j<size.cx;j++)
		{
			*pd++=*pr++;
			*pd++=*pg++;
			*pd++=*pb++;
		}
		pd+=space;
	}	
	return data;		
}

