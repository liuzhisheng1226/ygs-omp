#pragma once
#include "../../Share/Core/Core.h"
#include "../../Share/Common/Common.h"


class CS2T3 :
	public CIProcessable
{
public:
	CS2T3(void);
	~CS2T3(void);

	/*
	*	继承接口方法，必须实现
	*/
	void Process();
	void Batch();

	void S2toT3(CString strInHH,CString strInHV,CString strInVH,CString strInVV
		       ,CString strInT11,CString strInT12,CString strInT13
			   ,CString strInT22,CString strInT23,CString strInT33);
};