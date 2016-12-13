#include "../Plugins/PfxCTInSAR/Residue.h"
#include "test_config.h"
#include <iostream>

void testResidue() {
    cout << "test begins\n";
    CResidue *r = new CResidue();
    r->ResiduePro(
            DATA_ROOT_PATH+"CTI/PSC.rmg",
            DATA_ROOT_PATH+"CTI/PSC.ldr",
            DATA_ROOT_PATH+"CTI/integratePoint.rmg",
            DATA_ROOT_PATH+"CTI/integratePoint.ldr"
            );
    cout << "test finished\n";
}

int main() {
    testResidue();
    return 0;
}
