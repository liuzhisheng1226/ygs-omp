#pragma once
#include "../../Share/Core.h"


class CFlatPhase : public CIProcessable
{
public:
    CFlatPhase(void);
    ~CFlatPhase(void);

    void Process();
    void Batch();


    void m_Flat_R2(string mFile,string mFileH,string sFile,string sFileH,
             string mOutFile,string mOutFileH,
             bool isOrb);

    void pwcint(double *x, double *y, long m, double *xresult, double *yresult, long n); 
    void pwcslopes(double*x,double*y,long m,double*del,double*deriv);   //ÇóÒ»½×µ¼
    
};

