#include "StdAfx.h"
#include "EnviWindowSuite.h"

CEnviWindowSuite::CEnviWindowSuite(int id)
{
	//��ʼ��EnviWindow�׼�
	//�׼�id
	this->m_nId=id;
	//��ʼ��View
	this->VISUAL_VIEW=NULL;
	//��ʼ��IMG����
	WindowInit(ENVI_WINDOW_IMG,CONST_WINDOW_SIZE_IMG,id,ENUM_WINDOW_TYPE::IMG);
	//��ʼ��SCR����
	WindowInit(ENVI_WINDOW_SCR,CONST_WINDOW_SIZE_SCR,id,ENUM_WINDOW_TYPE::SCR);
	//��ʼ��ZOM����
	WindowInit(ENVI_WINDOW_ZOM,CONST_WINDOW_SIZE_ZOM,id,ENUM_WINDOW_TYPE::ZOM);
	//��ʼ�������������λ����Ϣ
	ENVI_RECT=StructEnviRect(ENVI_WINDOW_IMG,ENVI_WINDOW_SCR,ENVI_WINDOW_ZOM);
	//��ʼ��ZOM���ڱ�����
	this->m_nScale=CONST_SCALE_ZOM;	
	//��ʾ���д���
	EnviWindowShow(SW_SHOW);	
	m_bIsFiltering=false;
	LINK_WINDOW=NULL;

	//�ƶ��������
	WindowLocate(100,100,false);	


#pragma region ��ʼ������
	m_dlgColorTable.Create(IDD_DIALOG_ENVI_COLORTABLE);
	m_dlgPointPicker.Create(IDD_DIALOG_ENVI_POINTPICKER);
#pragma endregion
}


CEnviWindowSuite::~CEnviWindowSuite(void)
{
	//����
	//TODO

	UnLinkDisplay();
	ENVI_WINDOW_IMG.DestroyWindow();
	ENVI_WINDOW_SCR.DestroyWindow();
	ENVI_WINDOW_ZOM.DestroyWindow();
}


void CEnviWindowSuite::WindowInit(CDlgEnviWindow &window, int windowSize,int id,ENUM_WINDOW_TYPE windowType)
{	
	//��������״̬
	ResetState(false);
	//��������
	window.Create(IDD_DIALOG_ENVI_WINDOW);	
	//���㴰�ڱ߿��С
	CRect rectClient,rectWindow;
	window.GetClientRect(&rectClient);
	window.GetWindowRect(&rectWindow);
	int borderWidth=rectWindow.Width()-rectClient.Width();
	int borderHeight=rectWindow.Height()-rectClient.Height();
	//��ʼ������ѡ����С
	window.m_boxRect.SetRect(0,0,CONST_BOX_SIZE,CONST_BOX_SIZE);
	//���µ�������λ�úʹ�С
	window.MoveWindow(0,0,windowSize+borderWidth,windowSize+borderHeight);
	//�������ID
	window.m_nId=id;
	//��������
	window.m_eWindowType=windowType;	
	//ֲ��˵�
	window.PlantMenu();
	//�����������
	ResetState(true);
	
}

void CEnviWindowSuite::CursorValue(CPoint p,CString &lpPxValue)
{
	if(!this->VISUAL_VIEW->m_bIsRgb)
		this->VISUAL_VIEW->m_oBand_R->Pixel(p,lpPxValue);
	else
	{
		CString lpRValue,lpGValue,lpBValue;
		this->VISUAL_VIEW->m_oBand_R->Pixel(p,lpRValue);
		this->VISUAL_VIEW->m_oBand_R->Pixel(p,lpGValue);
		this->VISUAL_VIEW->m_oBand_R->Pixel(p,lpBValue);
		lpPxValue.Format("R:%,G:%s,B:%s",lpRValue,lpGValue,lpBValue);
	}
}


void CEnviWindowSuite::WindowResize()
{
	ResetBox();
	ResetView();
}

void CEnviWindowSuite::WindowLocate(int x,int y,bool isDrag)
{
	//��������״̬
	ResetState(false);
	//���´���״̬
	ENVI_RECT.Update();	

	//MoveWindow�����㴰��߿�
	if(!isDrag)
	{
		ENVI_RECT.img.window.MoveToXY(x,y);
		ENVI_WINDOW_IMG.MoveWindow(ENVI_RECT.img.window);	
	}
	x=ENVI_RECT.img.window.left;
	y=ENVI_RECT.img.window.top;
	
	ENVI_WINDOW_SCR.MoveWindow(x,y+ENVI_RECT.img.window.Height(),ENVI_RECT.scr.window.Width(),ENVI_RECT.scr.window.Height());	
	ENVI_WINDOW_ZOM.MoveWindow(x+ENVI_RECT.scr.window.Width(),y+ENVI_RECT.img.window.Height(),ENVI_RECT.zom.window.Width(),ENVI_RECT.zom.window.Height());
	
	ResetState(true);
}


void CEnviWindowSuite::ResetView()
{
	ResetState(false);
	//���´�����Ϣ
	ENVI_RECT.Update();
	//��ȡ����ͼ��С������SCR�������
	CSize thumbSize=VISUAL_VIEW->META_IMAGE_SCR.rect.Size();
	double ratio= (double)thumbSize.cx/thumbSize.cy;
	double _ratio=(double)ENVI_RECT.scr.client.Width()/ENVI_RECT.scr.client.Height();
	//����Ȳ����
	if(fabs(ratio-_ratio)>=1e-2)
	{
		ENVI_WINDOW_SCR.MoveWindow(ENVI_RECT.scr.window.left,
		ENVI_RECT.scr.window.top,
		ENVI_RECT.scr.client.Height()*ratio+ENVI_RECT.scr.window.Width()-ENVI_RECT.scr.client.Width(),
		ENVI_RECT.scr.client.Height()+ENVI_RECT.scr.window.Height()-ENVI_RECT.scr.client.Height());
	}	


#pragma region �޸Ĵ��ڱ���
	double scale=(double)ENVI_RECT.scr.client.Width()/VISUAL_VIEW->m_oBand_R->m_oImage->m_oHeader.Sample;
	this->ENVI_WINDOW_SCR.m_lpWindowTitle.Format("#%d Scroll (%f)",m_nId,scale);
	this->ENVI_WINDOW_ZOM.m_lpWindowTitle.Format("#%d Zoom [%dx]",m_nId,m_nScale);
	this->ENVI_WINDOW_IMG.m_lpWindowTitle.Format("#%d %s:%s",m_nId,this->VISUAL_VIEW->m_oBand_R->m_lpBandName,this->VISUAL_VIEW->m_oBand_R->m_oImage->GetFilename());
	if(this->VISUAL_VIEW->m_bIsRgb)	
		this->ENVI_WINDOW_IMG.m_lpWindowTitle.Format("#%d R:%s:%s,G:%s:%s,B:%s:%s",
		m_nId,
		this->VISUAL_VIEW->m_oBand_R->m_lpBandName,this->VISUAL_VIEW->m_oBand_R->m_oImage->GetFilename(),
		this->VISUAL_VIEW->m_oBand_G->m_lpBandName,this->VISUAL_VIEW->m_oBand_G->m_oImage->GetFilename(),
		this->VISUAL_VIEW->m_oBand_B->m_lpBandName,this->VISUAL_VIEW->m_oBand_B->m_oImage->GetFilename());
#pragma endregion �޸�ÿ�����ڵı���
#pragma region ����Boxλ�á���С
	ResetBox();
	int left=floor(ENVI_WINDOW_SCR.m_boxRect.left/scale);
	if(left+ENVI_RECT.img.client.Width()>VISUAL_VIEW->m_oBand_R->m_oImage->m_oHeader.Sample) left=VISUAL_VIEW->m_oBand_R->m_oImage->m_oHeader.Sample-ENVI_RECT.img.client.Width();
	int top=floor(ENVI_WINDOW_SCR.m_boxRect.top/scale);
	if(top+ENVI_RECT.img.client.Height()>VISUAL_VIEW->m_oBand_R->m_oImage->m_oHeader.Line) top=VISUAL_VIEW->m_oBand_R->m_oImage->m_oHeader.Line-ENVI_RECT.img.client.Height();
	ENVI_RECT.img.client.MoveToXY(left,top);
#pragma endregion
#pragma region ������ʾ����
	bool isChanged=VISUAL_VIEW->Visualize(ENVI_RECT.img.client);	
	//if(isChanged)
	
		//SCR		
	delete[]this->ENVI_WINDOW_SCR.m_dispData;
	this->ENVI_WINDOW_SCR.m_dispSize.SetSize(this->VISUAL_VIEW->META_IMAGE_SCR.rect.Width(),this->VISUAL_VIEW->META_IMAGE_SCR.rect.Height());		
	this->ENVI_WINDOW_SCR.m_dispRect.SetRect(0,0,ENVI_RECT.scr.client.Width(),ENVI_RECT.scr.client.Height());
	this->ENVI_WINDOW_SCR.m_dispData=Filtering(this->VISUAL_VIEW->META_IMAGE_SCR.data,VISUAL_VIEW->META_IMAGE_SCR.rect.Size());
	this->ENVI_WINDOW_SCR.Invalidate(false);
	
	//IMG
	delete[]this->ENVI_WINDOW_IMG.m_dispData;	
	this->ENVI_WINDOW_IMG.m_dispSize=ENVI_RECT.img.client.Size();
	this->ENVI_WINDOW_IMG.m_dispRect.CopyRect(ENVI_RECT.img.client);
	//this->ENVI_WINDOW_IMG.m_dispRect.SetRect(0,0,ENVI_RECT.img.client.Width(),ENVI_RECT.img.client.Height());
	this->ENVI_WINDOW_IMG.m_dispData=Filtering( VISUAL_VIEW->META_IMAGE_IZC.data,this->ENVI_WINDOW_IMG.m_dispSize);

	this->ENVI_WINDOW_IMG.Invalidate(false);
	//ZOM
	delete[]this->ENVI_WINDOW_ZOM.m_dispData;
	this->ENVI_WINDOW_ZOM.m_dispSize=ENVI_RECT.img.client.Size();
	this->ENVI_WINDOW_ZOM.m_dispRect.SetRect(0,0,ENVI_RECT.img.client.Width()*m_nScale,ENVI_RECT.img.client.Height()*m_nScale);
	this->ENVI_WINDOW_ZOM.m_dispRect.MoveToXY(-ENVI_WINDOW_IMG.m_boxRect.left*m_nScale,ENVI_WINDOW_IMG.m_boxRect.top*m_nScale);
	this->ENVI_WINDOW_ZOM.m_dispData=Filtering(VISUAL_VIEW->META_IMAGE_IZC.data,this->ENVI_WINDOW_ZOM.m_dispSize);
	this->ENVI_WINDOW_ZOM.Invalidate(false);
#pragma endregion  
	ResetState(true);
	
}
/*
* ��ʾ����
*/
void CEnviWindowSuite::ResetView(CVisualView* view)
{
	if(VISUAL_VIEW!=NULL&&VISUAL_VIEW!=view) delete VISUAL_VIEW;
	this->VISUAL_VIEW=view;	

	ResetView();
}
void CEnviWindowSuite::Zoom(int way)
{
	if(way<=0) m_nScale--;
	if(way>0) m_nScale++;
	if(m_nScale<1) m_nScale=1;
	if(m_nScale>=CONST_SCALE_MAX) m_nScale=CONST_SCALE_MAX;
	
	ResetView();
}




/*
*@name:	ResetState
*@func:	���ô���״̬
*@para:	flag  ״̬
*/
void CEnviWindowSuite::ResetState(bool flag)
{
	ENVI_WINDOW_IMG.m_bIsChangeable=flag;
	ENVI_WINDOW_SCR.m_bIsChangeable=flag;
	ENVI_WINDOW_ZOM.m_bIsChangeable=flag;
}

void CEnviWindowSuite::ResetBox()
{
	// ���ݴ��ڴ�С����Box��С
	// ����SCR���ڳ������
	StructEnviRect rect(ENVI_WINDOW_IMG,ENVI_WINDOW_SCR,ENVI_WINDOW_ZOM);

	CSize size(VISUAL_VIEW->m_oBand_R->m_oImage->m_oHeader.Sample,										//ԭʼͼ���С
		VISUAL_VIEW->m_oBand_R->m_oImage->m_oHeader.Line);

	double xRatio=(double)rect.img.client.Width()/size.cx;											//IMG������ԭʼͼ���ȱ�ֵ
	double yRatio=(double)rect.img.client.Height()/size.cy;											//IMG������ԭʼͼ�񳤶ȱ�ֵ
	//IMG=>SCR�� Box��С
	ENVI_WINDOW_SCR.m_boxRect.SetRect(
		ENVI_WINDOW_SCR.m_boxRect.left,
		ENVI_WINDOW_SCR.m_boxRect.top,
		ENVI_WINDOW_SCR.m_boxRect.left+floor(xRatio*rect.scr.client.Width()),
		ENVI_WINDOW_SCR.m_boxRect.top+floor(yRatio*rect.scr.client.Height()));	
	//ZOM=>IMG�� Box��С
	ENVI_WINDOW_IMG.m_boxRect.SetRect(
		ENVI_WINDOW_IMG.m_boxRect.left,
		ENVI_WINDOW_IMG.m_boxRect.top,
		ENVI_WINDOW_IMG.m_boxRect.left+rect.zom.client.Width()/m_nScale,
		ENVI_WINDOW_IMG.m_boxRect.top+rect.zom.client.Height()/m_nScale);
	


	
	ENVI_WINDOW_IMG.BoxValid();

}



void CEnviWindowSuite::EnviWindowShow(bool show)
{
	ENVI_WINDOW_IMG.ShowWindow(show);
	ENVI_WINDOW_SCR.ShowWindow(show);
	ENVI_WINDOW_ZOM.ShowWindow(show);
}


void CEnviWindowSuite::LinkDisplay(int cur)
{
	CEnviStyleManager::Instance()->Lock();
	if(CEnviStyleManager::Instance()->EnviWindowCount()<2)
	{
		AfxMessageBox("����������ϴ���");
		CEnviStyleManager::Instance()->UnLock();
		return;
	}
	
	CDlgEnviLinkDisplay dlg(AfxGetApp()->m_pMainWnd);
	dlg.m_disp1Window=cur;
	if(dlg.DoModal()==IDOK)
	{
		//�������Ӵ���
		LINK_WINDOW=CEnviStyleManager::Instance()->EnviWindowFinder(dlg.m_disp2Window);
		CEnviStyleManager::Instance()->EnviWindowFinder(dlg.m_disp2Window)->LINK_WINDOW=this;
		LinkUpdate();		
	}
	CEnviStyleManager::Instance()->UnLock();
}
void CEnviWindowSuite::LinkUpdate()
{
	if(LINK_WINDOW==NULL) return;
	LINK_WINDOW->ENVI_WINDOW_IMG.m_boxRect=this->ENVI_WINDOW_IMG.m_boxRect;
	LINK_WINDOW->ENVI_WINDOW_SCR.m_boxRect=this->ENVI_WINDOW_SCR.m_boxRect;
	this->ResetView();
	LINK_WINDOW->ResetView();
}
void CEnviWindowSuite::LinkSwap()
{
	if(LINK_WINDOW==NULL) return;
	delete[]this->ENVI_WINDOW_IMG.m_dispData;
	this->ENVI_WINDOW_IMG.m_dispData=LINK_WINDOW->ENVI_WINDOW_IMG.m_dispData;
	this->ENVI_WINDOW_IMG.m_dispRect=LINK_WINDOW->ENVI_WINDOW_IMG.m_dispRect;
	this->ENVI_WINDOW_IMG.m_dispSize=LINK_WINDOW->ENVI_WINDOW_IMG.m_dispSize;
	this->ENVI_WINDOW_IMG.Invalidate(false);
}

void CEnviWindowSuite::LinkRestore()
{
	if(LINK_WINDOW==NULL) return;	
	this->ENVI_WINDOW_IMG.m_dispSize=ENVI_RECT.img.client.Size();
	this->ENVI_WINDOW_IMG.m_dispRect.SetRect(0,0,ENVI_RECT.img.client.Width(),ENVI_RECT.img.client.Height());
	this->ENVI_WINDOW_IMG.m_dispData=Filtering( VISUAL_VIEW->META_IMAGE_IZC.data,this->ENVI_WINDOW_IMG.m_dispSize);
	this->ENVI_WINDOW_IMG.Invalidate(false);
}
void CEnviWindowSuite::UnLinkDisplay()
{
	if(LINK_WINDOW!=NULL&&LINK_WINDOW->LINK_WINDOW!=NULL)
		this->LINK_WINDOW->LINK_WINDOW=NULL;
	this->LINK_WINDOW=NULL;
}


//void CEnviWindowSuite::Destroy()
//{
//	ENVI_WINDOW_IMG.DestroyWindow();
//	ENVI_WINDOW_SCR.DestroyWindow();
//	ENVI_WINDOW_ZOM.DestroyWindow();
//}

void CEnviWindowSuite::Enable(bool flag)
{
	ENVI_WINDOW_IMG.EnableWindow(flag);
	ENVI_WINDOW_SCR.EnableWindow(flag);
	ENVI_WINDOW_ZOM.EnableWindow(flag);
}



byte* CEnviWindowSuite::Filtering(byte* data,CSize size)
{
	//��4λ����
	int span=(3*size.cx+3)/4*4;	
	int space=span-3*size.cx;		

	BYTE *_data= new BYTE[span*size.cy];	
	memcpy(_data,data,sizeof(byte)*span*size.cy);	

	if(!m_bIsFiltering) return _data;
	
	BYTE *pd=data;	
	BYTE *_pd=_data;
	for(int i=0;i<size.cy;i++)
	{
		for(int j=0;j<size.cx;j++)
		{
			*_pd++=m_ColorTable.color[*pd++].r;
			*_pd++=m_ColorTable.color[*pd++].g;
			*_pd++=m_ColorTable.color[*pd++].b;
		}
		pd+=space;
		_pd+=space;
	}	
	return _data;
}

bool CEnviWindowSuite::IsInclude(CString lpFilename)
{
	return VISUAL_VIEW->m_oBand_R->m_oImage->m_lpFullname==lpFilename||
			VISUAL_VIEW->m_oBand_G->m_oImage->m_lpFullname==lpFilename||
			VISUAL_VIEW->m_oBand_B->m_oImage->m_lpFullname==lpFilename;
}



#pragma region ����--ColorTable�¼�
void CEnviWindowSuite::ColorTable()
{
	m_dlgColorTable.ShowWindow(TRUE);
	m_dlgColorTable.m_nId=this->m_nId;
}

void CEnviWindowSuite::ColorMapping(StructColorTable colorTable)
{
	this->m_ColorTable=colorTable;
	this->m_bIsFiltering=true;	
	this->ResetView();

}

void CEnviWindowSuite::ColorMapping()
{
	this->m_bIsFiltering=false;
	this->ResetView();
}

#pragma endregion
void CEnviWindowSuite::PointPicker()
{
	m_bIsPointPicker=true;
	m_dlgPointPicker.ShowWindow(TRUE);
}
#pragma region ����--PointPicker


