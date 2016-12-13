#pragma once
#include "SpMatrix.h"
#include <cstddef>
#include <cstdlib>
#include <cmath>

/// <summary>
/// 简单统计类(泛型)
/// </summary>
/// <remarks>计算简单统计、线性回归</remarks>
template<class T>
class CSpStatistics
{
public:
    CSpStatistics()
    {
    
    }
    
    /// <summary>
    /// 构造函数
    /// </summary>
    /// <remarks>如果ydata为空，则只对xdata进行简单统计运算；否则对xdata和ydata进行线性回归运算；xdata和ydata长度必须相同</remarks>
    /// <param name="xdata">统计数据x</param>
    /// <param name="length">数据长度</param>
    /// <param name="ydata">统计数据y(缺省为NULL)</param>
    CSpStatistics(T *xdata,unsigned int length,T* ydata=NULL)
    {
        this->Set(xdata,length,ydata);
        ydata==NULL?Do():DoAdvanced();
    }

    /// <summary>
    /// 构造函数(统计矩阵)
    /// </summary>
    /// <remarks>如果ymatrix为空，则只对xdata进行简单统计运算；否则对xmatrix和ymatrix进行线性回归运算；xmatrix和ymatrix大小必须相同</remarks>
    /// <param name="xmatrix">统计矩阵x</param> 
    /// <param name="ymatrix">统计矩阵y(缺省为NULL)</param>
    CSpStatistics(const CSpMatrix<T> &xmatrix,const CSpMatrix<T> &ymatrix=NULL)
    {
        this->Set(xmatrix.m_pData,xmatrix.m_nLength,ymatrix.m_pData);
        this->Set(xmatrix,ymatrix);     
        ymatrix==NULL?Do():DoAdvanced();
    }   
    

    /// <summary>
    /// 设置数据
    /// </summary>
    /// <remarks>此方法在构造函数中已调用</remarks>
    /// <param name="xdata">统计数据x</param>
    /// <param name="length">数据长度</param>
    /// <param name="ydata">统计数据y(缺省为NULL)</param>
    void Set(T *xdata,unsigned int length,T* ydata=NULL)
    {
        this->m_xdata=xdata;
        this->m_Length=length;      
        this->m_ydata=ydata;
        
    }
    //void Set(const CSpMatrix<T> &xmatrix,const CSpMatrix<T> &ymatrix=NULL)
    //{
    //  this->m_xdata=xmatrix.m_pData;
    //  this->m_Length=xmatrix.m_nLength;       

    //  this->m_ydata=ymatrix.m_pData;      
    //}


    /// <summary>
    /// 基本统计方法
    /// </summary>
    /// <remarks>此方法在构造函数中已调用</remarks> 
    void Do()
    {
        Init();
        for(int i=0;i<m_Length;i++)
        {
            this->m_xSum+=this->m_xdata[i];
            this->m_xSqSum+=this->m_xdata[i]*this->m_xdata[i];
        }//for i
        this->m_xMean=m_xSum/m_Length;
        this->m_xSqMean=m_xSqSum/m_Length;
        this->m_xVar=this->m_xSqMean-m_xMean*m_xMean;
        this->m_xStdev=sqrt(m_xVar);
    }   


    /// <summary>
    /// 扩展统计、线性规划
    /// </summary>
    /// <remarks>此方法在构造函数中已调用</remarks>
    void DoAdvanced()
    {
        Init();
        for(int i=0;i<m_Length;i++)
        {
            this->m_xSum+=m_xdata[i];
            this->m_xSqSum+=this->m_xdata[i]*this->m_xdata[i];
            
            this->m_ySum+=m_ydata[i];
            this->m_ySqSum+=this->m_ydata[i]*this->m_ydata[i];

            this->m_ProductSum+=m_xdata[i]*m_ydata[i];
        }//for i

        this->m_xMean=m_xSum/m_Length;
        this->m_xSqMean=m_xSqSum/m_Length;
        this->m_xVar=this->m_xSqMean-m_xMean*m_xMean;
        this->m_xStdev=sqrt(m_xVar);
        
        this->m_yMean=m_ySum/m_Length;
        this->m_ySqMean=m_ySqSum/m_Length;
        this->m_yVar=this->m_ySqMean-m_yMean*m_yMean;
        this->m_yStdev=sqrt(m_yVar);

        m_Covariance=m_ProductSum-m_xMean*m_yMean;
        m_Coefficient=fabs(m_Covariance)/sqrt(m_xVar*m_yVar);

    }

    /// <summary>
    /// 复数类型统计
    /// </summary>
    /// <remarks>此方法需要单独调用</remarks>
    void DoComplex()
    {
        Init();             
        for(int i=0;i<m_Length;i++)
        {
            this->m_xSum+=abs(this->m_xdata[i]);
            this->m_xSqSum+=abs(this->m_xdata[i])*abs(this->m_xdata[i]);
        }//for i
        this->m_xMean=m_xSum/m_Length;
        this->m_xSqMean=m_xSqSum/m_Length;
        this->m_xVar=this->m_xSqMean-m_xMean*m_xMean;
        this->m_xStdev=sqrt(m_xVar);
    }

    /// <summary>
    /// 初始化统计变量
    /// </summary>  
    void Init()
    {
        this->m_xMean=0;
        this->m_xSum=0;
        this->m_xStdev=0;
        this->m_xVar=0;
        this->m_xSqSum=0;
        this->m_xSqMean=0;

        this->m_yMean=0;
        this->m_ySum=0;
        this->m_yStdev=0;
        this->m_yVar=0;
        this->m_ySqSum=0;
        this->m_ySqMean=0;

        this->m_ProductSum=0;
        this->m_Covariance=0;
        this->m_Coefficient=0;

    }

private:
    
    T* m_xdata;                     ///<x统计数据
    
    T* m_ydata;                     ///<y统计数据
    

public:
    
    unsigned int m_Length;          ///<统计数据长度
    //X基本统计变量定义
    double m_xMean;                 ///<x的均值
    double m_xSum;                  ///<x的和
    double m_xStdev;                ///<x的标准差
    double m_xVar;                  ///<x的方差
    double m_xSqSum;                ///<x的平方和
    double m_xSqMean;               ///<x的平方和均值
    
    //Y基本统计变量定义 
    double m_yMean;                 ///<y的均值
    double m_ySum;                  ///<y的和
    double m_yStdev;                ///<y的标准差
    double m_yVar;                  ///<y方差
    double m_ySqSum;                ///<y的平方和
    double m_ySqMean;               ///<y的平方和均值

    //线性回归变量定义
    double m_Covariance;            ///<协方差
    double m_Coefficient;           ///<相关系数
    double m_ProductSum;            ///<乘积和
};

