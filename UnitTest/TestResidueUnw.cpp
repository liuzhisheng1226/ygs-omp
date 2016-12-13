#include "../Plugins/PfxCTInSAR/ResidueUnw.h"
#include "test_config.h"
#include <iostream>

void testResidueUnw() {
    cout << "test begins\n";
    CResidueUnw *ru = new CResidueUnw();
    ru->ResidueUnwPro(
            DATA_ROOT_PATH+"CTI/integratePoint.rmg",
            DATA_ROOT_PATH+"CTI/integratePoint.ldr",
            DATA_ROOT_PATH+"CTI/unwrapPoint.rmg",
            DATA_ROOT_PATH+"CTI/unwrapPoint.ldr"
            );
    cout << "test finished\n";
}

int main() {
    testResidueUnw();
    return 0;
}
