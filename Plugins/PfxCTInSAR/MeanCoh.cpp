#include "MeanCoh.h"
#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>
#include <fstream>
/*OMP SUPPORT*/
// #include <omp.h>
// #include <sys/sysinfo.h>
using namespace std;
/*FILE*/
//#include <cstdio.h>
CMeanCoh::CMeanCoh(void)
{
}

void CMeanCoh::Process()
{
/*    CDlgMeanCoh dlg;*/
    //if(dlg.DoModal()!=IDOK) return;
    /*MeanCoherence(dlg.m_fileIn,dlg.m_fileOut,dlg.m_fileOutH,dlg.m_width,dlg.m_height);*/
}

void CMeanCoh::Batch()
{
    
}

void CMeanCoh::MeanCoherence(string fileIn,string fileOut,string fileOutH,int width,int height)
{
    string batfile=fileIn;
    vector<string> filein;

    FILE *fp;
    char buf[200];
    fp=fopen(batfile.c_str(),"r");
    int num=0;
    while(!feof(fp))
    {
        fscanf(fp,"%s",buf);
        filein.push_back(buf);
        num++;
    }
    fclose(fp);

    //开始计算   按行计算
    float *data=new float[width*height];
    memset(data,0,sizeof(float)*width*height);
    float *outdata=new float[width*height];
    memset(outdata, 0,sizeof(float)*width*height);
    for(int i=0;i<num;i++)
    {
        // float *data=new float[width*height];
        ifstream file1(filein[i], ios::in|ios::binary);
        if (!file1.is_open()) {
            cout << "error open image file1\n";
            exit(1) ;
        }
        file1.read((char *)data,sizeof(float)*width*height);
        file1.close();

        //求和
        for(int j=0;j<height;j++)
        {
            for(int k=0;k<width;k++)
            {
                outdata[j*width+k] +=data[j*width+k];
            }
        }
    }

    //求均值
    for(int j=0;j<height;j++)
    {
        for(int k=0;k<width;k++)
        {
            outdata[j*width+k] /=num;
        }
    }
    //文件输出
    ofstream file2(fileOut,ios::binary|ios::out );
    if(!file2.is_open()){
        cout<<"open output file failed\n";
        exit(1);
    }
    file2.write((char *)outdata,sizeof(float)*width*height);
    file2.close();

    //头文件输出
    CRMGImage image(filein[1]);
    image.m_oHeader.Save(fileOutH);

    delete[] data;
    delete[] outdata;

}

CMeanCoh::~CMeanCoh(void)
{
}
