#include "../Plugins/PfxCTInSAR/Baseline.h"
#include "test_config.h"
#include <iostream>

void testBaseline() {
    cout << "test begins\n";
    CBaseline *bl = new CBaseline();
    string m[] = {"1", "1", "2", "2", "3"};
    string s[] = {"2", "4", "3", "4", "4"};
    for (int i = 0; i < 5; ++i) {
        bl->BaselineInangleSlate(DATA_ROOT_PATH+"fineReg/"+m[i]+".rmg",
                                 DATA_ROOT_PATH+"fineReg/"+s[i]+".rmg",
                                 1,
                                 DATA_ROOT_PATH+"fineReg/"+m[i]+".orb",
                                 DATA_ROOT_PATH+"fineReg/"+s[i]+".orb",
                                 4,
                                 2,
                                 DATA_ROOT_PATH+"CTI/BIR/b"+m[i]+"-"+s[i]+".rmg",
                                 DATA_ROOT_PATH+"CTI/BIR/i"+m[i]+"-"+s[i]+".rmg",
                                 DATA_ROOT_PATH+"CTI/BIR/r"+m[i]+"-"+s[i]+".rmg");
    }
    cout << "test finished\n";
}

int main() {
    testBaseline();
    return 0;
}
