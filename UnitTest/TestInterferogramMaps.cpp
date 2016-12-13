#include "../Plugins/PfxInSAR/InterferogramMaps.h"
#include "test_config.h"

void testInterferogramMaps() {
    cout << "test begins\n";
    CInterferogramMaps *cifm = new CInterferogramMaps();
    cifm->InterferoGramMapAnalysis(DATA_ROOT_PATH+"SARdata.txt",
                                   DATA_ROOT_PATH+"inSARgraphy.txt",
                                   200,
                                   200);
    cout << "test finished\n";
}

int main() {
    testInterferogramMaps();
    return 0;
}
