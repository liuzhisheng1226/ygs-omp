#include <iostream>
#include <fstream>
#include <complex>

using namespace std;

void testDemo() {
    cout << "test begins\n";
    string file1 = "/home/liutao/workspace/ygs_data/SLC/coaReg/2.rmg";
    string file2 = "/home/liutao/workspace/win_data/SLC/coaReg/2.rmg";
    //CRMGImage img1(file1);
    //CRMGImage img2(file2);
    ifstream if1 (file1.c_str(), ios::in | ios::binary);
    ifstream if2 (file2.c_str(), ios::in | ios::binary);
    int cols = 18707;
    int rows = 21180;
    for (int i = 0; i < rows; ++i) {
        complex<short> *data1 = new complex<short>[cols];
        complex<short> *data2 = new complex<short>[cols];
        if1.read((char *)data1, sizeof(complex<short>)*cols);
        if2.read((char *)data2, sizeof(complex<short>)*cols);
        complex<short> tmp1;
        complex<short> tmp2;
        for (int j = 0; j < cols; ++j) {
            //if (abs(data1[j].real() - data2[j].real()) < 10e-6 && abs(data2[j].imag() - data2[j].imag()) < 10e-6) 
            if (data1[j].real() == data2[j].real() && data1[j].imag() == data2[j].imag())
                continue;
            printf("line %d: %d %d %d %d %d\n", j, data1[j].real(), j, data2[j].real(), data1[j].imag(), data2[j].imag());
        }
        delete[] data1;
        delete[] data2;
    }
    cout << "test finished\n\n";
}

int main() {
    testDemo();
    return 0;
}
