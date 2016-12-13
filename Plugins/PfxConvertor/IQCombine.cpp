#include "IQCombine.h"

CIQCombine::CIQCombine(void)
{
    
}

CIQCombine::~CIQCombine(void)
{
}

void CIQCombine::Process()
{
    /*CDlgIqCombine iqDlg;*/
    //if(iqDlg.DoModal()==IDCANCEL) return;
    /*IqBandsCombine(iqDlg.m_lpImport,iqDlg.m_lpExport);*/

}
void CIQCombine::Batch()
{
/*    IqBandsCombine(*/
        //m_aryStrs.GetAt(0),
        /*m_aryStrs.GetAt(1));*/
}


void CIQCombine::IqBandsCombine(string lpImport,string lpExport)
{
    CRMGImage image(lpImport);  
    if(image.m_oHeader.Band!=2)
    {
        cout << "not IQ tunnel data!\n";
        return;     
    }

    MACRO_GENERIC_INVOKE(image.m_oHeader.DataType,Bands2Complex,image,lpExport);

    CRMGHeader header(image.m_oHeader);
    header.Band=1;
    header.DataType=header.DataTypeToggle();
    lpExport.replace(lpExport.find(".rmg"), 4, ".ldr");
    header.Save(lpExport);
}
