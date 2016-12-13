#pragma once
#include "../../Share/Core.h"
#include "../../Share/Common.h"
#include <cstdio>
#include <complex>
#include <string>

using namespace std;

class CMultiLook : public CIProcessable
{
public:
    CMultiLook(void);
    ~CMultiLook(void);


    void Process();
    void Batch();

    //void MultiLookPro(string lpImport,string lpExport,unsigned int rLooks, unsigned int cLooks);  
    void MultiLookPro(string lpImport,string lpExport,unsigned int rLooks, unsigned int cLooks);

    /*
    *@name :    MultiLookFun
    *@func :    IQ通道合成泛型方法
    */
    template<class T>
    void MultiLookFun(CRMGImage image ,string lpDataExport,unsigned int rLooks, unsigned int cLooks)
    {
         FILE *fileImport,*fileExport;
         fileImport=fopen(image.m_lpFullname.c_str(),"rb+");
         fileExport=fopen(lpDataExport.c_str(),"wb+");

         int newRow = image.m_oHeader.Line/rLooks;
         int newCol = image.m_oHeader.Sample/cLooks;

         complex<T> *data = new complex<T>[rLooks* image.m_oHeader.Sample];
         complex<T> *outData = new complex<T>[newCol];
         memset(outData,0,sizeof(complex<T>)*newCol);
         complex<T> temp;
         for(int i=0;i<newRow;i++)
         {
             fread(data,sizeof(complex<T>),image.m_oHeader.Sample*rLooks,fileImport);
             for(int j=0;j<newCol;j++)
             {
                 temp = complex<T>(0,0);
                 for(int ii=0;ii<rLooks;ii++)
                 {
                     for(int jj=0;jj<cLooks;jj++)
                         temp += data[ii*image.m_oHeader.Sample+(j*cLooks+jj)];
                 }
                 outData[j] = complex<T>(temp.real()/rLooks/cLooks,temp.imag()/rLooks/cLooks);
             }
             fwrite(outData,sizeof(complex<T>),newCol,fileExport);
         }//for i   

         fclose(fileImport);
         fclose(fileExport);    
         delete[] data;
         delete[] outData;
         return;
    };
};
