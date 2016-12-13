#include "../Plugins/PfxConvertor/R2Calibration.h"
#include "test_config.h"

void testR2Calibration() {
    cout << "test begins\n";
    CR2Calibration *r2cal = new CR2Calibration();
    for (int i = 1; i < 5; ++i) {
        r2cal->R2CalibrationPro(
                DATA_ROOT_PATH+"SLC/" + to_string(i) + ".rmg",
                DATA_ROOT_PATH+"ori/lutSigma" + to_string(i) + ".xml",
                DATA_ROOT_PATH+"SLC/cal/" + to_string(i) + ".rmg");
    }
    cout << "test finished\n";
}

int main() {
    testR2Calibration();
    return 0;
}
