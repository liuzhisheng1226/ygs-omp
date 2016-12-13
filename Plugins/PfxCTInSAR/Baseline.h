#pragma once
#include "../../Share/Core.h"
#include "../../Share/Common.h"

class CBaseline : public CIProcessable
{
public:
    CBaseline(void);
    ~CBaseline(void);

     void Process();
     void Batch();

    void BaselineInangleSlate(string MFileIn,string SFileIn,bool CheckPreOrt,string MFineOrt,string SFineOrt,int AzuLooks,int RanLooks,
        string BaselineOut,string InAngleOut,string SlateOut);

    void AnalyseGeometry(string mfile,string sfile,
                            double &baseline_norm,bool& Incflag);

    void pwcint(double *x, double *y, long m, double *xresult, double *yresult, long n); 
    void pwcslopes(double*x,double*y,long m,double*del,double*deriv);   //ÇóÒ»½×µ¼
};

