#include "../Plugins/pfxRoiSelect/RoiSelectPro.h"
#include "test_config.h"
#include <iostream>

void testRoiSelectPro() {
    cout << "test begins\n";
    CRoiSelectPro *rsp = new CRoiSelectPro();
    for (int i = 1; i < 5; ++i) {
        rsp->getRoi(
                DATA_ROOT_PATH+"ori/"+to_string(i)+".rmg",
                3401,
                8001,
                3200,
                1600,
                DATA_ROOT_PATH+"ROI/"+to_string(i)+".rmg");
    cout << "test finished\n";

    }
}

int main() {
    testRoiSelectPro();
    return 0;
}
