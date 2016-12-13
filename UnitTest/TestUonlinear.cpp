#include "../Plugins/PfxCTInSAR/Uonlinear.h"
#include "test_config.h"
#include <iostream>

void testUonlinear() {
    cout << "test begins\n";
    CUonlinear *ul = new CUonlinear();
    ul->UonlinearPro(
            DATA_ROOT_PATH+"CTI/PSC.rmg",
            DATA_ROOT_PATH+"CTI/PSC.ldr",
            DATA_ROOT_PATH+"CTI/integratePoint.rmg",
            DATA_ROOT_PATH+"CTI/integratePoint.ldr",
            DATA_ROOT_PATH+"CTI/SARtime.txt",
            DATA_ROOT_PATH+"CTI/unwrapPoint.rmg",
            DATA_ROOT_PATH+"CTI/unwrapPoint.ldr",
            DATA_ROOT_PATH+"CTI/Group.txt",
            DATA_ROOT_PATH+"CTI/actmosOut.txt",
            DATA_ROOT_PATH+"CTI/velFileOut.txt",
            DATA_ROOT_PATH+"CTI/deformationOut"
            );
    cout << "test finished\n";
}

int main() {
    testUonlinear();
    return 0;
}
