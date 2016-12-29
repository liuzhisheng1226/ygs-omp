#include <vector>
#include <fstream>
#include "R2Calibration.h"
#include <omp.h>

CR2Calibration::CR2Calibration(void)
{
}

CR2Calibration::~CR2Calibration(void)
{
}

void CR2Calibration::Process()
{
/*    CDlgR2Calibration dlg;*/
    //if(dlg.DoModal()!=IDOK)
        //return;
    /*R2CalibrationPro(dlg.m_strInfile,dlg.m_strLutfile,dlg.m_strOutfile);*/
}

void CR2Calibration::Batch()
{
    //R2CalibrationPro(m_aryStrs.GetAt(0),m_aryStrs.GetAt(1),m_aryStrs.GetAt(2));
}

void CR2Calibration::R2CalibrationPro(string lpImport,string lpLutfile,string lpExport)
{
    CRMGImage inImg(lpImport);
    FILE *fp;
    if ((fp = fopen(lpLutfile.c_str(),"r")) == NULL) {
        cout << "can't open LUT file\n";
        return;
    }
    char buf[200];
    float fGain;
    int count=0;
    vector<float> gainVestor;
    string bufString;
    while (!feof(fp)) {
      fscanf(fp, "%s", buf);
      if (feof(fp)) break;
      bufString = buf;
      if (bufString.find("<gains>") != string::npos) {
        fGain = atof((bufString.substr(7)).c_str());
        const float temp = fGain;
        gainVestor.push_back(temp);
        count++;
        fscanf(fp, "%s", buf);
        while (strcmp(buf, "</lut>")) {
          fGain = atof(buf);
          const float tempp = fGain;
          gainVestor.push_back(tempp);
          count++;
          fscanf(fp, "%s", buf);
        }
      }
    }
    float offset;
    offset = 0.000;
    fclose(fp);

    int iCols = inImg.m_oHeader.Sample;
    int iRows = inImg.m_oHeader.Line;

    ifstream inFile(lpImport.c_str(), ios::in | ios::binary);
    if (!inFile.is_open()) {
        cout << "error open image inFile\n";
        return;
    }
    ofstream outFile(lpExport.c_str(), ios::out | ios::binary | ios::app);
    if (!outFile.is_open()) {
        cout << "open out file outFile failed\n";
        return;
    }

    if(inImg.m_oHeader.DataType == eCINT16)
    {
        omp_lock_t r_lock, w_lock;
        omp_init_lock(&r_lock);
        omp_init_lock(&w_lock);

#pragma omp parallel for
        for(int i=0;i<iRows;i++)
        {
            complex<float> *outData = new complex<float>[iCols];
            complex <short> *indata = new complex<short>[iCols];
            omp_set_lock(&r_lock);
            inFile.seekg((streampos)i*sizeof(complex<short>)*iCols);
            inFile.read((char *)indata,sizeof(complex<short>)*iCols);
            omp_unset_lock(&r_lock);
            for(int j=0;j<iCols;j++)
            {
                complex<float> temp = complex<float>(indata[j].real(),indata[j].imag());
                outData[j] = temp/gainVestor[j];            //norm(temp)/gainVestor.begin()[j]/gainVestor.begin()[j];               
            }
            omp_set_lock(&w_lock);
            outFile.seekp((streampos)i*sizeof(complex<float>)*iCols);
            outFile.write((char *)outData,sizeof(complex<float>)*iCols);
            omp_unset_lock(&w_lock);
            delete[] indata;
            delete[] outData;
        }
        omp_destroy_lock(&r_lock);
        omp_destroy_lock(&w_lock);
    }//c-short
    else if(eCFLOAT32 == inImg.m_oHeader.DataType)
        cout << "don't support float data yet\n";
        

    inFile.close();
    outFile.close();
    gainVestor.clear();

    CRMGHeader header(inImg.m_oHeader);
    header.DataType = eCFLOAT32;    
    lpExport.replace(lpExport.find(".rmg"), 4, ".ldr");
    header.Save(lpExport);
}
