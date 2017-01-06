#pragma once
#include "../../Share/Core.h"
#include "../../Share/Common.h"
#include <complex>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <omp.h>

class CComplexPro : public CIProcessable
{
public:
    CComplexPro(void);
    ~CComplexPro(void);

    void Process();
    void Batch();

    void ComplexConv(string strInpathm,string strOutPath,int iType);

    template<class T>
    void ComplexFun(CRMGImage image ,string lpDataExport,int iType)
    {
        ifstream fileImport(image.m_lpFullname.c_str(), ios::in|ios::binary);
        if (!fileImport.is_open()) {
            cout << "error open image fileImport\n";
            exit(1);
        }
        ofstream fileExport(lpDataExport.c_str(), ios::out|ios::binary);
        if (!fileExport.is_open()) {
            cout << "error open image fileExport\n";
            exit(1);
        }

        omp_lock_t r_lock, w_lock;
        omp_init_lock(&r_lock);
        omp_init_lock(&w_lock);

#pragma omp parallel for
        for(int i=0;i<image.m_oHeader.Line;i++)
        {
            complex<T> *data=new complex<T>[image.m_oHeader.Sample];
            float *outData=new float[image.m_oHeader.Sample];
            
            omp_set_lock(&r_lock);
            fileImport.seekg((streampos)sizeof(complex<T>)*i*image.m_oHeader.Sample, ios::beg);
            fileImport.read((char *)data, sizeof(complex<T>)*image.m_oHeader.Sample);
            omp_unset_lock(&r_lock);
            
            for(int j=0;j<image.m_oHeader.Sample;j++)
            {
                complex<float> tempData = complex<float>(data[j].real(),data[j].imag());
                if(iType == 0)
                    outData[j] = abs(tempData);
                else if(iType == 1)
                    outData[j] = arg(tempData);
            }
            omp_set_lock(&w_lock);
            fileExport.seekp((streampos)i*sizeof(float)*image.m_oHeader.Sample, ios::beg);
            fileExport.write((char *)outData, sizeof(float)*image.m_oHeader.Sample);
            omp_unset_lock(&w_lock);

            delete[] data;
            delete[] outData;
        }//for i

        omp_destroy_lock(&r_lock);
        omp_destroy_lock(&w_lock);

        fileImport.close();
        fileExport.close();
        return;
    }
};

