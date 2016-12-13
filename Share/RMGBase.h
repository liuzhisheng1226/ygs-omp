#pragma once
#include "RMGHeader.h"
#include "PreOrb.h"
#include <string>

using namespace std;

/// @file RMGBase.h
/// @brief RMGBase.h文件，RMG图像基类
/// @author Zhaolong


/// <summary>
/// 泛型调用宏
/// </summary>
/// <param name="type">类型</param>
/// <param name="method">调用方法</param>
/// <param name="...">方法参数</param>

// #define MACRO_GENERIC_INVOKE(type,method,args,...)\
// do{\
//     switch(type)\
//     {\
//     case eBYTE      :method<BYTE>(##args,__VA_ARGS__);      break;\
//     case eUINT16    :method<USHORT>(##args,__VA_ARGS__);    break;\
//     case eINT16     :method<INT16>(##args,__VA_ARGS__); break;\
//     case eUINT32    :method<UINT32>(##args,__VA_ARGS__);    break;\
//     case eINT32     :method<INT32>(##args,__VA_ARGS__); break;\
//     case eFLOAT32   :method<FLOAT>(##args,__VA_ARGS__); break;\
//     case eFLOAT64   :method<DOUBLE>(##args,__VA_ARGS__);    break;\
//     case eCINT16    :method<INT16>(##args,__VA_ARGS__);     break;\
//     case eCINT32    :method<INT32>(##args,__VA_ARGS__);     break;\
//     case eCFLOAT32  :method<FLOAT>(##args,__VA_ARGS__);     break;\
//     }\
// }while(0);

// <liutao> <2016-10-14 Fri 16:45>
#define MACRO_GENERIC_INVOKE(type,method,args,...)\
do{\
    switch(type)\
    {\
    case eBYTE      :method<BYTE>(args,__VA_ARGS__);      break;\
    case eUINT16    :method<USHORT>(args,__VA_ARGS__);    break;\
    case eINT16     :method<INT16>(args,__VA_ARGS__); break;\
    case eUINT32    :method<UINT32>(args,__VA_ARGS__);    break;\
    case eINT32     :method<INT32>(args,__VA_ARGS__); break;\
    case eFLOAT32   :method<FLOAT>(args,__VA_ARGS__); break;\
    case eFLOAT64   :method<DOUBLE>(args,__VA_ARGS__);    break;\
    case eCINT16    :method<INT16>(args,__VA_ARGS__);     break;\
    case eCINT32    :method<INT32>(args,__VA_ARGS__);     break;\
    case eCFLOAT32  :method<FLOAT>(args,__VA_ARGS__);     break;\
    }\
}while(0);
// </liutao>

/// <summary>
/// 简单数据类型的泛型调用宏
/// <summary>
/// <param name="type">类型</param>
/// <param name="method">调用方法</param>
/// <param name="...">方法参数</param>
#define MACRO_GENERIC_SIMPLE_INVOKE(type,method,...)\
do{\
    switch(type)\
    {\
    case eBYTE      :method<BYTE>(__VA_ARGS__);     break;\
    case eUINT16    :method<USHORT>(__VA_ARGS__);   break;\
    case eINT16     :method<INT16>(__VA_ARGS__);    break;\
    case eUINT32    :method<UINT32>(__VA_ARGS__);   break;\
    case eINT32     :method<INT32>(__VA_ARGS__);    break;\
    case eFLOAT32   :method<FLOAT>(__VA_ARGS__);    break;\
    case eFLOAT64   :method<DOUBLE>(__VA_ARGS__);   break;\
    case eCINT16    :method<complex<INT16> >(__VA_ARGS__);   break;\
    case eCFLOAT32  :method<complex<FLOAT> >(__VA_ARGS__);   break;\
    }\
}while(0);





/// <summary>
/// 图像文件基本类
/// </summary>
/// <remarks>所有类型图像均需要继承该类</remarks>
/// @ingroup Core
class CRMGBase
{
public:
    CRMGBase(void);
    ~CRMGBase(void);

public: 
    string m_lpFullname;                    ///<图像路径

    CRMGHeader m_oHeader;                   ///<图像头文件对象
    
    CPreOrb    m_oPreOrbit;                 ///图像的精轨对象


    const long   CONST_MATH_LIGHT_VELOCITY; ///<数学常量光速=299792458
    const double CONST_MATH_PI;             ///<数学常量π=3.141592658978;

private:
    const string CONST_HEADER_EXT;          ///<头文件扩展名
    const string CONST_RMG_EXT;         ///<RMG文件扩展名
    const string CONST_PREORB_EXT;          ///精轨数据文件扩展名

public:
    /// <summary>
    /// 获取头文件文件名
    /// </summary>
    string GetHeadername();

    /// <summary>
    /// 获取RMG文件文件名
    /// </summary>
    string GetFilename();

    string GetPreOrbitname();
};

