#include "Lsqr.h"
#include <math.h>

CLsqr::CLsqr(void)
{
}

void CLsqr::method(float **MM,float *NN,float X[])
{
    //赋值
    A=MM;
    trans(A,B,M,N);
    D=NN;
    S=D;

    int  i,j,k,n=0;
    float a,b,c,d,e,f,g,h;

 for(j=0;j<N;j++)
  {
    a=0;
   for(i=0;i<M;i++)
    {
     a=a+B[j][i]*D[i];
    }
    R[j] = a;
    P[j] = R[j];
    X[j] = 0;    /*解向量X赋初值为0*/
  }/*赋迭代初值*/

 do 
  {
    k=0;
    c=0;
    d=0;
     for(i=0;i<M;i++)
     {
       b=0;
       for(j=0;j<N;j++)
        {
           b=b+A[i][j]*P[j];
           
        }
        W[i] = b;
        d =(float) (d+pow(W[i],2));
      } 
     for(j=0;j<N;j++)
     {
      c=(float)(c+pow(R[j],2));
     }
 
     e=(float)(c*pow(d,-1));
 
     for(j=0;j<N;j++)
     {
       X[j] = X[j]+e*P[j];
       if(fabs(e*P[j])>=0.00001) k=1; 
     }
     for(i=0;i<M;i++)
     {
      S[i]=S[i]-e*W[i];
     }
     
     g=0;
     for(j=0;j<N;j++)
     {
       f=0;
       for(i=0;i<M;i++)
       {
         
         f = f+B[j][i]*S[i];
       }
       R[j] = f;
       g=(float)(g+pow(R[j],2));
     }
     h=(float)(g*pow(c,-1)); 
     for(j=0;j<N;j++)
     {
       P[j] = R[j]+h*P[j];  
     }n++;
  }while(k);

}

void CLsqr::trans(float **a,float **b,int m,int n)
{
    for(int i=0;i<m;i++)
    {
        for(int j=0;j<n;j++)
            b[j][i]=a[i][j];
    }
}

CLsqr::~CLsqr(void)
{
    //for(int i=0;i<M;i++)
    //  delete A[i];
    //delete A;

    //for(int i=0;i<N;i++)
    //  delete B[i];
 //   delete B;

    //delete D;
    //delete S;delete W;delete R;delete P;
}
