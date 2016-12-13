#pragma once
#include "../../Share/Core.h"
#include "../../Share/Common.h"
#include <complex>
#include <cstdio>


class CRoiSelectPro : public CIProcessable
{
public:
    CRoiSelectPro(void);
    virtual ~CRoiSelectPro(void);

    void Process();
    void Batch();


    void getRoi(string lpInFilesName,unsigned int uiStartR, unsigned int uiStartC,unsigned int roiRows, unsigned int roiCols,
                string lpOutMname);


    /*
    *@name :    getRoiFun
    *@func :    处理子区选择
    */
    template<class T>
    static void getRoiFun(CRMGImage image ,string lpDataExport,unsigned int iStartR,unsigned int iStartC,unsigned int iRows, unsigned int iCols)
    {
         FILE *fileImport,*fileExport;
         fileImport=fopen(image.m_lpFullname.c_str(),"rb+");
         fileExport=fopen(lpDataExport.c_str(),"wb+");

         T *data=new T[iCols];
         for(int i=0;i<iRows;i++)
         {
             fseek(fileImport,sizeof(T)*((i+iStartR-1)*image.m_oHeader.Sample+iStartC-1),SEEK_SET);
             fread(data,sizeof(T),iCols,fileImport);
             fwrite(data,sizeof(T),iCols,fileExport);           
         }//for i
         
         fclose(fileImport);
         fclose(fileExport);    
        delete []data;
        data=NULL;

    }

};

