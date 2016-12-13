#include "RoiSelectPro.h"


CRoiSelectPro::CRoiSelectPro(void)
{
}


CRoiSelectPro::~CRoiSelectPro(void)
{
}

void CRoiSelectPro::Process()
{
/*    CDlgRoiSelect dlg;*/
    //if(dlg.DoModal()!=IDOK)
        //return;
    /*getRoi(dlg.m_strFilesName,dlg.m_iStartR,dlg.m_iStartC,dlg.m_iRoiRs,dlg.m_iRoiCs,dlg.m_strOutFilesName);*/
}
void CRoiSelectPro::Batch()
{
/*    //getRoi(m_aryStrs.GetAt(0))*/
    //int iStartR = atoi(m_aryStrs.GetAt(1));
    //int iStartC = atoi(m_aryStrs.GetAt(2));
    //int iRoiRs = atoi(m_aryStrs.GetAt(3));
    //int iRoiCs = atoi(m_aryStrs.GetAt(4));
    //getRoi(m_aryStrs.GetAt(0),iStartR,iStartC,iRoiRs,iRoiCs,m_aryStrs.GetAt(5));
    /*return;*/
}

void CRoiSelectPro::getRoi(string lpInFilesName,unsigned int uiStartR, unsigned int uiStartC,unsigned int roiRows, unsigned int roiCols,
    string lpOutFilesName)
{
    CRMGImage img(lpInFilesName);
    //MACRO_GENERIC_INVOKE(img.m_oHeader.DataType,getRoiFun,img,lpOutFilesName,uiStartR,uiStartC,roiRows,roiCols);
    MACRO_GENERIC_SIMPLE_INVOKE(img.m_oHeader.DataType,getRoiFun,img,lpOutFilesName,uiStartR,uiStartC,roiRows,roiCols);


    string ldrName = lpOutFilesName;
    int dot = ldrName.find_last_of('.');
    if(dot==-1)
        ldrName = ldrName+".ldr";
    else
        ldrName = ldrName.substr(0, dot)+".ldr";

    
    CRMGHeader header(img.m_oHeader);                       //复制主图像头文件信息
    header.leftUpC = uiStartC;
    header.leftUpL = uiStartR;
    header.Sample=roiCols;
    header.Line=roiRows;
    header.Save(ldrName);       //lpHdrExport);
}
