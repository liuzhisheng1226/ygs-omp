#pragma once
#include "../../Share/Core.h"
#include "../../Share/Common.h"

class CMusk : public CIProcessable
{
public:
    CMusk(void);
    ~CMusk(void);

    void Process();
    void Batch();

    void MuskCoherenceTarget(string fileIn,string fileOut,float lowValue, float highValue);
};

