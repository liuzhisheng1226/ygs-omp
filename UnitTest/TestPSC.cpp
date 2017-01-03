#include "../Plugins/PfxCTInSAR/PSC.h"
#include <iostream>

void testPSC() {
    cout << "test begins\n";
    CPSC *psc = new CPSC();
    psc->Process();
    cout << "test finished\n";
}

int main() {
    testPSC();
    return 0;
}
