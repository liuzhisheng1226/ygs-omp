#pragma once
#include "../../Share/Core.h"

class CMeanCoh : public CIProcessable
{
public:
    CMeanCoh(void);
    ~CMeanCoh(void);

    void Process();
    void Batch();

    void MeanCoherence(string fileIn,string fileOut,string fileOutH,int widht,int height);
};

