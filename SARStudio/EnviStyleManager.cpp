#include "StdAfx.h"
#include "EnviStyleManager.h"
#include "VisualBand.h"



CEnviStyleManager* CEnviStyleManager::_instance=NULL;

CEnviStyleManager::CEnviStyleManager(void)
{
	//TODO ��ɳ�ʼ������

	_dlgEnviBandList=new CDlgEnviBandList();
	_dlgEnviBandList->Create(IDD_DIALOG_ENVI_FILE_LIST);
	
	_dlgEnviCursorValue=new CDlgEnviCursorValue();
	_dlgEnviCursorValue->Create(IDD_DIALOG_ENVI_CURSORLOC);
	m_bIsValueDisplayed=false;
	
	

}


CEnviStyleManager::~CEnviStyleManager(void)
{
	
}

//void CEnviStyleManager::ShowValue(bool flag)
//{
//	this->m_bIsValueDisplayed=flag;
//	_dlgEnviCursorValue->ShowWindow(flag);
//}

void CEnviStyleManager::ShowCursorValue(bool flag)
{
	this->m_bIsValueDisplayed=flag;
	_dlgEnviCursorValue->ShowWindow(flag);
}
void CEnviStyleManager::ShowCursorValue(int nId,ENUM_WINDOW_TYPE type,int x,int y,CRect rect)
{	
	if(!this->m_bIsValueDisplayed) return;
	CEnviWindowSuite *window=EnviWindowFinder(nId);		
	CString lpValue;
	lpValue.Format("����:#%d",nId);
	_dlgEnviCursorValue->SetDlgItemTextA(IDC_STATIC_DISP,lpValue);
	lpValue.Format("ƫ��:{x:%d,y:%d}",x,y);
	_dlgEnviCursorValue->SetDlgItemTextA(IDC_STATIC_OFFSET,lpValue);
	if(type==ENUM_WINDOW_TYPE::IMG||type==ENUM_WINDOW_TYPE::ZOM)
		lpValue.Format("����:{x:%d,y:%d}",rect.left+x,rect.top+y);	
	else if(type==ENUM_WINDOW_TYPE::SCR)
	{
		window->ENVI_RECT.Update();
		double scale=(double)window->ENVI_RECT.scr.client.Width()/window->VISUAL_VIEW->m_oBand_R->m_oImage->m_oHeader.Sample;
		lpValue.Format("����:{x:%d,y:%d}",int(x/scale),int (y/scale));
	}
	_dlgEnviCursorValue->SetDlgItemTextA(IDC_STATIC_LOC,lpValue);
	CString lpPxValue;
	
	CEnviStyleManager::Instance()->EnviWindowFinder(nId)->CursorValue(CPoint(x,y),lpPxValue);
	lpValue.Format("ͼ��ֵ:{%s}",lpPxValue);
	_dlgEnviCursorValue->SetDlgItemTextA(IDC_STATIC_VALUE,lpValue);
	
	
	HDC hDC=::GetDC(NULL);
	CPoint pt;
	GetCursorPos(&pt);
	COLORREF clr=::GetPixel(hDC,pt.x,pt.y);
	lpValue.Format("��ʾֵ:{R:%d,G:%d,B:%d}",GetRValue(clr),GetGValue(clr),GetBValue(clr));
	_dlgEnviCursorValue->SetDlgItemTextA(IDC_STATIC_RGB,lpValue);
	
	
}
/*
*@name:	FileOpen
*@func:	��ͼ�񣬲���ͼ����ص��б�
*@para:	lpFullname	�ļ�ȫ·��
*/
void CEnviStyleManager::FileOpen(CString lpFullname)
{
	if(IsFileLoad(lpFullname)) return;
	CRMGImage *rmgImage=new CRMGImage(lpFullname);	
	_aryRMGImages.Add(rmgImage);

	HTREEITEM hTreeItem;
	HTREEITEM hTreeChild;	
	
	hTreeItem=_dlgEnviBandList->m_tcBandList.InsertItem(rmgImage->GetFilename(),TVI_ROOT,TVI_FIRST);	
	_dlgEnviBandList->m_tcBandList.SetItemData(hTreeItem,(DWORD)rmgImage);
	for(int i=0;i<rmgImage->m_oHeader.Band;i++)
	{
		CVisualBand *vBand=new CVisualBand(rmgImage,i);			
		hTreeChild=this->_dlgEnviBandList->m_tcBandList.InsertItem(vBand->m_lpBandName,hTreeItem,TVI_LAST);
		_dlgEnviBandList->m_tcBandList.SetItemData(hTreeChild,(DWORD)vBand);
	}//for i
	this->_dlgEnviBandList->m_tcBandList.Expand(hTreeItem,TVE_EXPAND);
	EnviWinodwList();	
	_dlgEnviBandList->ShowWindow(SW_SHOW);
}

void CEnviStyleManager::FileClose(CString lpFullname)
{
	//�ļ�Ψһ����������ɾ��
	for(int i=0;i<_aryRMGImages.GetCount();i++)	
	{
		if(_aryRMGImages[i]->m_lpFullname==lpFullname)
		{
			//��Ҫ������������ ��Ȼ�ڴ�����
			delete _aryRMGImages[i];
			_aryRMGImages.RemoveAt(i);
			break;
		}
	}//for
	
	//�رմ��ڣ����ڿ��ܲ�Ψһ����Ҫ�������ѭ��
	int i=0;
	do
	{
		for(i=0;i<_aryEnviWindowSuites.GetCount();i++)
		{
			delete _aryEnviWindowSuites[i];
			_aryEnviWindowSuites.RemoveAt(i);
			break;
		}//for
	}while(i<_aryEnviWindowSuites.GetCount());
	
	EnviWinodwList();
}


/*
*@name: IsFileLoad
*@func:	��ѯ�ļ��Ƿ��Ѿ�����
*@para:	lpFullname	�ļ�����(ȫ·��)
*/
bool CEnviStyleManager::IsFileLoad(CString lpFullname)
{
	bool isLoad=false;

	for(int i=0;i<_aryRMGImages.GetCount();i++)	
		if(_aryRMGImages[i]->m_lpFullname==lpFullname) isLoad=true;	
	
	return isLoad;
} 


/*
*@name:	EnviWindowFinder
*@func:	���ڲ�����
*@para:	id	 ����id
*/
CEnviWindowSuite* CEnviStyleManager::EnviWindowFinder(int id)
{
	for(int i=0;i<_aryEnviWindowSuites.GetCount();i++)
	{
		if(_aryEnviWindowSuites[i]->m_nId==id) return _aryEnviWindowSuites[i];
	}	
	return NULL;
} 

/*
*@name:	EnviWindowLoader
*@func:	���ڼ�����
*@para:	nid		����id��������صĴ��ڣ�
*@para:	view	���ص�VIEW
*/
void CEnviStyleManager::EnviWindowLoader(int nid,CVisualView* view)
{
	CEnviWindowSuite* window=NULL;
	if(nid<=0)  window=EnviWindowCreate(view);
	else		window=EnviWindowFinder(nid);
	window->ResetView(view);
	window->EnviWindowShow(true);
}

int CEnviStyleManager::EnviWindowCount()
{
	return _aryEnviWindowSuites.GetCount();
}

void CEnviStyleManager::EnableWindow(bool flag)
{
	_dlgEnviBandList->EnableWindow(flag);
	for(int i=0;i<_aryEnviWindowSuites.GetCount();i++)
		_aryEnviWindowSuites[i]->Enable(flag);

}
void CEnviStyleManager::Lock()
{
	EnableWindow(false);
}
void CEnviStyleManager::UnLock()
{
	EnableWindow(true);
}

/*
*@name:	EnviWindowCreate
*@func:	�����´���
*@para: view	���ص�view
*/
CEnviWindowSuite* CEnviStyleManager::EnviWindowCreate(CVisualView* view)
{
	int id=1;
	bool flag=false;
	//����δʹ�õĴ���id
	do
	{
		flag=false;
		for(int i=0;i<_aryEnviWindowSuites.GetCount();i++)
		{
			if(_aryEnviWindowSuites[i]->m_nId==id)
			{
				id++;
				flag=true;
				break;
			}
		}//for
	}while(flag);

	CEnviWindowSuite *window=new CEnviWindowSuite(id);
	_aryEnviWindowSuites.Add(window);	
	EnviWinodwList();
	return window;
}

void CEnviStyleManager::EnviWindowDestroy(int nid)
{
	
	for(int i=0;i<_aryEnviWindowSuites.GetCount();i++)
	{
		if(_aryEnviWindowSuites[i]->m_nId==nid)
		{
			delete _aryEnviWindowSuites[i];
			_aryEnviWindowSuites.RemoveAt(i);
			break;
		}
	}//for
	EnviWinodwList();
}




/*
*@name:	EnviWinodwRefresh
*@func:	ˢ�´��������˵�
*/
void CEnviStyleManager::EnviWinodwList()
{
	this->_dlgEnviBandList->m_cbWindowList.ResetContent();
	int index=this->_dlgEnviBandList->m_cbWindowList.AddString("�´���");
	this->_dlgEnviBandList->m_cbWindowList.SetItemData(index,(DWORD)0);
	CString lpText;
	for(int i=0;i<_aryEnviWindowSuites.GetSize();i++)
	{
		lpText.Format("���� #%d",_aryEnviWindowSuites.GetAt(i)->m_nId);
		index=_dlgEnviBandList->m_cbWindowList.InsertString(_dlgEnviBandList->m_cbWindowList.GetCount(), lpText);
		this->_dlgEnviBandList->m_cbWindowList.SetItemData(index,(DWORD)_aryEnviWindowSuites.GetAt(i)->m_nId);
	}
	_dlgEnviBandList->m_cbWindowList.SetCurSel(_aryEnviWindowSuites.GetSize());
}