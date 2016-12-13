#include "../Plugins/PfxInSAR/FlatPhase.h"
#include "test_config.h"
#include <iostream>

void testFlatPhase() {
    cout << "test begins\n";
    CFlatPhase *fp = new CFlatPhase();
    string m[] = {"1", "1", "2", "2", "3"};
    string s[] = {"2", "4", "3", "4", "4"};
    for (int i = 0; i < 5; ++i) {
        fp->m_Flat_R2(DATA_ROOT_PATH+"fineReg/"+m[i]+".rmg",
                      DATA_ROOT_PATH+"fineReg/"+m[i]+".ldr",
                      DATA_ROOT_PATH+"fineReg/"+s[i]+".rmg",
                      DATA_ROOT_PATH+"fineReg/"+s[i]+".ldr",
                      DATA_ROOT_PATH+"flatPha/"+m[i]+"-"+s[i]+".rmg",
                      DATA_ROOT_PATH+"flatPha/"+m[i]+"-"+s[i]+".ldr",
                      1);
    }
    cout << "test finished\n";
}

int main() {
    testFlatPhase();
    return 0;
}
