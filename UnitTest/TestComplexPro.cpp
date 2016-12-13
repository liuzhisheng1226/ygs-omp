#include "../Plugins/PfxConvertor/ComplexPro.h"
#include "test_config.h"
#include <iostream>

void testComplexPro() {
    cout << "test begins\n";
    CComplexPro *cp = new CComplexPro();
    string s[] = {"1-2", "1-4", "2-3", "2-4", "3-4"};
    for (int i = 0; i < 5; ++i) {
        cp->ComplexConv(DATA_ROOT_PATH+"coh/"+s[i]+".rmg",
                        DATA_ROOT_PATH+"coh/amp/"+s[i]+".rmg",
                        0);
        cp->ComplexConv(DATA_ROOT_PATH+"coh/"+s[i]+".rmg",
                        DATA_ROOT_PATH+"coh/pha/"+s[i]+".rmg",
                        1);

    }
    cout << "test finished\n";
}

int main() {
    testComplexPro();
    return 0;
}
