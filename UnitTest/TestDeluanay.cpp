#include "../Plugins/PfxCTInSAR/DenuanayNetwork.h"
#include "test_config.h"
#include <iostream>

void testDenuanayNetwork() {
    cout << "test begins\n";
    CDenuanayNetwork *dn = new CDenuanayNetwork();
    dn->CTInSAR_incre(
            DATA_ROOT_PATH+"CTI/PSC.rmg",
            DATA_ROOT_PATH+"CTI/PSC.ldr",
            DATA_ROOT_PATH+"CTI/edge.rmg",
            DATA_ROOT_PATH+"CTI/edge.ldr"
            );
    cout << "test finished\n";
}

int main() {
    testDenuanayNetwork();
    return 0;
}
