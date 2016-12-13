#pragma once
#include "../../Share/Core.h"
#include "../../Share/Common.h"
#include "Delaunay.h"
#include <vector>
#include <string>

using namespace std;

class CDenuanayNetwork : public CIProcessable
{
public:
    CDenuanayNetwork(void);

    //�������


    void Process();
    void Batch();

    void CTInSAR_incre(string strPscFile,string strPscFileH,string strEdgeFileOut,string strEdgeFileOutH);

    void CTInSAR_incre(string filecoh,string filetime,vector<string> filepha,
                vector<string> filebn,vector<string> fileinc, vector<string> fileslt,
                int height,int width,int numsar,int numdiff);
    //����ROI
    void CTInSAR_incre(string filecoh,string filetime,vector<string> filepha,
                vector<string> filebn,vector<string> fileinc, vector<string> fileslt,
                int height,int width,int numsar,int numdiff,int roi_x0,int roi_y0,int roi_h0,int roi_w0);

    //��ȡ�����λ
    void CTInSAR_ReadPha(vector<string> filepha,int nFileIndex,int height,int width, pscSet& PSpoints);
    void CTInSAR_ReadPha(vector<string> filepha,int nFileIndex,int height,int width, pscSet& PSpoints,
                int roi_x0,int roi_y0,int roi_h0,int roi_w0);

    void  CTInSAR_ReadBIS(vector<string> filebn,vector<string> fileinc, vector<string> fileslt,int nFileIndex,
        int height,int width, pscSet& PSpoints);
    void  CTInSAR_ReadBIS(vector<string> filebn,vector<string> fileinc, vector<string> fileslt,int nFileIndex,
        int height,int width, pscSet& PSpoints,int roi_x0,int roi_y0,int roi_h0,int roi_w0);

    //���Ż�����
    void  CTInSAR_Findopt(float options[],int numdiff,float time_c[],float diff[],float upbn[],float upinc[],float upslt[],float wl,float **result);

    //����ģ����λ������λ��ϵ
    float  Condition_v_h(float **V_H,float wl,int numdiff,float time_c[],float diff[],float upbn[],float upinc[],float upslt[]);


    ~CDenuanayNetwork(void);
};

