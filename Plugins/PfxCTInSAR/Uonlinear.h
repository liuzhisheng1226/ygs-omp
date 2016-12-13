#pragma once
#include "../../Share/Core.h"
#include "../../Share/Common.h"
#include "Delaunay.h"
#include <string>

class CUonlinear : public CIProcessable
{
public:
    CUonlinear(void);
    ~CUonlinear(void);

    void Process();
    void Batch();

    void UonlinearPro(string pscFileIn,string pscFileInH,string inteFileIn,string inteFileInH,string imageTime,
        string unwResidueFileIn,string unwResidueFileInH,string interGroup,string actmosOut,string velFileOut,string deformationOut);

    float distance(PointF p1,PointF p2);
};

