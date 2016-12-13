#pragma once
#include "../../Share/Core.h"
#include <complex>

/*
* 干涉操作
*/
class CInterferDepict : public CIProcessable
{
public:
    CInterferDepict(void);
    ~CInterferDepict(void);

    void Process();
    void Batch();


    /*
    *   干涉图生成
    */
    void Depict();
};

