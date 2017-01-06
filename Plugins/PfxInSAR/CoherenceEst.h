#pragma once
#include "../../Share/Core.h"
#include <complex>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <omp.h>
using namespace std;
class CoherenceEst : public CIProcessable
{
public:
    CoherenceEst(void);
    ~CoherenceEst(void);

    /*
    *   继承接口方法，必须实现
    */
    void Process();
    void Batch();

    void CoherenceEstimate(string strInM,string strInS,string strInFlat,string strOutCoh,int rLooks,int sLooks);

    //template<class T>
    void CoherenceFun(string inM,string inS,string inFlat,string strOutCoh,unsigned int rLooks, unsigned int cLooks,int iCols,int iRows)
    {
        ifstream fInM(inM.c_str(), ios::in|ios::binary);
        if (!fInM.is_open()) {
            cout << "error open image fInM\n";
            exit(1) ;
        }
        ifstream fInS(inS.c_str(), ios::in|ios::binary);
        if (!fInS.is_open()) {
            cout << "error open image fInS\n";
            exit(1) ;
        }
        ifstream fInFlat(inFlat.c_str(), ios::in|ios::binary);
        if (!fInFlat.is_open()) {
            cout << "error open image fInFlat\n";
            exit(1) ;
        }

        ofstream fOutCoh(strOutCoh.c_str(), ios::out|ios::binary);
        if (!fOutCoh.is_open()) {
            cout << "error open output fOutCoh  image\n";
            exit(1);
        }

        /*FIXME*/
        /*rlook and clook range?*/
        int newRow = iRows/rLooks;
        int newCol = iCols/cLooks;

        omp_lock_t r_lock, w_lock;
        omp_init_lock(&r_lock);
        omp_init_lock(&w_lock);

#pragma omp parallel for
        for(int i=0;i<newRow;i++)
        {
            complex<float> *mData = new complex<float>[rLooks*iCols];
            complex<float> *sData = new complex<float>[rLooks*iCols];
            float *ftData = new float[rLooks*iCols];
            complex<float> *outData = new complex<float>[newCol];
            memset(outData,0,sizeof(complex<float>)*newCol);
        
            complex<float> covar,ftTemp;
            float mCov,sCov;

            omp_set_lock(&r_lock);
            fInM.seekg((streampos)i*sizeof(complex<float>)*rLooks*iCols, ios::beg);
            fInM.read((char *)mData,sizeof(complex<float>)*rLooks*iCols);
            fInS.seekg((streampos)i*sizeof(complex<float>)*rLooks*iCols, ios::beg);
            fInS.read((char *)sData,sizeof(complex<float>)*rLooks*iCols);
            fInFlat.seekg((streampos)i*sizeof(float)*rLooks*iCols, ios::beg);
            fInFlat.read((char *)ftData,sizeof(float)*rLooks*iCols);
            omp_unset_lock(&r_lock);

            for(int j=0;j<newCol;j++)
            {
                covar = complex<float>(0,0);
                mCov =0.0,sCov = 0.0;
                for(int ii=0;ii<rLooks;ii++)
                {
                    for(int jj=0;jj<cLooks;jj++)
                    {
                        ftTemp = complex<float>(cos(ftData[ii*iCols+(j*cLooks+jj)]),-sin(ftData[ii*iCols+(j*cLooks+jj)]));
                        covar += mData[ii*iCols+(j*cLooks+jj)]*conj(sData[ii*iCols+(j*cLooks+jj)])*ftTemp;

                        mCov += norm(mData[ii*iCols+(j*cLooks+jj)]);
                        sCov += norm(sData[ii*iCols+(j*cLooks+jj)]);
                    }
                }
                mCov = sqrt(mCov);
                sCov = sqrt(sCov);
                if(mCov*sCov != 0)
                    outData[j] = complex<float>(covar.real()/float(mCov*sCov),covar.imag()/float(mCov*sCov));
                else
                    outData[j] = complex<float>(0.0,0.0);
            }
            omp_set_lock(&w_lock);
            fOutCoh.seekp((streampos)i*sizeof(complex<float>)*newCol, ios::beg);
            fOutCoh.write((char *)outData,sizeof(complex<float>)*newCol);
            omp_unset_lock(&w_lock);

            delete[] mData;
            delete[] sData;
            delete[] ftData;
            delete[] outData;
        }//for i    

        omp_destroy_lock(&r_lock);
        omp_destroy_lock(&w_lock);

        fInM.close();
        fInS.close();
        fInFlat.close();
        fOutCoh.close();
        return;
    }
};

