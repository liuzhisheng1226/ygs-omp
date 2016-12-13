/// @file SpMatrix.h
/// @brief 简单矩阵类

#pragma once
#include <cstring>
/// <summary>
/// 简单矩阵类(泛型)
/// </summary>
/// <remarks>完成矩阵相关操作</remarks>
template<class T=double>
class CSpMatrix
{
public:

    
    /// <summary>
    /// 构造函数
    /// </summary>
    /// <remarks>通过行，列初始化矩阵，所有值均初始化为0</remarks>
    /// <param name="m">矩阵行</param>
    /// <param name="n">矩阵列</param>
    CSpMatrix(unsigned int m, unsigned int n)
    {
        this->m_nRow=m;
        this->m_nCol=n;
        this->m_nLength=m*n;
        this->m_pData=new T[m_nLength];     
        SetValues(0);
    }
    /// <summary>
    /// 构造函数
    /// </summary>
    /// <remarks>通过行，列初始化矩阵，并将所有值设置为value</remarks>
    /// <param name="m">矩阵行</param>
    /// <param name="n">矩阵列</param>
    /// <param name="value">矩阵值</param>
    CSpMatrix(unsigned int m, unsigned int n,T value)
    {
        this->m_nRow=m;
        this->m_nCol=n;
        this->m_nLength=m*n;
        this->m_pData=new T[m_nLength];     
        SetValues(value);
    }
    /// <summary>
    /// 构造函数
    /// </summary>
    /// <remarks>通过已知矩阵初始化矩阵，新生成的矩阵为已知矩阵的深拷贝</remarks>
    /// <param name="mat">矩阵</param>  
    CSpMatrix(const CSpMatrix<T>& mat)
    {
        //深拷贝数据
        this->m_nRow=mat.m_nRow;
        this->m_nCol=mat.m_nCol;
        this->m_nLength=m_nRow*m_nCol;
        this->m_pData=new T[m_nLength];
        memcpy(this->m_pData,mat.m_pData,m_nLength*sizeof(T));      
    }


    /// <summary>
    /// 析构函数
    /// </summary>
    ~CSpMatrix(void)
    {
        delete[] m_pData;
    }

    /// <summary>
    /// 矩阵赋值操作
    /// </summary>  
    /// <param name="m">矩阵行</param>
    /// <param name="n">矩阵列</param>
    /// <param name="value">值</param>  
    void Set(unsigned int m, unsigned int n,T value)
    {
        this->m_pData[m*m_nCol+n]=value;
    }   

    /// <summary>
    /// 获取当前位置数值
    /// </summary>  
    /// <param name="m">矩阵行</param>
    /// <param name="n">矩阵列</param>
    /// <returns>值</returns>
    T Get(unsigned int m, unsigned int n)
    {
        return m_pData[m*m_nCol+n];
    }

    
    /// <summary>
    /// 设置矩阵默认值
    /// </summary>      
    /// <remarks>此方法在构造函数中已调用</remarks>
    /// <param name="value">将所有矩阵值设置为value</param> 
    void SetValues(T value)
    {
        for(int i=0;i<m_nLength;i++)
            m_pData[i]=value;
    }

    /// <summary>
    /// 设置矩阵默认值
    /// </summary>              
    /// <param name="value">将所有矩阵值设置为value[],value[]为长度为m*n的数组</param>
    void SetMatrix(T values[])
    {       
        for(int i=0;i<m_nLength;i++)
            m_pData[i]=values[i];
    }
    

    
    /// <summary>
    /// [] 操作符重载
    /// </summary>  
    /// <remarks>使矩阵可以通过下标访问其元素</remarks>
    /// <param name="n">元素下标，此下标为一维下标</param>
    T &operator[](unsigned int n)
    {
        return this->m_pData[n];
    }
    
    /// <summary>
    /// + 操作符重载
    /// </summary>  
    /// <remarks>使矩阵可以通过下标访问其元素</remarks>
    /// <param name="n">元素下标，此下标为一维下标</param>
    /// <returns>结果矩阵</returns>
    CSpMatrix<T> operator+(CSpMatrix& mt)
    {
        CSpMatrix<T> matrix(this->m_nRow,this->m_nCol);
        for(int i=0;i<this->m_nLength;i++)
            matrix[i]=this->m_pData[i]+mt.m_pData[i];
        return matrix;
    }

    /// <summary>
    /// = 操作符重载
    /// </summary>      
    /// <param name="mt">赋值矩阵</param>
    /// <returns>结果矩阵</returns>
    CSpMatrix<T> &operator=(const CSpMatrix &mt)
    {
        this->m_nRow=mt.m_nRow;
        this->m_nCol=mt.m_nCol;
        this->m_nLength=mt.m_nLength;
        delete []this->m_pData;
        this->m_pData=new T[m_nLength];
        for(int i=0;i<m_nLength;i++)
            m_pData[i]=mt.m_pData[i];
        return *this;
    }
    
    /// <summary>
    /// * 操作符重载
    /// </summary>      
    /// <remarks>使矩阵可以通过*操作符完成矩阵相乘操作，进行举证相乘时请注意相乘两举证行列大小，否则可能产生位置错误</remarks>
    /// <param name="mt">赋值矩阵</param>
    /// <returns>结果矩阵</returns>
    CSpMatrix<T> operator*(CSpMatrix &mt)
    {
        CSpMatrix<T> matrix(this->m_nRow,mt.m_nCol);                
        for(int m=0;m<m_nRow;m++)       
            for(int n=0;n<mt.m_nCol;n++)            
                for(int k=0;k<m_nCol;k++)                       
                    matrix[m*mt.m_nCol+n]+=this->m_pData[m*m_nCol+k]*mt[k*mt.m_nCol+n];     
        return matrix;
    }

    /// <summary>
    /// 矩阵转置操作
    /// </summary>      
    /// <remarks>将当前矩阵转置</remarks>   
    /// <returns>结果矩阵</returns>
    CSpMatrix<T> Transpose()
    {
        CSpMatrix<T> matrix(this->m_nCol,this->m_nRow);
        int m,n;
        for(m=0;m<m_nRow;m++)
            for(n=0;n<m_nCol;n++)
                matrix.Set(n,m,this->m_pData[m*m_nCol+n]);      
        return matrix;
    }

    /// <summary>
    /// 矩阵过采样
    /// </summary>      
    /// <remarks>求当前矩阵的过采样矩阵</remarks>   
    /// <returns>结果矩阵</returns>
    CSpMatrix<T> Oversample(int factor)
    {
        CSpMatrix<T> matrix(this->m_nRow*factor,this->m_nCol*factor);
        int m,n;
        T result; 
        for(m=0;m<m_nRow*factor;m++)
        {
            for(n=0;n<m_nCol*factor;n++)
            {
                int lowx=n/factor;
                int lowy=m/factor;

                float xx= float(n)/factor-lowx;
                float yy= float(m)/factor-lowy;
                
                if(lowx<m_nCol-1&&lowy<m_nRow-1)
                {
                    result=this->m_pData[lowy*m_nRow+lowx]*(1-xx)*(1-yy)
                        +this->m_pData[lowy*m_nRow+lowx+1]*xx*(1-yy)
                        +this->m_pData[(lowy+1)*m_nRow+lowx]*(1-xx)*yy
                        +this->m_pData[(lowy+1)*m_nRow+lowx+1]*xx*yy;
                    matrix.Set(m,n,result);
                }
                if(m==(m_nRow-1)*factor)  //最后一行
                {
                    result=this->m_pData[lowy*m_nRow+lowx];
                    matrix.Set(m,n,result);
                }
                if(n==(m_nCol-1)*factor)
                {
                    result=this->m_pData[lowy*m_nRow+lowx];
                    matrix.Set(m,n,result);
                }

            }
        }
        return matrix;
    }

    
    /// <summary>
    /// 矩阵求逆操作
    /// </summary>      
    /// <remarks>求当前矩阵的逆矩阵</remarks>   
    /// <returns>结果矩阵</returns>
    CSpMatrix<T> Inverse()
    {
        CSpMatrix<T> matrix(this->m_nRow,this->m_nCol);
        T a1,a2;
        for(int i=0;i<m_nRow;i++)
            matrix.Set(i,i,1);                  //单位矩阵

        for(int k=0;k<m_nRow;k++)
        {
            int indx=Pivot(k);
            if(indx!=0) matrix.SwapRow(k,indx);
            a1=this->Get(k,k);
            for(int j=0;j<m_nRow;j++)
            {
                this->Set(k,j,
                    this->Get(k,j)/a1);
                matrix.Set(k,j,
                    matrix.Get(k,j)/a1);
            }//for j
            for(int i=0;i<m_nRow;i++)
            {
                if(i!=k)
                {
                    a2=this->Get(i,k);
                    for(int j=0;j<m_nRow;j++)
                    {
                        this->Set(i,j,this->Get(i,j)-a2*this->Get(k,j));
                        matrix.Set(i,j,matrix.Get(i,j)-a2*matrix.Get(k,j));
                    }//for
                }//if
            }//for i        
        }//for k
        return matrix;
    }//Inverse


    /// <summary>
    /// 求初等行变换
    /// </summary>      
    /// <param name="row">矩阵行号</param>
    /// <returns>矩阵行号</returns>
    int Pivot(int row)
    {
        int k=row;
        double t=0,amax=-1;
        for(int i=row;i<m_nRow;i++)
        {
            if((t=fabs(this->Get(i,row)))>amax&&t!=0)
            {
                amax=t;
                k=i;
            }//if
        }//for i
        if(this->Get(k,row)==0) return-1;
        if(k!=row)
        {
            SwapRow(k,row);
            return k;
        }
        return 0;
    }

    /// <summary>
    /// 交换当前矩阵指定两行数据
    /// </summary>      
    /// <param name="row1">矩阵行号1</param>
    /// <param name="row2">矩阵行号2</param>    
    void SwapRow(int row1,int row2)
    {       
        T* data=new T[m_nCol];
        memcpy(data,m_pData+row1*m_nCol,sizeof(T)*m_nCol);
        memcpy(m_pData+row1*m_nCol,m_pData+row2*m_nCol,sizeof(T)*m_nCol);
        memcpy(m_pData+row1*m_nCol,data,sizeof(T)*m_nCol);
        delete[] data;
    }
    



    

    /// <summary>
    /// 求矩阵最小二乘解 
    /// </summary>      
    /// <remarks>求解k*x=y 中x的最小二乘解</remarks>    
    /// <returns>x最小二乘解矩阵</returns>
    static CSpMatrix<T> Solve(CSpMatrix<T> kmat,CSpMatrix<T> ymat)
    {
        CSpMatrix<T> tmat=kmat.Transpose();
        CSpMatrix<T> ttmat=tmat*kmat;
        CSpMatrix<T> tymat=tmat*ymat;
        CSpMatrix<T> imat=ttmat.Inverse();
        return imat*tymat;
    }

public:
    
    unsigned int m_nRow;                    ///<矩阵行大小
    
    unsigned int m_nCol;                    ///<矩阵列大小
    
    unsigned long m_nLength;                ///<矩阵大小
    
    T* m_pData;                             ///<矩阵数据
};

