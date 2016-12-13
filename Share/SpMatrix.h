/// @file SpMatrix.h
/// @brief �򵥾�����

#pragma once
#include <cstring>
/// <summary>
/// �򵥾�����(����)
/// </summary>
/// <remarks>��ɾ�����ز���</remarks>
template<class T=double>
class CSpMatrix
{
public:

    
    /// <summary>
    /// ���캯��
    /// </summary>
    /// <remarks>ͨ���У��г�ʼ����������ֵ����ʼ��Ϊ0</remarks>
    /// <param name="m">������</param>
    /// <param name="n">������</param>
    CSpMatrix(unsigned int m, unsigned int n)
    {
        this->m_nRow=m;
        this->m_nCol=n;
        this->m_nLength=m*n;
        this->m_pData=new T[m_nLength];     
        SetValues(0);
    }
    /// <summary>
    /// ���캯��
    /// </summary>
    /// <remarks>ͨ���У��г�ʼ�����󣬲�������ֵ����Ϊvalue</remarks>
    /// <param name="m">������</param>
    /// <param name="n">������</param>
    /// <param name="value">����ֵ</param>
    CSpMatrix(unsigned int m, unsigned int n,T value)
    {
        this->m_nRow=m;
        this->m_nCol=n;
        this->m_nLength=m*n;
        this->m_pData=new T[m_nLength];     
        SetValues(value);
    }
    /// <summary>
    /// ���캯��
    /// </summary>
    /// <remarks>ͨ����֪�����ʼ�����������ɵľ���Ϊ��֪��������</remarks>
    /// <param name="mat">����</param>  
    CSpMatrix(const CSpMatrix<T>& mat)
    {
        //�������
        this->m_nRow=mat.m_nRow;
        this->m_nCol=mat.m_nCol;
        this->m_nLength=m_nRow*m_nCol;
        this->m_pData=new T[m_nLength];
        memcpy(this->m_pData,mat.m_pData,m_nLength*sizeof(T));      
    }


    /// <summary>
    /// ��������
    /// </summary>
    ~CSpMatrix(void)
    {
        delete[] m_pData;
    }

    /// <summary>
    /// ����ֵ����
    /// </summary>  
    /// <param name="m">������</param>
    /// <param name="n">������</param>
    /// <param name="value">ֵ</param>  
    void Set(unsigned int m, unsigned int n,T value)
    {
        this->m_pData[m*m_nCol+n]=value;
    }   

    /// <summary>
    /// ��ȡ��ǰλ����ֵ
    /// </summary>  
    /// <param name="m">������</param>
    /// <param name="n">������</param>
    /// <returns>ֵ</returns>
    T Get(unsigned int m, unsigned int n)
    {
        return m_pData[m*m_nCol+n];
    }

    
    /// <summary>
    /// ���þ���Ĭ��ֵ
    /// </summary>      
    /// <remarks>�˷����ڹ��캯�����ѵ���</remarks>
    /// <param name="value">�����о���ֵ����Ϊvalue</param> 
    void SetValues(T value)
    {
        for(int i=0;i<m_nLength;i++)
            m_pData[i]=value;
    }

    /// <summary>
    /// ���þ���Ĭ��ֵ
    /// </summary>              
    /// <param name="value">�����о���ֵ����Ϊvalue[],value[]Ϊ����Ϊm*n������</param>
    void SetMatrix(T values[])
    {       
        for(int i=0;i<m_nLength;i++)
            m_pData[i]=values[i];
    }
    

    
    /// <summary>
    /// [] ����������
    /// </summary>  
    /// <remarks>ʹ�������ͨ���±������Ԫ��</remarks>
    /// <param name="n">Ԫ���±꣬���±�Ϊһά�±�</param>
    T &operator[](unsigned int n)
    {
        return this->m_pData[n];
    }
    
    /// <summary>
    /// + ����������
    /// </summary>  
    /// <remarks>ʹ�������ͨ���±������Ԫ��</remarks>
    /// <param name="n">Ԫ���±꣬���±�Ϊһά�±�</param>
    /// <returns>�������</returns>
    CSpMatrix<T> operator+(CSpMatrix& mt)
    {
        CSpMatrix<T> matrix(this->m_nRow,this->m_nCol);
        for(int i=0;i<this->m_nLength;i++)
            matrix[i]=this->m_pData[i]+mt.m_pData[i];
        return matrix;
    }

    /// <summary>
    /// = ����������
    /// </summary>      
    /// <param name="mt">��ֵ����</param>
    /// <returns>�������</returns>
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
    /// * ����������
    /// </summary>      
    /// <remarks>ʹ�������ͨ��*��������ɾ�����˲��������о�֤���ʱ��ע���������֤���д�С��������ܲ���λ�ô���</remarks>
    /// <param name="mt">��ֵ����</param>
    /// <returns>�������</returns>
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
    /// ����ת�ò���
    /// </summary>      
    /// <remarks>����ǰ����ת��</remarks>   
    /// <returns>�������</returns>
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
    /// ���������
    /// </summary>      
    /// <remarks>��ǰ����Ĺ���������</remarks>   
    /// <returns>�������</returns>
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
                if(m==(m_nRow-1)*factor)  //���һ��
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
    /// �����������
    /// </summary>      
    /// <remarks>��ǰ����������</remarks>   
    /// <returns>�������</returns>
    CSpMatrix<T> Inverse()
    {
        CSpMatrix<T> matrix(this->m_nRow,this->m_nCol);
        T a1,a2;
        for(int i=0;i<m_nRow;i++)
            matrix.Set(i,i,1);                  //��λ����

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
    /// ������б任
    /// </summary>      
    /// <param name="row">�����к�</param>
    /// <returns>�����к�</returns>
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
    /// ������ǰ����ָ����������
    /// </summary>      
    /// <param name="row1">�����к�1</param>
    /// <param name="row2">�����к�2</param>    
    void SwapRow(int row1,int row2)
    {       
        T* data=new T[m_nCol];
        memcpy(data,m_pData+row1*m_nCol,sizeof(T)*m_nCol);
        memcpy(m_pData+row1*m_nCol,m_pData+row2*m_nCol,sizeof(T)*m_nCol);
        memcpy(m_pData+row1*m_nCol,data,sizeof(T)*m_nCol);
        delete[] data;
    }
    



    

    /// <summary>
    /// �������С���˽� 
    /// </summary>      
    /// <remarks>���k*x=y ��x����С���˽�</remarks>    
    /// <returns>x��С���˽����</returns>
    static CSpMatrix<T> Solve(CSpMatrix<T> kmat,CSpMatrix<T> ymat)
    {
        CSpMatrix<T> tmat=kmat.Transpose();
        CSpMatrix<T> ttmat=tmat*kmat;
        CSpMatrix<T> tymat=tmat*ymat;
        CSpMatrix<T> imat=ttmat.Inverse();
        return imat*tymat;
    }

public:
    
    unsigned int m_nRow;                    ///<�����д�С
    
    unsigned int m_nCol;                    ///<�����д�С
    
    unsigned long m_nLength;                ///<�����С
    
    T* m_pData;                             ///<��������
};

