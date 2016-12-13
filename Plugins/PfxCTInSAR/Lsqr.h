#pragma once

class CLsqr
{
public:
   int M;
   int N;

   float **A,**B,*D;

   float *S,*W,*R,*P;
	CLsqr(void);
	CLsqr(int m,int n):M(m),N(n){
	
		A=new float *[M];
		for(int i=0;i<M;i++)
			A[i]=new float[N];

		B=new float *[N];
		for(int i=0;i<N;i++)
			B[i]=new float[M];

		D=new float[M];

		S=new float[M];
		W=new float[M];
		R=new float[N];
		P=new float[N];

	};
	 
	void method(float **a,float *b,float X[]);
	void trans(float **a,float **b,int m,int n);
	~CLsqr(void);
};

