#include "../Plugins/PfxInSAR/RegistrCoarse.h"
#include "test_config.h"

void testRegistrCoarse() {
    cout << "test begins\n";
    CRegistrCoarse *rc = new CRegistrCoarse(); 
    for (int i = 2; i < 5; ++i) 
        rc->Coarse(
                DATA_ROOT_PATH+"SLC/cal/1.rmg",
                DATA_ROOT_PATH+"SLC/cal/1.ldr",
                DATA_ROOT_PATH+"SLC/cal/"+to_string(i)+".rmg",
                DATA_ROOT_PATH+"SLC/cal/"+to_string(i)+".ldr",
                DATA_ROOT_PATH+"coaReg/"+to_string(i)+".rmg",
                DATA_ROOT_PATH+"coaReg/"+to_string(i)+".ldr");
    cout << "test finished\n";
}

int main() {
    testRegistrCoarse();
    return 0;
}
