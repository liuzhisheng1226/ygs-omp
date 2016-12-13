#pragma once
#include "../../Share/Core.h"
#include "../../Share/Common.h"
#include "Delaunay.h"

#include <vector>

using namespace std;

class CPSC : public CIProcessable
{
public:
    CPSC(void);
    ~CPSC(void);

    void Process();
    void Batch();

    void CreatePSC(string filecoh,string filetime,string outFile,vector<string> filepha,
                vector<string> filebn,vector<string> fileinc, vector<string> fileslt,
                int height,int width,int numsar,int numdiff,int roi_x0,int roi_y0,int roi_h0,int roi_w0,float wl);
        //读取差分相位
    void CTInSAR_ReadPha(vector<string> filepha,float time[],int nFileIndex,int height,int width, pscSet& PSpoints,
                int roi_x0,int roi_y0,int roi_h0,int roi_w0);

    void  CTInSAR_ReadBIS(vector<string> filebn,vector<string> fileinc, vector<string> fileslt,int nFileIndex,
        int height,int width, pscSet& PSpoints,int roi_x0,int roi_y0,int roi_h0,int roi_w0);
};

