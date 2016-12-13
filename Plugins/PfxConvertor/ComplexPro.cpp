#include "ComplexPro.h"


CComplexPro::CComplexPro(void)
{
}


CComplexPro::~CComplexPro(void)
{
}

void CComplexPro::Process()
{
    /*CDlgComplexPro complexDlg;*/
    //if(complexDlg.DoModal()==IDCANCEL) return;
    /*ComplexConv(complexDlg.m_strInpath,complexDlg.m_strOutPath,complexDlg.m_iType); */
}

void CComplexPro::Batch()
{
    /*int iType  = atoi(m_aryStrs.GetAt(2));*/
    //ComplexConv(
        //m_aryStrs.GetAt(0),
        /*m_aryStrs.GetAt(1),iType);      //iType=0 --->abs; iType =1---->phase*/
}

void CComplexPro::ComplexConv(string strInpath,string strOutPath,int iType)
{
    CRMGImage inRmg(strInpath);
    
    MACRO_GENERIC_INVOKE(inRmg.m_oHeader.DataTypeToggle(),ComplexFun,inRmg,strOutPath,iType);

    CRMGHeader header(inRmg.m_oHeader);
    header.Band=1;
    header.DataType = eFLOAT32;
    strOutPath.replace(strOutPath.find(".rmg"), 4, ".ldr");
    header.Save(strOutPath);
}
