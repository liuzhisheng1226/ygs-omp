#pragma once
#include "DlgEnviBandList.h"
#include "DlgEnviWindow.h"
#include "EnviWindowSuite.h"
#include "DlgEnviCursorValue.h"

/*
*创建类似于Envi窗口
*/
class CEnviStyleManager
{
public:
	CEnviStyleManager(void);
	~CEnviStyleManager(void);


	static CEnviStyleManager* Instance()
	{
		if(_instance==NULL) _instance=new CEnviStyleManager();
		return _instance;
	}

	void FileOpen(CString lpFullname);
	void FileClose(CString lpFullname);	

	CEnviWindowSuite* EnviWindowFinder(int index);

	void EnviWindowLoader(int nid,CVisualView* view);
	void EnviWindowDestroy(int nid);
	int EnviWindowCount();
	

	void EnviWindowLink();
	
	/// <summary>
	/// 用于锁定所有窗口
	/// </summary>
	void Lock();

	/// <summary>
	/// 解锁所有窗口
	/// </summary>
	void UnLock();

	void ShowCursorValue(int nId,ENUM_WINDOW_TYPE type,int x,int y,CRect rect);
	void ShowCursorValue(bool flag);
	

	
	CArray<CEnviWindowSuite*,CEnviWindowSuite*>* WindowManager()
	{
		return &_aryEnviWindowSuites;	
	}
private:	
	bool IsFileLoad(CString lpFullname);
	CEnviWindowSuite* EnviWindowCreate(CVisualView* view);	
	
	void EnviWinodwList();
	void EnableWindow(bool flag);
	

public:
	

private:
	static CEnviStyleManager*						_instance;						//管理器实例		

	CArray<CRMGImage*,CRMGImage*>					_aryRMGImages;					//图像列表
	CArray<CEnviWindowSuite*,CEnviWindowSuite*>		_aryEnviWindowSuites;			//窗口管理器	

	CDlgEnviBandList 								*_dlgEnviBandList;				//波段列表窗口
	CDlgEnviCursorValue								*_dlgEnviCursorValue;			//<鼠标值显示
	bool m_bIsValueDisplayed;														//<是否需要显示当前鼠标值

	
};

