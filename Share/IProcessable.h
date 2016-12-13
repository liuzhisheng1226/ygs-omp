#pragma once
#include <cstdarg>
/// @file IProcessable.h
/// @brief ͼ����ӿ�
/// @author Zhaolong


/// <summary>
/// ͨ�ô���ӿ�
/// </summary>
/// <remarks>��������ͨ��������������</remarks>
/// @ingroup Core
class CIProcessable
{
public:
	CIProcessable(void);	
	~CIProcessable(void);



	//CArray<void*,void*> m_aryArgs;					///ͨ�ò���
	
	//CStringArray		m_aryStrs;					///�ַ����� Ĭ�ϵ��ò���	

	
	/// <summary>
	/// ��ʼ�������б�
	/// </summary>
	/// <param name="argc">��������</param>
	/// <param name="argv">�����б�</param>
	//void InitArgs(int argc,const void* argv,...);
		
	/// <summary>
	/// ��ʼ�������б�
	/// </summary>
	/// <param name="aryStrs">��������</param>
	//void InitArgs(CStringArray *aryStrs);	
	
	/// <summary>
	/// ��ͨ������
	/// </summary>
	/// <remarks>���ڼ̳�����ʵ�ָ÷�����UI�Լ��������ڷ����ڴ���</remarks>
	virtual void Process()=0;

	/// <summary>
	/// ��������
	/// </summary>
	/// <remarks>���ڼ̳�����ʵ�ָ÷��������������m_aryStrs��m_aryArgs</remarks>
	virtual void Batch()=0;

	/// <summary>
	/// �Ƿ������������
	/// </summary>
	/// <remarks>��ǵ�ǰ�����Ƿ������������Ĭ��Ϊtrue</remarks>
	//bool m_bIsBatchable;

	/// <summary>
	/// ��ǰ��������
	/// </summary>
	/// <remarks>��ǰ�����������������ܣ���������</remarks>
	//CString m_lpDescription;

	
	
	
};

