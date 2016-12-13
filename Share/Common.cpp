// Common.cpp : 定义 DLL 的初始化例程。
//

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


/************************************************
*****************通用宏定义**********************
************************************************/

#define MACRO_GENERIC_CALL(generi,entry,para)\
	switch (generi)\
	{\
	case ENUM_DATA_TYPE::eBYTE:entry<BYTE>(para);break;\
	case ENUM_DATA_TYPE::eUINT16:entry<USHORT>(para);break;\
	case ENUM_DATA_TYPE::eINT16:entry<INT16>(para);break;\
	case ENUM_DATA_TYPE::eUINT32:entry<UINT32>(para);break;\
	case ENUM_DATA_TYPE::eINT32:entry<INT32>(para);break;\
	case ENUM_DATA_TYPE::eFLOAT32:entry<FLOAT32>(para);break;\
	case ENUM_DATA_TYPE::eFLOAT64:entry<FLOAT64>(para);break;\
	case ENUM_DATA_TYPE::eCINT16:entry<complex<INT16>(para);break;\
	case ENUM_DATA_TYPE::eCINT32:entry<complex<INT32> >(para);break;\
	case ENUM_DATA_TYPE::eCFLOAT32:entry<complex<FLOAT32> >(para);break;\
	case ENUM_DATA_TYPE::eCFLOAT64:entry<complex<FLOAT64>(para);break;\
	}\



