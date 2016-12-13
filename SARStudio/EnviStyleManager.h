#pragma once
#include "DlgEnviBandList.h"
#include "DlgEnviWindow.h"
#include "EnviWindowSuite.h"
#include "DlgEnviCursorValue.h"

/*
*����������Envi����
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
	/// �����������д���
	/// </summary>
	void Lock();

	/// <summary>
	/// �������д���
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
	static CEnviStyleManager*						_instance;						//������ʵ��		

	CArray<CRMGImage*,CRMGImage*>					_aryRMGImages;					//ͼ���б�
	CArray<CEnviWindowSuite*,CEnviWindowSuite*>		_aryEnviWindowSuites;			//���ڹ�����	

	CDlgEnviBandList 								*_dlgEnviBandList;				//�����б���
	CDlgEnviCursorValue								*_dlgEnviCursorValue;			//<���ֵ��ʾ
	bool m_bIsValueDisplayed;														//<�Ƿ���Ҫ��ʾ��ǰ���ֵ

	
};

