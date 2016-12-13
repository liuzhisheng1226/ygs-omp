#include "MultiLook.h"


CMultiLook::CMultiLook(void)
{
}


CMultiLook::~CMultiLook(void)
{
}

void CMultiLook::Process()
{
/*    CDlgMultiLook multiLookDlg;*/
    //if(multiLookDlg.DoModal()==IDCANCEL) 
        //return;
    /*MultiLookPro(multiLookDlg.m_strInFilename,multiLookDlg.m_strOutFilename,multiLookDlg.m_iRowLook,multiLookDlg.m_unColLook);*/
}
void CMultiLook::Batch()
{
/*    UINT rLooks = atoi(m_aryStrs.GetAt(2));*/
    //UINT cLooks = atoi(m_aryStrs.GetAt(3));
    //MultiLookPro(
        //m_aryStrs.GetAt(0),
        /*m_aryStrs.GetAt(1),rLooks,cLooks);*/
}

void CMultiLook::MultiLookPro(string lpImport,string lpExport,unsigned int rLooks, unsigned int cLooks)
{
    CRMGImage image(lpImport);  
    //if(image.m_oHeader.Band!=2)
    //{
    //  AfxMessageBox("非IQ通道数据!");
    //  return;     
    //}

    //header.DataType=header.DataTypeToggle();
    MACRO_GENERIC_INVOKE(image.m_oHeader.DataTypeToggle(),MultiLookFun,image,lpExport,rLooks,cLooks);

    //MultiLookFun<complex<short>>(image,lpExport,rLooks,cLooks);

    CRMGHeader header(image.m_oHeader);
    //header.Band=1;
    //
    header.Line = header.Line/rLooks;
    header.Sample = header.Sample/cLooks;
    lpExport.replace(lpExport.find(".rmg"), 4, ".ldr");
    header.Save(lpExport);

}
