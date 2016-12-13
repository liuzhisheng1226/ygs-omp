#include "SpMatrix.h"
#include <iostream>

using namespace std;

int main() {
    CSpMatrix<int> *spm = new CSpMatrix<int>(1,2);
    cout << spm->m_pData[0] << endl;
    return 0;
}
