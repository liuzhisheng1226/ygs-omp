#include "../Plugins/PfxCTInSAR/Integration.h"
#include "test_config.h"
#include <iostream>

void testIntegration() {
    cout << "test begins\n";
    CIntegration *ig = new CIntegration();
    ig->IntegrationPro(
            DATA_ROOT_PATH+"CTI/edge.rmg",
            DATA_ROOT_PATH+"CTI/edge.ldr",
            DATA_ROOT_PATH+"CTI/PSC.rmg", 
            DATA_ROOT_PATH+"CTI/PSC.ldr",
            0.3, 10, 3000, 9, 22
            ); 
    cout << "test finished\n";
}

int main() {
    testIntegration();
    return 0;
}
