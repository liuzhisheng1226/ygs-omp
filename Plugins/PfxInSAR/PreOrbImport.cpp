#include "PreOrbImport.h"
#include <cstdio>

CPreOrbImport::CPreOrbImport(void)
{
}


CPreOrbImport::~CPreOrbImport(void)
{
}

void CPreOrbImport::Process()
{
/*    CDlgPreorbImport dlg;*/
    //if(dlg.DoModal()!=IDOK)
        //return;
    //preOrb2Local(dlg.m_strInRmgFile,dlg.m_strInLdrFile,
        //dlg.m_strInPreOrb,dlg.m_strOutPreOrb,
        /*dlg.m_iType,dlg.m_fTgap);*/
}

void CPreOrbImport::Batch()
{
  /*  int iType =atoi(m_aryStrs.GetAt(4));*/
    //float fInterval = atof(m_aryStrs.GetAt(5));

    ////strInRmgFile,strInLdrFile,strInPreOrb,strOutPreOrb,iType,m_fTgap
    //preOrb2Local(m_aryStrs.GetAt(0),m_aryStrs.GetAt(1),
        //m_aryStrs.GetAt(2),m_aryStrs.GetAt(3),
        /*iType,fInterval);*/
}

void CPreOrbImport::preOrb2Local(string lpInRmgFile,string lpInLdrFile,string lpInOrbFile,string lpOutOrbFile,int iType,float fTgap)
{
    CRMGImage img(lpInRmgFile,lpInLdrFile);

    img.m_oPreOrbit.LoadOrifile(lpInOrbFile,fTgap);

    //double startT = img.preOrbitPoint[0].timePoint;
    FILE *fp;
    if((fp=fopen(lpOutOrbFile.c_str(),"w"))==NULL)
        return;
    double t,x,y,z;
    // ¶àÏîÊ½ÄâºÏ
    if(iType==0)
    {
        //Í¼Ïñ¹ìµÀÄâºÏÏµÊý  
        CSpMatrix<double> xPolyCoef(4,1);                       //----**aef
        CSpMatrix<double> yPolyCoef(4,1);                       //----**bef
        CSpMatrix<double> zPolyCoef(4,1);                       //----**cef
        img.OrbitCoef(img.m_oPreOrbit.preOrbitPoint,xPolyCoef,yPolyCoef,zPolyCoef);

        fprintf(fp,"[起始时间]\t\t%.5f",img.m_oPreOrbit.preOrbitPoint[0].timePoint.precision);
        fprintf(fp,"\n[时间间隔]\t\t%.2f",fTgap);
        fprintf(fp,"\n[点数]\t\t%d",img.m_oPreOrbit.m_nCount);
        fprintf(fp,"\n[卫星位置]");
        
        for(int i=0;i<img.m_oPreOrbit.m_nCount;i++)
        {
            t= i*img.m_oPreOrbit.m_fIntervalT;
            x = xPolyCoef.Get(0,0)+xPolyCoef.Get(1,0)*t+xPolyCoef.Get(2,0)*t*t+xPolyCoef.Get(3,0)*t*t*t;
            y = yPolyCoef.Get(0,0)+yPolyCoef.Get(1,0)*t+yPolyCoef.Get(2,0)*t*t+yPolyCoef.Get(3,0)*t*t*t;
            z = zPolyCoef.Get(0,0)+zPolyCoef.Get(1,0)*t+zPolyCoef.Get(2,0)*t*t+zPolyCoef.Get(3,0)*t*t*t;

            fprintf(fp,"\n\t\t%.6f\t%.6f\t%.6f",x,y,z);         
        }       
    }
    else if(iType==1)
    {   //ÑùÌõÄâºÏ

        ;
    }
    fclose(fp);
}
