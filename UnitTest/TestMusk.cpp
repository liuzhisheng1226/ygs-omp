#include "../Plugins/PfxCTInSAR/Musk.h"
#include "test_config.h"
#include <iostream>

void testMusk() {
    cout << "test begins\n";
    CMusk *m = new CMusk();
    m->MuskCoherenceTarget(
            DATA_ROOT_PATH+"CTI/meanCoh.rmg",
            DATA_ROOT_PATH+"CTI/CTmusk.rmg",
            0.7,
            1);
    cout << "test finished\n";
}

int main() {
    testMusk();
    return 0;
}
