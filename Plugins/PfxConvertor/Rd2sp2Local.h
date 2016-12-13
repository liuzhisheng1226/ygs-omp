#pragma once
#include "../../share/Core.h"
#include "../../share/Common.h"
#include "DlgRd2sp2Rmg.h"


class CRd2sp2Local : public CIProcessable
{
public:
    CRd2sp2Local(void);
    ~CRd2sp2Local(void);

    void Process();
    void Batch();

    /*
    *@name :    Rd2spHdr2Ldr
    *@func :    ��Rd2��ͷ�ļ�ת��Ϊldr�ļ�
    *@para :    lpHeaderImport      ����ͷ�ļ�����
    */
    CRMGHeader Rd2spHdr2Ldr(CString lpHeaderImport);
    /*
    *@name :    Rd2spData2Rmg
    *@func :    ��Rd2�������ļ�ת��Ϊrmg�ļ�
    */
    void Rd2spData2Rmg(CString lpDataImport,CString lpHdrImport,CString lpDataExport,CString lpHdrExport);  

    template<class T>
    void GenericCPLMalloc(int xsize,int &typeSize,void **mem)
    {
        typeSize=sizeof(T);
        *mem=(T*)CPLMalloc(xsize*typeSize);
        return;
    };
};

