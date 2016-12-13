#include "Musk.h"
#include <fstream>
#include <iostream>

using namespace std;

CMusk::CMusk(void)
{
}

void CMusk::Batch()
{

}

void CMusk::Process()
{
/*    CDlgMusk dlg;*/
    //if(dlg.DoModal()!=IDOK) return;

    /*MuskCoherenceTarget(dlg.m_fileIn,dlg.m_fileOut,dlg.m_lowValue,dlg.m_highValue);*/
}

void CMusk::MuskCoherenceTarget(string fileIn,string fileOut,float lowValue, float highValue)
{
    //打开相干系数图
    CRMGImage image(fileIn);
    int width=image.m_oHeader.Sample;
    int height=image.m_oHeader.Line;

    float *data=new float[width*height];
    char *outdata=new char[width*height];

    memset(data,0,sizeof(float)*width*height);
    memset(outdata, 0,sizeof(char)*width*height);
    ifstream file1 (fileIn, ios::in);
    if (!file1.is_open()) {
        cout << "error open file\n";
        return;
    }
    file1.read((char *)data, sizeof(float)*width*height);
    file1.close();

    //输出文件
    ofstream file2 (fileOut, ios::out | ios::binary);
    if (!file2.is_open()) {
        cout << "error open file\n";
        return;
    }

    for(int i=0;i<height;i++)
    {
        for(int j=0;j<width;j++)
        {
            if((data[i*width+j]>=lowValue)&&(data[i*width+j]<=highValue))
            {
                outdata[i*width+j]=1;
            }
        }
    }

    //保存输出
    file2.write((char *)outdata, sizeof(char)*width*height);
    file2.close();

    delete[] data;
    delete[] outdata;

    //输出头文件
    CRMGHeader header(image.m_oHeader);
    header.DataType = ENUM_DATA_TYPE(1);
    fileOut.replace(fileOut.find("rmg"), 3, "ldr");

    header.Save(fileOut);

}

CMusk::~CMusk(void)
{
}
