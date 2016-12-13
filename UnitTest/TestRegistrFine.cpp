#include "../Plugins/PfxInSAR/RegistrFine.h"
#include "test_config.h"
#include <iostream>

void testRegistrCoarse() {
    cout << "test begins\n";
    CRegistrFine *rf = new CRegistrFine();
    for (int i = 2; i < 5; ++i)
        rf->Fine(
                DATA_ROOT_PATH+"SLC/cal/1.rmg",
                DATA_ROOT_PATH+"SLC/cal/1.ldr",
                DATA_ROOT_PATH+"coaReg/"+to_string(i)+".rmg",
                DATA_ROOT_PATH+"coaReg/"+to_string(i)+".ldr",
                DATA_ROOT_PATH+"fineReg/1.rmg",
                DATA_ROOT_PATH+"fineReg/1.ldr",
                DATA_ROOT_PATH+"fineReg/"+to_string(i)+".rmg",
                DATA_ROOT_PATH+"fineReg/"+to_string(i)+".ldr");
    cout << "test finished\n";
}

int main() {
    testRegistrCoarse();
    return 0;
}
