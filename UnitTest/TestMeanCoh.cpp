#include "../Plugins/PfxCTInSAR/MeanCoh.h"
#include "test_config.h"
#include <iostream>

void testMeanCoh() {
    cout << "test begins\n";
    CMeanCoh *mc = new CMeanCoh();
    mc->MeanCoherence(
            DATA_ROOT_PATH+"CTI/coherence.txt", 
            DATA_ROOT_PATH+"CTI/meanCoh.rmg",
            DATA_ROOT_PATH+"CTI/meanCoh.ldr",
            1600,
            3200);
    cout << "test finished\n";
}

int main() {
    testMeanCoh();
    return 0;
}
