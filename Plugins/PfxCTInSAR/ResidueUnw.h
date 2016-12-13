#pragma once
#include "../../Share/Core.h"
#include "../../Share/Common.h"
#include <string>

using namespace std;

class CResidueUnw : public CIProcessable
{
public:
    CResidueUnw(void);
    ~CResidueUnw(void);


    void Process();
    void Batch();

    void ResidueUnwPro(string filein,string fileinH,string fileout,string fileoutH);
    double Round(double r);   //就近去整
    void WrapPhase(float & phase);
};

