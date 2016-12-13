#pragma once
#include "SpMatrix.h"
#include <cstddef>
#include <cstdlib>
#include <cmath>

/// <summary>
/// ��ͳ����(����)
/// </summary>
/// <remarks>�����ͳ�ơ����Իع�</remarks>
template<class T>
class CSpStatistics
{
public:
    CSpStatistics()
    {
    
    }
    
    /// <summary>
    /// ���캯��
    /// </summary>
    /// <remarks>���ydataΪ�գ���ֻ��xdata���м�ͳ�����㣻�����xdata��ydata�������Իع����㣻xdata��ydata���ȱ�����ͬ</remarks>
    /// <param name="xdata">ͳ������x</param>
    /// <param name="length">���ݳ���</param>
    /// <param name="ydata">ͳ������y(ȱʡΪNULL)</param>
    CSpStatistics(T *xdata,unsigned int length,T* ydata=NULL)
    {
        this->Set(xdata,length,ydata);
        ydata==NULL?Do():DoAdvanced();
    }

    /// <summary>
    /// ���캯��(ͳ�ƾ���)
    /// </summary>
    /// <remarks>���ymatrixΪ�գ���ֻ��xdata���м�ͳ�����㣻�����xmatrix��ymatrix�������Իع����㣻xmatrix��ymatrix��С������ͬ</remarks>
    /// <param name="xmatrix">ͳ�ƾ���x</param> 
    /// <param name="ymatrix">ͳ�ƾ���y(ȱʡΪNULL)</param>
    CSpStatistics(const CSpMatrix<T> &xmatrix,const CSpMatrix<T> &ymatrix=NULL)
    {
        this->Set(xmatrix.m_pData,xmatrix.m_nLength,ymatrix.m_pData);
        this->Set(xmatrix,ymatrix);     
        ymatrix==NULL?Do():DoAdvanced();
    }   
    

    /// <summary>
    /// ��������
    /// </summary>
    /// <remarks>�˷����ڹ��캯�����ѵ���</remarks>
    /// <param name="xdata">ͳ������x</param>
    /// <param name="length">���ݳ���</param>
    /// <param name="ydata">ͳ������y(ȱʡΪNULL)</param>
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
    /// ����ͳ�Ʒ���
    /// </summary>
    /// <remarks>�˷����ڹ��캯�����ѵ���</remarks> 
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
    /// ��չͳ�ơ����Թ滮
    /// </summary>
    /// <remarks>�˷����ڹ��캯�����ѵ���</remarks>
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
    /// ��������ͳ��
    /// </summary>
    /// <remarks>�˷�����Ҫ��������</remarks>
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
    /// ��ʼ��ͳ�Ʊ���
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
    
    T* m_xdata;                     ///<xͳ������
    
    T* m_ydata;                     ///<yͳ������
    

public:
    
    unsigned int m_Length;          ///<ͳ�����ݳ���
    //X����ͳ�Ʊ�������
    double m_xMean;                 ///<x�ľ�ֵ
    double m_xSum;                  ///<x�ĺ�
    double m_xStdev;                ///<x�ı�׼��
    double m_xVar;                  ///<x�ķ���
    double m_xSqSum;                ///<x��ƽ����
    double m_xSqMean;               ///<x��ƽ���;�ֵ
    
    //Y����ͳ�Ʊ������� 
    double m_yMean;                 ///<y�ľ�ֵ
    double m_ySum;                  ///<y�ĺ�
    double m_yStdev;                ///<y�ı�׼��
    double m_yVar;                  ///<y����
    double m_ySqSum;                ///<y��ƽ����
    double m_ySqMean;               ///<y��ƽ���;�ֵ

    //���Իع��������
    double m_Covariance;            ///<Э����
    double m_Coefficient;           ///<���ϵ��
    double m_ProductSum;            ///<�˻���
};

