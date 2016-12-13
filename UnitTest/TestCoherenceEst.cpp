#include "../Plugins/PfxInSAR/CoherenceEst.h"
#include "test_config.h"
#include <iostream>

void testCoherenceEst() {
    cout << "test begins\n";
    CoherenceEst *coh = new CoherenceEst();
    string m[] = {"1", "1", "2", "2", "3"};
    string s[] = {"2", "4", "3", "4", "4"};
    for (int i = 0; i < 5; ++i) {
        coh->CoherenceEstimate(DATA_ROOT_PATH+"fineReg/"+m[i]+".rmg",
                               DATA_ROOT_PATH+"fineReg/"+s[i]+".rmg",
                               DATA_ROOT_PATH+"flatPha/"+m[i]+"-"+s[i]+".rmg",
                               DATA_ROOT_PATH+"coh/"+m[i]+"-"+s[i]+".rmg",
                               4,
                               2);
    }
    cout << "test finished\n";
}

int main() {
    testCoherenceEst();
    return 0;
}
