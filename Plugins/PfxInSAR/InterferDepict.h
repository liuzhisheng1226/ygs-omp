#pragma once
#include "../../Share/Core.h"
#include <complex>

/*
* �������
*/
class CInterferDepict : public CIProcessable
{
public:
    CInterferDepict(void);
    ~CInterferDepict(void);

    void Process();
    void Batch();


    /*
    *   ����ͼ����
    */
    void Depict();
};

