#pragma once
#include "../../Share/Core.h"
#include "../../Share/Common.h"
#include <complex>

class CPreOrbImport :public CIProcessable
{
public:
    CPreOrbImport(void);
    ~CPreOrbImport(void);

    /*
    *   �̳нӿڷ���������ʵ��
    */
    void Process();
    void Batch();

    void preOrb2Local(string lpInRmgFile,string lpInLdrFile,string lpInOrbFile,string lpOutOrbFile,int iType,float iTgap);


};

