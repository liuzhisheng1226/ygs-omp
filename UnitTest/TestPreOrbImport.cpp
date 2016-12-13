#include "../Plugins/PfxInSAR/PreOrbImport.h"
#include "test_config.h"
#include <iostream>

void testRegistrCoarse() {
    cout << "test begins\n";
    CPreOrbImport *poi = new CPreOrbImport();
    string orbs[] = {
        "",
        "24951_def-120924.orb",
        "25980_def-121205.orb",
        "26666_DEF-130122.orb",
        "27009_DEF-130215.orb"
    };
    for (int i = 1; i <=4; ++i) {
        string s = to_string(i);
        poi->preOrb2Local(
                DATA_ROOT_PATH+"SLC/cal/"+s+".rmg",
                DATA_ROOT_PATH+"SLC/cal/"+s+".ldr",
                DATA_ROOT_PATH+"ori/"+orbs[i],
                DATA_ROOT_PATH+"fineReg/"+s+".orb",
                0,
                0.5);
    }
    cout << "test finished\n";
}

int main() {
    testRegistrCoarse();
    return 0;
}
