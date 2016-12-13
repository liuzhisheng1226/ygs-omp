#pragma once
#include <complex>
#include "../../Share/Core.h"
#include "../../Share/Common.h"
#include <string>
using namespace std;

/*
*@name : CRegistrFine
*@func : 精配准操作
*
*/
class CRegistrFine : public CIProcessable
{
public:
    CRegistrFine(void);
    ~CRegistrFine(void);
    

    void Process();
    void Batch();

    void Fine(string lpDataIn1,string lpHdrIn1,
              string lpDataIn2,string lpHdrIn2,
              string lpDataOutM,string lpHdrOutM,
              string lpDataOut,string lpHdrOut);

    void oversample(float *A, short factor,short l,short p,float *Result);
    int fft2d(float *xr,float *xi,int nx,int ny);
    int getIndex(int i);
    int ifft2d(float *xr,float *xi,int nx,int ny);
    void fft1d(double *xr,double *xi,int ln);

    void Get_para_gcp(float *basePosX,float *basePosY,float *slavePosX,float *slavePosY,CSpMatrix<double> &colCoef,CSpMatrix<double> &rowCoef,int pointN);

    void Resample(string lpDataIn1,string lpHdrIn1,
                  string lpDataIn2,string lpHdrIn2,
                  string lpDataOut,string lpHdrOut,
                  CSpMatrix<double> rowCoef,CSpMatrix<double> colCoef,double newAoffset);
    
    bool ReSampleImg_Master(string inMfile,string inHdrfile,string outMfile,string outHdrfile,int colM,int rowM,int dtype);
    
    /*complex<float>* BlockDiff(CRMGImage image,
        int blockIndex,
        int blockHeight, 
        complex<float> *block,
        CSpMatrix<double> &xMtxCoef,
        CSpMatrix<double> &yMtxCoef);*/


public:
    //const int blockDepth=200;                                 //该值越大，对偏差越大的主辅图像越合适
    const int CONST_BLOCK_DEPTH;
    //const int blockDiff=600;
    const int CONST_BLOCK_DIFF;                                 //差值高度


    /**********************************************
    *插值函数
    *indata，outdata，输入输出数据
    *Aef，Bef，列和行的多项式拟合系数
    *startR，输入辅图像块的起始行（从0开始计
    *i，需要插值的行数（outdata对应的行）
    *colS，rowS：输入辅图像块的列数和行数
    *colM：主图像的列数
    *
    ******************************************************/
    template<class T>
    void Interpolate(T *indata,complex<float> *outdata,
        CSpMatrix<double> Aef,CSpMatrix<double> Bef,int startR,
        int i,int colS,int rowS,int colM)
    {
        int r1,c1,r2,c2;
        float posC,posR;
        float alpha,beta;
        for(int j=0;j<colM;j++)
        {
            posC = Aef[0] + Aef[1]*j + Aef[2]*i+Aef[3]*j*j +Aef[4]*i*j + Aef[5]*i*i;        //列
            posR = Bef[0] + Bef[1]*j + Bef[2]*i+Bef[3]*j*j +Bef[4]*i*j + Bef[5]*i*i;        //行

        //  posC = Aef.Get(0,0)+Aef.Get(1,0)*j+Aef.Get(2,0)*i+Aef.Get(3,0)*j*j+Aef.Get(4,0)*j*i+Aef.Get(5,0)*i*i;
        //  posR = Bef.Get(0,0)+Bef.Get(1,0)*j+Bef.Get(2,0)*i+Bef.Get(3,0)*j*j+Bef.Get(4,0)*j*i+Bef.Get(5,0)*i*i;
            //posC = Aef[0] + Aef[1]*(j+1)+ Aef[2]*(i+1) 
            //  +Aef[3]*(j+1)*(j+1) +Aef[4]*(i+1)*(j+1) + Aef[5]*(i+1)*(i+1);       //列
            //posR = Bef[0] + Bef[1]*(j+1) + Bef[2]*(i+1)
            //  +Bef[3]*(j+1)*(j+1) +Bef[4]*(i+1)*(j+1) + Bef[5]*(1+i)*(1+i);       //行

            c1 = floor(posC); r1 = floor(posR);

            alpha = posC-c1;            //横向
            beta  = posR-r1;            //纵向

            c2 = c1+1;      //
            r1 = r1- startR;
            r2 = r1+1;

            //边缘：双线性插值              
            if((r1 ==0 && c1>=0 && c1<colS-1)||(c1==0 && r1>=0 && r1<rowS-1)
                ||(c1==colS-2 && r1>=0 && r1<rowS-1))
            {
                T slave1,slave2,slave3,slave4;
                slave1 = indata[r1*colS+c1];
                slave2 = indata[r1*colS+c2];
                slave3 = indata[r2*colS+c1];
                slave4 = indata[r2*colS+c2];

                outdata[j] = (complex<float>(slave1.real(),slave1.imag()))*(1-alpha)*(1-beta)
                    +alpha*(1-beta)*(complex<float>(slave2.real(),slave2.imag()))
                    +(1-alpha)*beta*(complex<float>(slave3.real(),slave3.imag()))
                    +alpha*beta*(complex<float>(slave4.real(),slave4.imag()));
            }
            //内部：三次立方插值
            else if(r1 > 0 && c1>0 && c1<colS-2 && r2< rowS-2)
            {
                double A[4];
                double C[4];
                T Bint[4][4];
                complex<float> B[4][4];
                //纵向
                A[0]=4-8*(1+alpha)+5*(1+alpha)*(1+alpha)-(1+alpha)*(1+alpha)*(1+alpha);     
                A[1]=1-2*alpha*alpha+alpha*alpha*alpha;
                A[2]=1-2*(1-alpha)*(1-alpha)+(1-alpha)*(1-alpha)*(1-alpha);
                A[3]=4-8*(2-alpha)+5*(2-alpha)*(2-alpha)-(2-alpha)*(2-alpha)*(2-alpha);
                //横向
                C[0]=4-8*(1+beta)+5*(1+beta)*(1+beta)-(1+beta)*(1+beta)*(1+beta);
                C[1]=1-2*beta*beta+beta*beta*beta;
                C[2]=1-2*(1-beta)*(1-beta)+(1-beta)*(1-beta)*(1-beta);
                C[3]=4-8*(2-beta)+5*(2-beta)*(2-beta)-(2-beta)*(2-beta)*(2-beta);
                for(int k=0;k<4;k++)
                {
                    for(unsigned short t=0;t<4;t++)
                    {
                        Bint[k][t]=indata[(r1-1+k)*colS+c1-1+t];
                        B[k][t] = complex<float>(Bint[k][t].real(),Bint[k][t].imag());
                    }
                }
                complex<float> D[4];
                for(int k=0;k<4;k++)
                {
                    D[k]=(float)A[0]*B[0][k]+(float)A[1]*B[1][k]+(float)A[2]*B[2][k]+(float)A[3]*B[3][k];
                }
                outdata[j]=D[0]*(float)C[0]+D[1]*(float)C[1]+D[2]*(float)C[2]+D[3]*(float)C[3];
            }
            //之外：0
            else
            {
                outdata[j] =  complex<float>(0.0,0.0);
            }
        }
        return;
    };

};

