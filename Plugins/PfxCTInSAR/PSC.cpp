#include "PSC.h"
#include "../../UnitTest/test_config.h"
#include <iostream>
#include <fstream>
#include <cstdio>


CPSC::CPSC(void)
{
}

void CPSC::Process()
{
    string filecoh = DATA_ROOT_PATH + "CTI/CTmusk.rmg";
    string filetime = DATA_ROOT_PATH + "CTI/BT.txt";
    string outFile = DATA_ROOT_PATH + "CTI/PSC.rmg";
    string filepha = DATA_ROOT_PATH + "CTI/phase.txt";
    string filepara = DATA_ROOT_PATH + "CTI/BIR.txt";

    CRMGImage image(filecoh);
    int height = image.m_oHeader.Line;
    int width = image.m_oHeader.Sample;
    float wl = image.CONST_MATH_LIGHT_VELOCITY/image.m_oHeader.RadarCenterFrequency;

    int numsar = 4;
    int numdiff = 5;
    int roi_x = 1;
    int roi_y = 1;
    int roi_h = 0; 
    int roi_w = 0;
    bool roiCheck = false;

    vector<string> filephase;
    vector<string> filebase;
    vector<string> fileslt;
    vector<string> fileinc;

    int num = 0;
    FILE *fp;
    char buf[200];
    fp = fopen(filepha.c_str(), "r");
    while (!feof(fp)) {
        fscanf(fp, "%s", buf);
        if (feof(fp)) break;
        filephase.push_back(buf);
        num++;
    }
    fclose(fp);
    if (num != numdiff) {
        cout << "num = " << num << " != numdiff\n";
        return;
    }
    num = 0;
    fp = fopen(filepara.c_str(), "r");
    while (!feof(fp)) {
        fscanf(fp, "%s", buf);
        if (feof(fp)) break;
        filebase.push_back(buf);
        fscanf(fp, "%s", buf);
        fileinc.push_back(buf);
        fscanf(fp, "%s", buf);
        fileslt.push_back(buf);
        num++;
    }
    for (string s : filebase) cout << s << " ";
    cout << endl;
    for (string s : fileinc) cout << s << " ";
    cout << endl;
    for (string s : fileslt) cout << s << " ";
    cout << endl;
    fclose(fp);
    if (num != numdiff) {
        cout << "num = " << num << " != numdiff\n";
        return;
    }
    if (!roiCheck) {
        roi_x = 1; roi_y = 1;
        roi_h = height; roi_w = width;
    }

    CreatePSC(filecoh, filetime, outFile, filephase, filebase, fileinc, fileslt, height, width, numsar, numdiff, roi_x, roi_y, roi_h, roi_w, wl);
}

void CPSC::Batch() 
{
}

void CPSC::CreatePSC(string filecoh,string filetime,string outFile,vector<string> filepha,
                     vector<string> filebn,vector<string> fileinc, vector<string> fileslt,
                     int height,int width,int numsar,int numdiff,int roi_x,int roi_y,int roi_h,int roi_w,float wl)
{
    //第一步 读取相干点
    ifstream file1 (filecoh, ios::in);
    if (!file1.is_open()) {
        cout << "error open file\n";
        return;
    }

    pscSet PSpoints;  //CT点集合

    int offset;
    offset=sizeof(char)*((roi_y-1)*width+roi_x-1);
    file1.seekg(offset, ios::beg);

    char *coh=new char[roi_w];
    int numPoint=0;
    //读取相干点文件
    for(int i=0;i<roi_h;i++)
    {

        file1.read((char *)coh, sizeof(char)*roi_w);

        int numEdge=0;
        for(int j=0;j<roi_w;j++)
        {
            if(coh[j]==1)
            {
                PSpoints.insert(PSC(j+1,i+1,numPoint));  //点的index从0开始
                numPoint++;
            }
        }

        file1.seekg(sizeof(char)*(width-roi_w), ios::cur);
    }
    file1.close();

    //读取时间基线
    FILE *fp;
    char buf[200];
    float *time_c=new float[numdiff];
    memset(time_c,0,sizeof(float)*numdiff);
    int temp_num=0;
    fp=fopen(filetime.c_str(),"r");
    while(!feof(fp))
    {
        fscanf(fp,"%s",buf);
        if (feof(fp)) break;
        time_c[temp_num]=atof(buf);
        temp_num++;
    }
    fclose(fp); 

    //读取参数
    for(int nFileIndex=0;nFileIndex<numdiff;nFileIndex++ )
    {
        CTInSAR_ReadPha(filepha,time_c,nFileIndex,height,width,PSpoints,roi_x,roi_y,roi_h,roi_w);       //PSpoints= 

        //读取CT点垂直基线，入射角，斜距等参数
        CTInSAR_ReadBIS(filebn,fileinc,fileslt,nFileIndex,height,width,PSpoints,roi_x,roi_y,roi_h,roi_w);   //PSpoints=
    }

    //输出PSC点
    ofstream file_psc (outFile, ios::out);
    if (!file_psc.is_open()) {
        cout << "error open file\n";
        return;
    }

    for(pscIterator pi=PSpoints.begin();pi!=PSpoints.end();pi++)
    {
        file_psc.write((char *)(&(*pi)), sizeof(PSC));
    }
    file_psc.close();


    PSpoints.clear();

    //输出头文件
    outFile.replace(outFile.find("rmg"), 3, "ldr");
    fp=fopen(outFile.c_str(),"w");
    if(fp==NULL) return;

    fprintf(fp,"[width]");
    fprintf(fp,"\n\t\t%d",width);
    fprintf(fp,"\n[height]");
    fprintf(fp,"\n\t\t%d",height);
    fprintf(fp,"\n[roi_y]");
    fprintf(fp,"\n\t\t%d",roi_y);
    fprintf(fp,"\n[roi_x]");
    fprintf(fp,"\n\t\t%d",roi_x);
    fprintf(fp,"\n[roi_w]");
    fprintf(fp,"\n\t\t%d",roi_w);
    fprintf(fp,"\n[roi_h]");
    fprintf(fp,"\n\t\t%d",roi_h);
    fprintf(fp,"\n[numPoint]");
    fprintf(fp,"\n\t\t%d",numPoint);
    fprintf(fp,"\n[numdiff]");
    fprintf(fp,"\n\t\t%d",numdiff);
    fprintf(fp,"\n[numsar]");
    fprintf(fp,"\n\t\t%d",numsar);
    fprintf(fp,"\n[wavelength]");
    fprintf(fp,"\n\t\t%f",wl);

    fclose(fp);
    //释放内存
    delete []time_c;
    time_c=NULL;
}

//差分相位读取
void CPSC::CTInSAR_ReadPha(vector<string> filepha,float time[],int nFileIndex,int height,int width, pscSet& PSpoints,
                           int roi_x0,int roi_y0,int roi_h0,int roi_w0)
{
    pscSet PssetTemp=PSpoints;

    string temp=filepha[nFileIndex];
    ifstream file1 (temp, ios::in);
    if (!file1.is_open()) {
        cout << "error open file\n";
        return;
    }

    float **pha=new float*[roi_h0];
    for(int i=0;i<roi_h0;i++)
    {
        pha[i]=new float[roi_w0];
    }
    int offset;
    offset=sizeof(float)*((roi_y0-1)*width+roi_x0-1);
    file1.seekg(offset, ios::beg);

    for(int i=0;i<roi_h0;i++)
    {
        file1.read((char *)pha[i], sizeof(float)*roi_w0);
        file1.seekg(sizeof(float)*(width-roi_w0), ios::cur);
    }
    file1.close();

    PSC  pstemp;  
    for(pscIterator Pi=PSpoints.begin();Pi!=PSpoints.end();Pi++)
    {
        pstemp =(*Pi);
        pstemp.data[nFileIndex]=pha[(int)pstemp.m_pos.Y-1][(int)pstemp.m_pos.X-1];   //行列
        pstemp.deltaD[nFileIndex]=time[nFileIndex];
        pscIterator found =PssetTemp.find(*Pi);
        PssetTemp.erase(found);
        PssetTemp.insert(pstemp);
        
    }

    PSpoints=PssetTemp;

    for(int i=0;i<roi_h0;i++)
    {
        delete[]pha[i];

    }
    delete pha;

    return;
}

//读取基线，入射角，斜距
void CPSC::CTInSAR_ReadBIS(vector<string> filebn,vector<string> fileinc, vector<string> fileslt,int nFileIndex,
                           int height,int width, pscSet& PSpoints,int roi_x0,int roi_y0,int roi_h0,int roi_w0)
{
    pscSet PssetTemp=PSpoints;

    ifstream file1 (filebn[nFileIndex], ios::in);
    if (!file1.is_open()) {
        cout << "error open file\n";
        return;
    }
    ifstream file2 (fileinc[nFileIndex], ios::in);
    if (!file2.is_open()) {
        cout << "error open file\n";
        return;
    }
    ifstream file3 (fileslt[nFileIndex], ios::in);
    if (!file3.is_open()) {
        cout << "error open file\n";
        return;
    }

    float **bn=new float*[roi_h0];
    float **inc=new float*[roi_h0];
    double **slc=new double*[roi_h0];

    for(int i=0;i<roi_h0;i++)
    {

        bn[i]=new float[roi_w0];
        inc[i]=new float[roi_w0];
        slc[i]=new double[roi_w0];

    }
    int offset,offset1;
    offset=sizeof(float)*((roi_y0-1)*width+roi_x0-1);
    offset1=sizeof(double)*((roi_y0-1)*width+roi_x0-1);
    file1.seekg(offset, ios::beg);
    file2.seekg(offset, ios::beg);
    file3.seekg(offset, ios::beg);
    for(int i=0;i<roi_h0;i++)
    {

        file1.read((char *)bn[i], sizeof(float)*roi_w0);
        file2.read((char *)inc[i], sizeof(float)*roi_w0);
        file3.read((char *)slc[i], sizeof(double)*roi_w0);

        file1.seekg(sizeof(float)*(width-roi_w0), ios::cur);
        file2.seekg(sizeof(float)*(width-roi_w0), ios::cur);
        file3.seekg(sizeof(double)*(width-roi_w0), ios::cur);

    }
    file1.close();
    file2.close();
    file3.close();


    PSC pstemp; 
    for(pscIterator Pi=PSpoints.begin();Pi!=PSpoints.end();Pi++)
    {
 
        pstemp=(*Pi);
        
        pstemp.bv[nFileIndex]=bn[(int)pstemp.m_pos.Y-1][(int)pstemp.m_pos.X-1];
        pstemp.iAngle[nFileIndex]=inc[(int)pstemp.m_pos.Y-1][(int)pstemp.m_pos.X-1];
        pstemp.sRange[nFileIndex]=slc[(int)pstemp.m_pos.Y-1][(int)pstemp.m_pos.X-1];
        pscIterator found =PssetTemp.find(*Pi);
        PssetTemp.erase(found);
        PssetTemp.insert(pstemp);
    }
    PSpoints=PssetTemp;

    for(int i=0;i<roi_h0;i++)
    {
        delete[]bn[i];
        delete[]inc[i];
        delete[]slc[i];


    }
    delete bn;
    delete inc;
    delete slc;
    return ;
}
CPSC::~CPSC(void)
{
}
