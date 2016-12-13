#pragma once
#include "../../Share/Core/Core.h"
#include "../../Share/Common/Common.h"


class CpolLeeFilter :
	public CIProcessable
{
public:
	CpolLeeFilter(void);
	~CpolLeeFilter(void);

	/*
	*	继承接口方法，必须实现
	*/
	void Process();
	void Batch();

	void RefineLee(CString strInT11,CString strInT12,CString strInT13
			      ,CString strInT22,CString strInT23,CString strInT33
				  ,CString strOutT11,CString strOutT12,CString strOutT13
			      ,CString strOutT22,CString strOutT23,CString strOutT33
				  ,int iWin, float fLook);

	void make_Mask(int ***Mask, int Nwin);
};