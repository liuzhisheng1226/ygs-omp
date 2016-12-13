#pragma once
#include "../../Share/Core.h"
#include "../../Share/Common.h"
#include <complex>
#include <string>

using namespace std;

class CR2Calibration : public CIProcessable
{
public:
    CR2Calibration(void);
    ~CR2Calibration(void);

    void Process();
    void Batch();

    void R2CalibrationPro(string lpImport,string lpLutfile,string lpExport);
};

