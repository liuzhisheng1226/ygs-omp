#include "StdAfx.h"
#include "EnviStyleManager.h"
#include "VisualBand.h"



CEnviStyleManager* CEnviStyleManager::_instance=NULL;

CEnviStyleManager::CEnviStyleManager(void)
{
	//TODO 完成初始化操作

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
	lpValue.Format("窗口:#%d",nId);
	_dlgEnviCursorValue->SetDlgItemTextA(IDC_STATIC_DISP,lpValue);
	lpValue.Format("偏移:{x:%d,y:%d}",x,y);
	_dlgEnviCursorValue->SetDlgItemTextA(IDC_STATIC_OFFSET,lpValue);
	if(type==ENUM_WINDOW_TYPE::IMG||type==ENUM_WINDOW_TYPE::ZOM)
		lpValue.Format("坐标:{x:%d,y:%d}",rect.left+x,rect.top+y);	
	else if(type==ENUM_WINDOW_TYPE::SCR)
	{
		window->ENVI_RECT.Update();
		double scale=(double)window->ENVI_RECT.scr.client.Width()/window->VISUAL_VIEW->m_oBand_R->m_oImage->m_oHeader.Sample;
		lpValue.Format("坐标:{x:%d,y:%d}",int(x/scale),int (y/scale));
	}
	_dlgEnviCursorValue->SetDlgItemTextA(IDC_STATIC_LOC,lpValue);
	CString lpPxValue;
	
	CEnviStyleManager::Instance()->EnviWindowFinder(nId)->CursorValue(CPoint(x,y),lpPxValue);
	lpValue.Format("图像值:{%s}",lpPxValue);
	_dlgEnviCursorValue->SetDlgItemTextA(IDC_STATIC_VALUE,lpValue);
	
	
	HDC hDC=::GetDC(NULL);
	CPoint pt;
	GetCursorPos(&pt);
	COLORREF clr=::GetPixel(hDC,pt.x,pt.y);
	lpValue.Format("显示值:{R:%d,G:%d,B:%d}",GetRValue(clr),GetGValue(clr),GetBValue(clr));
	_dlgEnviCursorValue->SetDlgItemTextA(IDC_STATIC_RGB,lpValue);
	
	
}
/*
*@name:	FileOpen
*@func:	打开图像，并将图像加载到列表
*@para:	lpFullname	文件全路径
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
	//文件唯一，可以这样删除
	for(int i=0;i<_aryRMGImages.GetCount();i++)	
	{
		if(_aryRMGImages[i]->m_lpFullname==lpFullname)
		{
			//需要增加析构操作 不然内存会溢出
			delete _aryRMGImages[i];
			_aryRMGImages.RemoveAt(i);
			break;
		}
	}//for
	
	//关闭窗口，窗口可能不唯一，需要增加外层循环
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
*@func:	查询文件是否已经加载
*@para:	lpFullname	文件名称(全路径)
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
*@func:	窗口查找器
*@para:	id	 窗口id
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
*@func:	窗口加载器
*@para:	nid		窗口id（请求加载的窗口）
*@para:	view	加载的VIEW
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
*@func:	创建新窗口
*@para: view	加载的view
*/
CEnviWindowSuite* CEnviStyleManager::EnviWindowCreate(CVisualView* view)
{
	int id=1;
	bool flag=false;
	//查找未使用的窗口id
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
*@func:	刷新窗口下拉菜单
*/
void CEnviStyleManager::EnviWinodwList()
{
	this->_dlgEnviBandList->m_cbWindowList.ResetContent();
	int index=this->_dlgEnviBandList->m_cbWindowList.AddString("新窗口");
	this->_dlgEnviBandList->m_cbWindowList.SetItemData(index,(DWORD)0);
	CString lpText;
	for(int i=0;i<_aryEnviWindowSuites.GetSize();i++)
	{
		lpText.Format("窗口 #%d",_aryEnviWindowSuites.GetAt(i)->m_nId);
		index=_dlgEnviBandList->m_cbWindowList.InsertString(_dlgEnviBandList->m_cbWindowList.GetCount(), lpText);
		this->_dlgEnviBandList->m_cbWindowList.SetItemData(index,(DWORD)_aryEnviWindowSuites.GetAt(i)->m_nId);
	}
	_dlgEnviBandList->m_cbWindowList.SetCurSel(_aryEnviWindowSuites.GetSize());
}