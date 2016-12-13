#include "SpMatrix.h"
#include "SpStatistics.h"
#include <iostream>

using namespace std;

int main() {
    CSpMatrix<int> *spm = new CSpMatrix<int>(1,2);
    CSpStatistics<int> *sps = new CSpStatistics<int>(spm->m_pData, 2, spm->m_pData);
    cout << sps->m_Length << endl;
    return 0;
}
