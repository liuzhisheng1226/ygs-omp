#include <vector>
#include <fstream>
#include "R2Calibration.h"

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

    complex<float> *outData = new complex<float>[iCols];
    if(inImg.m_oHeader.DataType == eCINT16)
    {
        for(int i=0;i<iRows;i++)
        {
            {
                complex <short> *indata = new complex<short>[iCols];
                complex<float> temp;
                inFile.read((char *)indata,sizeof(complex<short>)*iCols);
                for(int j=0;j<iCols;j++)
                {
                    temp = complex<float>(indata[j].real(),indata[j].imag());
                    outData[j] = temp/gainVestor[j];            //norm(temp)/gainVestor.begin()[j]/gainVestor.begin()[j];               
                }
                delete[] indata;
            }
            outFile.write((char *)outData,sizeof(complex<float>)*iCols);
        }
    }//c-short
    else if(eCFLOAT32 == inImg.m_oHeader.DataType)
        cout << "don't support float data yet\n";
        

    inFile.close();
    outFile.close();
    gainVestor.clear();
    delete[] outData;

    CRMGHeader header(inImg.m_oHeader);
    header.DataType = eCFLOAT32;    
    lpExport.replace(lpExport.find(".rmg"), 4, ".ldr");
    header.Save(lpExport);
}
