#pragma once
#include "../../Share/Core.h"
#include "../../Share/Common.h"
#include <complex>
#include <string>

using namespace std;

class CInterferogramMaps : public CIProcessable
{
public:
    CInterferogramMaps(void);
    ~CInterferogramMaps(void);

        /*
    *   继承接口方法，必须实现
    */
    void Process();
    void Batch();

    void InterferoGramMapAnalysis(string inFiles,string outMaps,float Bv,float Bt);

    void BaselineCal(string strInM,string strInS,float &baselineNorm, int &baselineT);
};

