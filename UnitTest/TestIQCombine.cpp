#include "../Plugins/PfxConvertor/IQCombine.h"
#include "test_config.h"

void testIQCombine() {
    CIQCombine *iqc = new CIQCombine();
    cout << "test begins\n";
    for (int i = 1; i < 5; ++i) {
        iqc->IqBandsCombine(
                DATA_ROOT_PATH+"ori/" + to_string(i) + ".rmg", 
                DATA_ROOT_PATH+"SLC/" + to_string(i) + ".rmg");
    }
    cout << "test finished\n";
}

int main() {
    testIQCombine(); 
    return 0;
}
