#pragma once
#include "../../Share/Core.h"
#include "../../Share/Common.h"
#include <complex>
#include <string>

using namespace std;

class CBaseLineEst : public CIProcessable
{
public:
    CBaseLineEst(void);
    ~CBaseLineEst(void);

    /*
    *   �̳нӿڷ���������ʵ��
    */
    void Process();
    void Batch();

    void BaselineEstimate(string strInM,string strInS);
};

