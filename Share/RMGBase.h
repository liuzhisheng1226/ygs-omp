#pragma once
#include "RMGHeader.h"
#include "PreOrb.h"
#include <string>

using namespace std;

/// @file RMGBase.h
/// @brief RMGBase.h�ļ���RMGͼ�����
/// @author Zhaolong


/// <summary>
/// ���͵��ú�
/// </summary>
/// <param name="type">����</param>
/// <param name="method">���÷���</param>
/// <param name="...">��������</param>

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
/// ���������͵ķ��͵��ú�
/// <summary>
/// <param name="type">����</param>
/// <param name="method">���÷���</param>
/// <param name="...">��������</param>
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
/// ͼ���ļ�������
/// </summary>
/// <remarks>��������ͼ�����Ҫ�̳и���</remarks>
/// @ingroup Core
class CRMGBase
{
public:
    CRMGBase(void);
    ~CRMGBase(void);

public: 
    string m_lpFullname;                    ///<ͼ��·��

    CRMGHeader m_oHeader;                   ///<ͼ��ͷ�ļ�����
    
    CPreOrb    m_oPreOrbit;                 ///ͼ��ľ������


    const long   CONST_MATH_LIGHT_VELOCITY; ///<��ѧ��������=299792458
    const double CONST_MATH_PI;             ///<��ѧ������=3.141592658978;

private:
    const string CONST_HEADER_EXT;          ///<ͷ�ļ���չ��
    const string CONST_RMG_EXT;         ///<RMG�ļ���չ��
    const string CONST_PREORB_EXT;          ///���������ļ���չ��

public:
    /// <summary>
    /// ��ȡͷ�ļ��ļ���
    /// </summary>
    string GetHeadername();

    /// <summary>
    /// ��ȡRMG�ļ��ļ���
    /// </summary>
    string GetFilename();

    string GetPreOrbitname();
};

