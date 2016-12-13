#pragma once
#include "RMGBase.h"
#include "SpMatrix.h"
#include <string.h>

/// @file RMGImage.h 
/// @brief RMG图像定义文件
/// @author Zhaolong


/// <summary>
//// RMG图像类定义
/// </summary>
/// <remarks>定义RMG图像类，提供RMG图像公共操作、字段</remarks>
/// @ingroup Core
class CRMGImage : public CRMGBase
{
public:
    /// <summary>
    /// 构造函数
    /// </summary>
    /// <param name="lpDataFullname">数据文件路径</param>
    CRMGImage(string lpDataFullname);
    /// <summary>
    /// 构造函数
    /// </summary>
    /// <param name="lpDataFullname">数据文件路径</param>
    /// <param name="lpHdrFullname">头文件路径</param>
    CRMGImage(string lpDataFullname,string lpHdrFullname);
    /// <summary>
    /// 析构函数
    /// </summary>  
    ~CRMGImage(void);
    /// <summary>
    /// 经纬度坐标到笛卡尔坐标转换
    /// </summary>
    /// <param name="x">x坐标引用</param>
    /// <param name="y">y坐标引用</param>
    /// <param name="z">z坐标引用</param>
    void LonLat2Coordinate(double &x,double &y,double &z);

    //根据地面点坐标，求对该点成像时卫星的时刻
    double  getSatposT(double x, double y,double z,CSpMatrix<double> &xcoef,CSpMatrix<double> &ycoef,CSpMatrix<double> &zcoef,bool isPreorb=false);

    //原RMGInSAR类，现放到rmgImage中,根据卫星状态矢量stateVector求卫星轨道(x,y,z)的多项式拟合系数
    void OrbitCoef(vector<CRMGHeader::StructStateVector> stateVector,CSpMatrix<double> &xcoef,CSpMatrix<double> &ycoef,CSpMatrix<double> &zcoef);

    //根据插值精轨的卫星状态矢量求卫星轨道的多项式拟合系数
    void PreorbCoef(vector<CRMGHeader::StructStateVector> stateVector,CSpMatrix<double> &xcoef,CSpMatrix<double> &ycoef,CSpMatrix<double> &zcoef);

    
    //实现多项式，计算（satX-x）*satVx+（satY-y）*satVy+(satZ-z)*satVz,在成像时刻其值=0
    double Polynomial(CSpMatrix<double> &aef,CSpMatrix<double> &bef,CSpMatrix<double> &cef,double t,double x,double y,double z);
    
    //多项式拟合，该公式主要实现以下重写以下宏定义，根据order求pos或是速度v
    double Polyfit(CSpMatrix<double> &mat,double t,int order);


   //跟迭代法求椭球体上的坐标
    void Newton(double t,double semia, double semib,CSpMatrix<double> xPolyCoef,CSpMatrix<double> yPolyCoef,CSpMatrix<double> zPolyCoef,
            double R,double &x, double &y, double &z,int num);


};

