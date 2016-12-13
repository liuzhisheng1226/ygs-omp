// #include "StdAfx.h"
#include "RMGImage.h"


CRMGImage::CRMGImage(string lpFullname)
{   
    this->m_lpFullname=lpFullname;  
    this->m_oHeader.Load(this->GetHeadername());
}

CRMGImage::CRMGImage(string lpDataFullname,string lpHdrFullname)
{
    this->m_lpFullname=lpDataFullname;
    //printf("image contruct complete1\n");
    if(true != this->m_oHeader.Load(lpHdrFullname))
    {
        fprintf(stderr,"load ini file failed\n");
        exit(-1);

    }
    //printf("image contruct complete2\n");
    if(true != this->m_oPreOrbit.CRMGHeader::Load(lpHdrFullname))
    {
        fprintf(stderr,"preorbit load ini file failed\n");
        exit(-1);
    }
    //printf("image contruct complete3\n");
    this->m_oPreOrbit.m_fileName = this->GetPreOrbitname();
    //printf("image contruct complete4\n");
}

CRMGImage::~CRMGImage(void)
{


}

//中心点经纬度转换为x,y,z坐标
void CRMGImage::LonLat2Coordinate(double &x,double &y,double &z)
{
    double f,c,s;
    f=1-m_oHeader.Ellipse.minor/m_oHeader.Ellipse.major;
    c=1/sqrt(1+f*(f-2)*sin(m_oHeader.GeodeticCoordinateCenter.latitude*CONST_MATH_PI/180)*sin(m_oHeader.GeodeticCoordinateCenter.latitude*CONST_MATH_PI/180));
    s=(1-f)*(1-f)*c;
    x=(m_oHeader.Ellipse.major*c)*cos(m_oHeader.GeodeticCoordinateCenter.latitude*CONST_MATH_PI/180)*cos(m_oHeader.GeodeticCoordinateCenter.longitude*CONST_MATH_PI/180);
    y=(m_oHeader.Ellipse.major*c)*cos(m_oHeader.GeodeticCoordinateCenter.latitude*CONST_MATH_PI/180)*sin(m_oHeader.GeodeticCoordinateCenter.longitude*CONST_MATH_PI/180);
    z=(m_oHeader.Ellipse.major*s)*sin(m_oHeader.GeodeticCoordinateCenter.latitude*CONST_MATH_PI/180);
}

void CRMGImage::OrbitCoef(vector<CRMGHeader::StructStateVector> stateVector,CSpMatrix<double> &xcoef,CSpMatrix<double> &ycoef,CSpMatrix<double> &zcoef)
{
    //CONST_ORBIT_SAPMLING_POINT_COUNT
    int orbit_sampling_point_count =stateVector.size();
    double *timeSampling=new double[orbit_sampling_point_count];                        //每个采样点时间
    for(int i=0;i<orbit_sampling_point_count;i++)
        timeSampling[i]=stateVector[i].timePoint-stateVector[0].timePoint;

    CSpMatrix<double> lmtx(2*orbit_sampling_point_count,4);
    CSpMatrix<double> xmtx(2*orbit_sampling_point_count,1);
    CSpMatrix<double> ymtx(2*orbit_sampling_point_count,1);
    CSpMatrix<double> zmtx(2*orbit_sampling_point_count,1);

    for(int i=0;i<orbit_sampling_point_count;i++)
    {       
        lmtx.Set(i,0,1);
        lmtx.Set(i,1,timeSampling[i]);
        lmtx.Set(i,2,timeSampling[i]*timeSampling[i]);
        lmtx.Set(i,3,timeSampling[i]*timeSampling[i]*timeSampling[i]);
        lmtx.Set(i+orbit_sampling_point_count,0,0);
        lmtx.Set(i+orbit_sampling_point_count,1,1);
        lmtx.Set(i+orbit_sampling_point_count,2,2*timeSampling[i]);
        lmtx.Set(i+orbit_sampling_point_count,3,3*timeSampling[i]*timeSampling[i]);

        xmtx.Set(i,0,stateVector[i].xPosition);
        xmtx.Set(i+orbit_sampling_point_count,0,stateVector[i].xVelocity);

        ymtx.Set(i,0,stateVector[i].yPosition);
        ymtx.Set(i+orbit_sampling_point_count,0,stateVector[i].yVelocity);

        zmtx.Set(i,0,stateVector[i].zPosition);
        zmtx.Set(i+orbit_sampling_point_count,0,stateVector[i].zVelocity);
    }//for int i;

    CSpMatrix<double> ltrmtx=lmtx.Transpose();                          //-----a2;
    CSpMatrix<double> lmtmtx=ltrmtx*lmtx;                               //-----a1;
    lmtmtx=lmtmtx.Inverse();
    CSpMatrix<double> lfmtx=lmtmtx*ltrmtx;
    //CSpMatrix<double> linvmtx=lmtmtx.Inverse();
    //CSpMatrix<double> lfmtx=linvmtx*ltrmtx;
    
    xcoef=lfmtx*xmtx;
    ycoef=lfmtx*ymtx;
    zcoef=lfmtx*zmtx;
}

/***********************************************************
*函数名称：getSatposT
*函数功能：在获得卫星轨道拟合参数的基础上，计算对（x,y,z）成像时卫星的像对时刻（相对第一卫星位置时刻）
*
*
*
******************************************************************/

double CRMGImage::getSatposT(double x, double y,double z,CSpMatrix<double> &xPolyCoef,CSpMatrix<double> &yPolyCoef,CSpMatrix<double> &zPolyCoef,bool isPreorb)
{
    double timeSpan=isPreorb?m_oPreOrbit.m_fIntervalT:m_oHeader.StateVector[1].timePoint-m_oHeader.StateVector[0].timePoint;
    double ft1= 0;
    //double ft2=(image.CONST_ORBIT_SAPMLING_POINT_COUNT-1)*timeSpan+5; //时间间隔
    double ft2=isPreorb?(m_oPreOrbit.preOrbitPoint.size()-1)*timeSpan+3:(m_oHeader.StateVector.size()-1)*timeSpan+5;    //时间间隔
    double ft3=(ft1+ft2)/2; 
    
    while(fabs(ft1-ft2)>1e-6)
    {           
        //计算多项式值
        double p1=Polynomial(xPolyCoef,yPolyCoef,zPolyCoef,ft1,x,y,z);
        double p2=Polynomial(xPolyCoef,yPolyCoef,zPolyCoef,ft2,x,y,z);
        double p3=Polynomial(xPolyCoef,yPolyCoef,zPolyCoef,ft3,x,y,z);

        if(p1*p3>=0&&p2*p3<=0) {ft1=ft3;ft3=(ft1+ft2)/2;}
        else if(p1*p3<=0&&p2*p3>=0)  {ft2=ft3;ft3=(ft1+ft2)/2;}
        else 
        {
            fprintf(stderr,"Polyfit Fail!");
            exit(0);
        }
    }//while fabs
    return ft3;
    //double timeAzimuth=ft3;   
}

#pragma region Poly多项式拟合
double CRMGImage::Polynomial(CSpMatrix<double> &aef,CSpMatrix<double> &bef,CSpMatrix<double> &cef,double t,double x,double y,double z)
{   
    return  (Polyfit(aef,t,0)-x)*Polyfit(aef,t,1)+                  //(X(t)-x0)*Vx(t)
        (Polyfit(bef,t,0)-y)*Polyfit(bef,t,1)+                  //(Y(t)-y0)*Vy(t)
        (Polyfit(cef,t,0)-z)*Polyfit(cef,t,1);                  //(Z(t)-z0)*Vz(t)
}
double CRMGImage::Polyfit(CSpMatrix<double> &mat,double t,int order)
{
    double value=0,item=0,coefficient=1;
    if(order==0) 
        value=mat.Get(0,0)+mat.Get(1,0)*t+mat.Get(2,0)*t*t+mat.Get(3,0)*t*t*t;
    else if(order==1) 
        value=mat.Get(1,0)+ mat.Get(2,0)*t*2 + mat.Get(3,0)*t*t*3;  
    return value;
}
#pragma endregion


/******************************************************
*Fun Name: PreorbCoef
*Fun :根据插值精轨数据进行轨道拟合
*
*
*
*******************************************************/
void CRMGImage::PreorbCoef(vector<CRMGHeader::StructStateVector> stateVector,CSpMatrix<double> &xcoef,CSpMatrix<double> &ycoef,CSpMatrix<double> &zcoef)
{
    //CONST_ORBIT_SAPMLING_POINT_COUNT
    int orbit_sampling_point_count =stateVector.size();
    double *timeSampling=new double[orbit_sampling_point_count];                        //每个采样点时间
    for(int i=0;i<orbit_sampling_point_count;i++)
        timeSampling[i]=i*(this->m_oPreOrbit.m_fIntervalT);

    CSpMatrix<double> lmtx(orbit_sampling_point_count,4);
    CSpMatrix<double> xmtx(orbit_sampling_point_count,1);
    CSpMatrix<double> ymtx(orbit_sampling_point_count,1);
    CSpMatrix<double> zmtx(orbit_sampling_point_count,1);

    for(int i=0;i<orbit_sampling_point_count;i++)
    {       
        lmtx.Set(i,0,1);
        lmtx.Set(i,1,timeSampling[i]);
        lmtx.Set(i,2,timeSampling[i]*timeSampling[i]);
        lmtx.Set(i,3,timeSampling[i]*timeSampling[i]*timeSampling[i]);
        
        xmtx.Set(i,0,stateVector[i].xPosition);
        ymtx.Set(i,0,stateVector[i].yPosition);
        zmtx.Set(i,0,stateVector[i].zPosition);
        
    }//for int i;

    CSpMatrix<double> ltrmtx=lmtx.Transpose();                          //-----a2;
    CSpMatrix<double> lmtmtx=ltrmtx*lmtx;                               //-----a1;
    lmtmtx=lmtmtx.Inverse();
    CSpMatrix<double> lfmtx=lmtmtx*ltrmtx;
    //CSpMatrix<double> linvmtx=lmtmtx.Inverse();
    //CSpMatrix<double> lfmtx=linvmtx*ltrmtx;

    xcoef=lfmtx*xmtx;
    ycoef=lfmtx*ymtx;
    zcoef=lfmtx*zmtx;
}


/******************************************************
*Fun Name: Newton
*Fun :根据牛顿差值计算椭球体上的点坐标
*
*
*
*******************************************************/
void CRMGImage::Newton(double t,double semia, double semib,CSpMatrix<double> xPolyCoef,CSpMatrix<double> yPolyCoef,CSpMatrix<double> zPolyCoef,
            double R,double &x, double &y, double &z,int num)
{
      int i,j;


    //定义二维矩阵
    CSpMatrix<double> AA(3,3);
    CSpMatrix<double> BB(3,1);
    CSpMatrix<double> CC(3,1);

    
    //误差控制
    double errf,errx;
    double Xt = xPolyCoef.Get(0,0) +xPolyCoef.Get(1,0)*t +xPolyCoef.Get(2,0)*t*t +xPolyCoef.Get(3,0)*t*t*t;
    double Yt = yPolyCoef.Get(0,0) +yPolyCoef.Get(1,0)*t +yPolyCoef.Get(2,0)*t*t +yPolyCoef.Get(3,0)*t*t*t;
    double Zt = zPolyCoef.Get(0,0) +zPolyCoef.Get(1,0)*t +zPolyCoef.Get(2,0)*t*t +zPolyCoef.Get(3,0)*t*t*t;
    double Vx = xPolyCoef.Get(1,0) +2*xPolyCoef.Get(2,0)*t +3*xPolyCoef.Get(3,0)*t*t;
    double Vy = yPolyCoef.Get(1,0) +2*yPolyCoef.Get(2,0)*t +3*yPolyCoef.Get(3,0)*t*t;
    double Vz = zPolyCoef.Get(1,0) +2*zPolyCoef.Get(2,0)*t +3*zPolyCoef.Get(3,0)*t*t;
    
    //迭代
    for(i=0;i<num;i++)      //num为最大迭代次数
    {
        //给方程组系数赋值
        AA.Set(0,0,2*x/semia/semia);    AA.Set(0,1,2*y/semia/semia);    AA.Set(0,2,2*z/semib/semib);
        AA.Set(1,0,Vx);             AA.Set(1,1,Vy);             AA.Set(1,2,Vz);
        AA.Set(2,0,2*(x-Xt));           AA.Set(2,1,2*(y-Yt));           AA.Set(2,2,2*(z-Zt));
        //
        BB.Set(0,0,x*x/semia/semia+y*y/semia/semia+z*z/semib/semib-1);
        BB.Set(1,0,(x-Xt)*Vx+(y-Yt)*Vy+(z-Zt)*Vz);
        BB.Set(2,0,(x-Xt)*(x-Xt)+(y-Yt)*(y-Yt)+(z-Zt)*(z-Zt)-R*R);
        
        //函数误差计算及判断退出
        errf=0;
        for(int j=0;j<3;j++)
        {
            errf=errf+fabs(BB.Get(j,0));
        }
        if(errf<1e-6)
            break;
        
        //求解
        //Inverse(A,3); //A的逆矩阵，保存在A中
        //CMatrix m_Matrix;
        CSpMatrix<double> AAI=AA.Inverse();  //求AA的逆
        //m_Matrix.Matrix_Inv(A,3);
        CC=AAI*BB;
        //m_Matrix.Multiply(A,B,3,3,1,C);
        //更新未知数
        x=x-CC.Get(0,0);    y=y-CC.Get(1,0);    z=z-CC.Get(2,0);
        
        //自变量误差计算及判断
        errx=0;
        for(int j=0;j<3;j++)
        {
            errx=errx+fabs(CC.Get(j,0));
        }
        if(errx<1e-6)
            break;
    }

    return;
}
