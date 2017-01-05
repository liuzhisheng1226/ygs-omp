#pragma once
#include <iostream>
#include <string>
#include "../../Share/Core.h"
#include "../../Share/Common.h"
#include <fstream>
#include <complex>
#include <omp.h>

using namespace std;

class CIQCombine : public CIProcessable
{
public:
    CIQCombine(void);
    ~CIQCombine(void);

    void Process();
    void Batch();

    void IqBandsCombine(string lpImport,string lpExport);
    

    /*
     *@name :    IqCombine
     *@func :    template method for IQ combine
     */
    template<class T>
    void Bands2Complex(CRMGImage image ,string lpDataExport)
        {
            cout << "enter Bands2Complex" << endl;
            ifstream fileImport (image.m_lpFullname, ios::in | ios::binary);
            if (fileImport.is_open() == false) {
                cout << "Error opening fileImport" << endl;
                exit(1);
            }
            ofstream fileExport (lpDataExport, ios::out | ios::binary);
            if (fileExport.is_open() == false) {
                cout << "Error openning fileExport" << endl;
                exit(1);
            }
            long offset=sizeof(T)*(image.m_oHeader.Line-1)*image.m_oHeader.Sample;
            cout << image.m_oHeader.Line << " " << image.m_oHeader.Sample << endl;

            omp_lock_t r_lock, w_lock;
            omp_init_lock(&r_lock);
            omp_init_lock(&w_lock);
#pragma omp parallel for
            for(int i=0;i<image.m_oHeader.Line;i++)
            {
                T *data=new T[2* image.m_oHeader.Sample];
                complex<T> *cdata=new complex<T>[image.m_oHeader.Sample];
                omp_set_lock(&r_lock);
                fileImport.seekg(sizeof(T)*i*image.m_oHeader.Sample, ios::beg);
                fileImport.read((char *)data,sizeof(T)*image.m_oHeader.Sample);
                fileImport.seekg(offset, ios::cur);
                fileImport.read((char *)(data+image.m_oHeader.Sample), sizeof(T)*image.m_oHeader.Sample);
                omp_unset_lock(&r_lock);
                for(int j=0;j<image.m_oHeader.Sample;j++)
                    cdata[j]=complex<T>(data[j],data[j+image.m_oHeader.Sample]);
                omp_set_lock(&w_lock);
                fileExport.seekp((streampos)i*sizeof(complex<T>)*image.m_oHeader.Sample, ios::beg);
                fileExport.write((char *)cdata, sizeof(complex<T>)*image.m_oHeader.Sample);
                omp_unset_lock(&w_lock);
                delete[] data;
                delete[] cdata;
            }
            omp_destroy_lock(&r_lock);
            omp_destroy_lock(&w_lock);
         
            cout << "read and write done\n";
            fileImport.close();
            fileExport.close();
            cout << "exit Bands2Complex" << endl;
            return;
        };

};

