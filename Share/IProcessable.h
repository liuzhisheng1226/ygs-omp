#pragma once
#include <cstdarg>
/// @file IProcessable.h
/// @brief 图像处理接口
/// @author Zhaolong


/// <summary>
/// 通用处理接口
/// </summary>
/// <remarks>定义了普通方法和批处理方法</remarks>
/// @ingroup Core
class CIProcessable
{
public:
	CIProcessable(void);	
	~CIProcessable(void);



	//CArray<void*,void*> m_aryArgs;					///通用参数
	
	//CStringArray		m_aryStrs;					///字符参数 默认调用参数	

	
	/// <summary>
	/// 初始化参数列表
	/// </summary>
	/// <param name="argc">参数个数</param>
	/// <param name="argv">参数列表</param>
	//void InitArgs(int argc,const void* argv,...);
		
	/// <summary>
	/// 初始化参数列表
	/// </summary>
	/// <param name="aryStrs">参数个数</param>
	//void InitArgs(CStringArray *aryStrs);	
	
	/// <summary>
	/// 普通处理方法
	/// </summary>
	/// <remarks>请在继承类中实现该方法，UI以及参数请在方法内传递</remarks>
	virtual void Process()=0;

	/// <summary>
	/// 批处理方法
	/// </summary>
	/// <remarks>请在继承类中实现该方法，参数请调用m_aryStrs或m_aryArgs</remarks>
	virtual void Batch()=0;

	/// <summary>
	/// 是否可用于批处理
	/// </summary>
	/// <remarks>标记当前方法是否可用于批处理，默认为true</remarks>
	//bool m_bIsBatchable;

	/// <summary>
	/// 当前方法描述
	/// </summary>
	/// <remarks>当前方法描述，包括功能，参数调用</remarks>
	//CString m_lpDescription;

	
	
	
};

