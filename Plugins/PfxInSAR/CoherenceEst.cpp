#include "CoherenceEst.h"


CoherenceEst::CoherenceEst(void)
{
}


CoherenceEst::~CoherenceEst(void)
{
}

void CoherenceEst::Process()
{
/*    CDlgCoherenceEst dlg;*/
    //if(dlg.DoModal()!=IDOK)
        //return;

    //CoherenceEstimate(dlg.m_strInMaster,dlg.m_strInSlave,dlg.m_strInFlat,
                        /*dlg.m_strOutCoh,dlg.m_iRLooks,dlg.m_iSLooks);*/
}

void CoherenceEst::Batch()
{
  /*  int iAlooks = atoi(m_aryStrs.GetAt(4));*/
    //int iRlooks = atoi(m_aryStrs.GetAt(5)); 

    //CoherenceEstimate(m_aryStrs.GetAt(0),m_aryStrs.GetAt(1),m_aryStrs.GetAt(2),m_aryStrs.GetAt(3),
                    /*iAlooks,iRlooks);*/
}

void CoherenceEst::CoherenceEstimate(string strInM,string strInS,string strInFlat,string strOutCoh,int rLooks,int sLooks)
{
    CRMGImage mRmg(strInM);
    CRMGImage sRmg(strInS);

    int width = mRmg.m_oHeader.Sample;
    int line = mRmg.m_oHeader.Line;

    //MACRO_GENERIC_INVOKE(mRmg.m_oHeader.DataTypeToggle(),CoherenceFun,strInM,strInS,strInFlat,strOutCoh,rLooks,sLooks,width,line);
    CoherenceFun(strInM,strInS,strInFlat,strOutCoh,rLooks,sLooks,width,line);

    CRMGHeader header(mRmg.m_oHeader);
    //header.Band=1;
    //
    header.Line = header.Line/rLooks;
    header.Sample = header.Sample/sLooks;
    /*<string> imagration*/
    int len = strOutCoh.length();
    int dotpos = len - 4;
    string postfix(".ldr");
    strOutCoh = strOutCoh.replace(dotpos, 4, postfix);
    header.Save(strOutCoh);
}
