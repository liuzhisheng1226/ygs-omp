#pragma once
#include "../../Share/Core.h"
#include "../../Share/Common.h"
#include <complex>
#include <string>

class CRegistrCoarse : public CIProcessable
{
public:
    CRegistrCoarse(void);
    ~CRegistrCoarse(void);


    /*
    *   继承接口方法，必须实现
    */
    void Process();
    void Batch();

    

    void Coarse(string lpDataFullname1,string lpHdrFullname1,string lpDataFullname2,string lpHdrFullname2,string lpDataExport,string lpHdrExport);

    /*
     * 计算粗配准偏移
     */
    void CoarseOffset(CRMGImage &image1,CRMGImage &image2,int &colOffset,int &rowOffset);
    /*
     * 读取粗配准指定大小的子区域
     */
    float* CoarseBlock(CRMGImage &image,int blockWidth,int blockHeight,Rect window);
    /*
     * 计算相应图像对应的行列号
     */
    //DOUBLE LocCoordinate(CRMGImage &image,int &row,int &col,double x3d,double y3d,double z3d);
    /*
     *实现多项式
     */
    //DOUBLE Polynomial(CSpMatrix<double> &aef,CSpMatrix<double> &bef,CSpMatrix<double> &cef,double t,double x,double y,double z);
    /*
     *多项式拟合，该公式主要实现以下重写以下宏定义
    */
    //DOUBLE Polyfit(CSpMatrix<double> &mat,double t,int order);
    
    /*
     *计算多项式order阶系数
     *@para: n      多项式指数
     *@para: order  当前阶数
     */
    //LONG PolyCoef(int n,int order);

};

