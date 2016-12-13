#pragma once
#include "../../Share/Core.h"
#include "../../Share/Common.h"
#include <string>

using namespace std;

class CResidue : public CIProcessable
{
public:
    CResidue(void);
    ~CResidue(void);


    void Process();
    void Batch();

    void ResiduePro(string pscFileIn,string pscFileInH,string integrationFileIn,string integrationFileInH);
};

