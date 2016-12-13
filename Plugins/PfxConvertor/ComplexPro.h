#pragma once
#include "../../Share/Core.h"
#include "../../Share/Common.h"
#include <complex>
#include <cstdio>

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
        FILE *fileImport,*fileExport;
        fileImport=fopen(image.m_lpFullname.c_str(),"rb");
        fileExport=fopen(lpDataExport.c_str(),"wb+");

        complex<T> *data=new complex<T>[image.m_oHeader.Sample];
        float *outData=new float[image.m_oHeader.Sample];
        complex<float> tempData;
        for(int i=0;i<image.m_oHeader.Line;i++)
        {
            fseek(fileImport,sizeof(complex<T>)*i*image.m_oHeader.Sample,SEEK_SET);
            fread(data,sizeof(complex<T>),image.m_oHeader.Sample,fileImport);
            
            for(int j=0;j<image.m_oHeader.Sample;j++)
            {
                tempData = complex<float>(data[j].real(),data[j].imag());
                if(iType == 0)
                    outData[j] = abs(tempData);
                else if(iType == 1)
                    outData[j] = arg(tempData);
            }
            fwrite(outData,sizeof(float),image.m_oHeader.Sample,fileExport);
        }//for i
        fclose(fileImport);
        fclose(fileExport); 
        return;
    }
};

