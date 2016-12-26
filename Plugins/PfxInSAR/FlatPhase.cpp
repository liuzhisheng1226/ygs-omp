#include "FlatPhase.h"
#include "../../Share/RMGImage.h"
#include <vector>
#include "../../Share/SpMatrix.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <omp.h>
using namespace std;

#define pi_v 3.14159265359
#define Vl_C 299792458

CFlatPhase::CFlatPhase(void)
{

}
CFlatPhase::~CFlatPhase(void)
{

}

void CFlatPhase::Process()
{
/*    CDlgFlatPhase dlg;*/
    //if(dlg.DoModal()!=IDOK) return;
    /*m_Flat_R2(dlg.FileInputM,dlg.FileInputMH,dlg.FileInputS,dlg.FileInputSH,dlg.FileOutputF,dlg.FileOutputFH,dlg.m_bIsPreorb);*/
}
void CFlatPhase::Batch()
{
/*    string mFile  = m_aryStrs.GetAt(0);*/
    //string mFileH = m_aryStrs.GetAt(1);
    //string sFile  = m_aryStrs.GetAt(2);
    //string sFileH = m_aryStrs.GetAt(3);
    //string mOutFile  = m_aryStrs.GetAt(4);
    //string mOutFileH = m_aryStrs.GetAt(5);
    //int isOrb = atoi(m_aryStrs.GetAt(6));
    /*m_Flat_R2(mFile,mFileH,sFile,sFileH,mOutFile,mOutFileH,isOrb);*/
}

void CFlatPhase::m_Flat_R2(string mFile,string mFileH,string sFile,string sFileH,
             string mOutFile,string mOutFileH,bool isOrb)
{
    int i;
    //读取图像信息
    CRMGImage imageM(mFile,mFileH);
    CRMGImage imageS(sFile,sFileH);
    
    //获取图像头文件信息
    //CRMGHeader imageHeaderMaster=imageM.m_oHeader;
    //CRMGHeader imageHeaderSlave=imageS.m_oHeader;

        //图像轨道拟合系数  
    CSpMatrix<double> xPolyCoef(4,1);                       //----**aef
    CSpMatrix<double> yPolyCoef(4,1);                       //----**bef
    CSpMatrix<double> zPolyCoef(4,1);                       //----**cef

    CSpMatrix<double> xPolyCoef1(4,1);                      //----**aef1
    CSpMatrix<double> yPolyCoef1(4,1);                      //----**bef1
    CSpMatrix<double> zPolyCoef1(4,1);                      //----**cef1


    if(!isOrb)      
    {
        //非精轨数据
        imageM.OrbitCoef(imageM.m_oHeader.StateVector,xPolyCoef,yPolyCoef,zPolyCoef);
        imageS.OrbitCoef(imageS.m_oHeader.StateVector,xPolyCoef1,yPolyCoef1,zPolyCoef1);
    }
    else
    {
        //精轨数据
        imageM.m_oPreOrbit.LoadOrbfile();
        imageS.m_oPreOrbit.LoadOrbfile();
        imageM.PreorbCoef(imageM.m_oPreOrbit.preOrbitPoint,xPolyCoef,yPolyCoef,zPolyCoef);
        imageS.PreorbCoef(imageS.m_oPreOrbit.preOrbitPoint,xPolyCoef1,yPolyCoef1,zPolyCoef1);
    }
    

    //参数进行赋值
   double wl=Vl_C/imageM.m_oHeader.RadarCenterFrequency;
   double prf=imageM.m_oHeader.PulseRepetitionFrequency;        //(imageHeaderMaster.Line-1)/(imageHeaderMaster.ZeroDopplerTimeLastLine.precision-imageHeaderMaster.ZeroDopplerTimeFirstLine.precision); 
   double semia=imageM.m_oHeader.Ellipse.major;   //长半轴
   double semib=imageM.m_oHeader.Ellipse.minor;     //短半轴
   float m_Da=imageM.m_oHeader.SampledLineSpacing;              //azimuth resolution
   float m_Dr=imageM.m_oHeader.SampledPixelSpacing;         //range   resolution

   //用5个卫星采样点去拟合
   //主图像 
   double Azimutht1_m=imageM.m_oHeader.ZeroDopplerTimeFirstLine.precision;  //第一行成像时间
   double Azimutht2_m=imageM.m_oHeader.ZeroDopplerTimeLastLine.precision;   //最后一行成像时间
   double start1_m=isOrb?imageM.m_oPreOrbit.preOrbitPoint.begin()->timePoint.precision:imageM.m_oHeader.StateVector.begin()->timePoint.precision;   //卫星第一个采样点时间


   //辅图像
   double Azimutht1_s = imageS.m_oHeader.ZeroDopplerTimeFirstLine.precision;  //第一行成像时间
   double Azimutht2_s = imageS.m_oHeader.ZeroDopplerTimeLastLine.precision;   //最后一行成像时间
   double start1_s = isOrb?imageS.m_oPreOrbit.preOrbitPoint.begin()->timePoint.precision:imageS.m_oHeader.StateVector.begin()->timePoint.precision; //卫星第一个采样点时间

    //计算最小斜距
    int lFirstClm,lFirstRow;     //主图像在原图像中的位置,近似为粗配准的偏移
    int blockclm,blockrow;      //roi子区在粗配准图像中的位置
    int lWidth,lHeight;        //数据大小

    lWidth = imageM.m_oHeader.Sample;   //宽
    lHeight= imageM.m_oHeader.Line;   //高
    //原整景数据大小
    int oriH = imageM.m_oHeader.LineOri;
    int oriW = imageM.m_oHeader.SampleOri;

    lFirstClm = imageM.m_oHeader.Registration.rangeOffset;
    lFirstRow = imageM.m_oHeader.Registration.azimuthOffst;
    blockclm  = imageM.m_oHeader.leftUpC;
    blockrow  = imageM.m_oHeader.leftUpL;

    float center_lat = imageM.m_oHeader.GeodeticCoordinateCenter.latitude*pi_v/180;         //中间像元的经纬度坐标
    float center_lon = imageM.m_oHeader.GeodeticCoordinateCenter.longitude*pi_v/180;    
    double azimuth_tn_m= imageM.m_oHeader.ZeroDopplerTimeLastLine.precision;        //主图像最后行方位时间
    double azimuth_tn_s= imageS.m_oHeader.ZeroDopplerTimeLastLine.precision;        //辅图像最后行方位时间
    double slt_near_m =  imageM.m_oHeader.SlantRangeTimeToFirstRangeSample*Vl_C/2;          //主图像近斜距   S=V*t  时间是双程的  所以除2  时间单位是毫秒 所以除1000


     //处理数据在原始图像数据中的偏移
    lFirstClm=lFirstClm+blockclm;
    lFirstRow=lFirstRow+blockrow;

    //计算地面中间点的坐标：经纬度——>坐标,作为牛顿迭代的初始值
    double xx,yy,zz;
    imageM.LonLat2Coordinate(xx,yy,zz);

    //打开输出数据文件
    //Open OUTPUT image File
    ofstream file(mOutFile,ios::binary|ios::out );
    if(!file.is_open()){
        cout<<"open output file failed\n";
        exit(1);
    }

    //利用5个插值点计算每行的平地相位
    long box_Width=lWidth;   //当前数据区的数据列
    int step=box_Width/(5-1);  //每个插值点的间隔

    omp_lock_t file_lock;
    omp_init_lock(&file_lock);

    //开始计算
#pragma omp parallel for
    for(long row=0;row<lHeight;row++)
    {
        float *phase=new float[lWidth];
        memset(phase,0,sizeof(float)*lWidth);

        double interp_position[5]={0,(double)step,(double)2*step,(double)3*step,(double)box_Width-1};   //插值点位置
        double interp_phase[5];   //插值点相位

        double *phase_temp=new double [box_Width];    
        double *phase_position=new double [box_Width];    //记录每个点的位置
        for(int ii=0;ii<box_Width;ii++) 
        {
            phase_position[ii]=ii;
        }

        double t0;
        //计算平地相位
        double R1;      //主图像斜距
        double R2;      //辅图像斜距
        double satXm,satYm,satZm,satXs,satYs,satZs;
        double x0=xx; double y0=yy; double z0=zz;
        //进度条
        if(!imageM.m_oHeader.PassDirection)
        {//ASCEND
            t0 = Azimutht2_m + (oriH-lFirstRow-row)/prf-start1_m;
        }
        else
        {//DESCNED
            t0 = Azimutht1_m+(lFirstRow+row)/prf-start1_m;
        }
        //double t0=Azimutht1_m+(lFirstRow+row)/prf-start1_m;     //卫星成像时对应的轨道拟合时间
        double t_poi=t0;        //在后面的迭代中用到
        
        // 主图像卫星坐标
        satXm = imageM.Polyfit(xPolyCoef,t0,0);
        satYm = imageM.Polyfit(yPolyCoef,t0,0);
        satZm = imageM.Polyfit(zPolyCoef,t0,0);
        /*x10=Polyfit(xPolyCoef,t0,0);
        y10=Polyfit(yPolyCoef,t0,0);
        z10=Polyfit(zPolyCoef,t0,0);*/

        //迭代计算地面点坐标
        R1 = slt_near_m+lFirstClm*m_Dr;
        imageM.Newton(t_poi,semia,semib,xPolyCoef,yPolyCoef,zPolyCoef,slt_near_m,x0,y0,z0,30);//x0,y0,z0为地面坐标

        //计算辅图像卫星轨迹对应点的直角坐标(x2,y2,z2)
        t0 = imageS.getSatposT(x0,y0,z0,xPolyCoef1,yPolyCoef1,zPolyCoef1,isOrb);
        //确定辅图像卫星坐标
        satXs = imageS.Polyfit(xPolyCoef1,t0,0);
        satYs = imageS.Polyfit(yPolyCoef1,t0,0);
        satZs = imageS.Polyfit(zPolyCoef1,t0,0);

        //先计算五个差值点的相位
        for(int ii=0;ii<5;ii++)
        {
            R1=slt_near_m+lFirstClm*m_Dr+interp_position[ii]*m_Dr;  //主图像斜距
            //方法Newton（）用斜距求出此时图像中的点的地面坐标
            imageM.Newton(t_poi,semia,semib,xPolyCoef,yPolyCoef,zPolyCoef,R1,x0,y0,z0,30);
            //辅助图像斜距
            R2=sqrt((satXs-x0)*(satXs-x0)+(satYs-y0)*(satYs-y0)+(satZs-z0)*(satZs-z0));
            //
            interp_phase[ii] = 2*pi_v*2*(R2-R1)/wl;     //主相位
        }

        //插值拟合
        pwcint(interp_position,interp_phase,5,phase_position,phase_temp,box_Width);

        //缠绕
        for(int ii=0;ii<box_Width;ii++)
        {
            phase[ii]=phase_temp[ii] - floor(phase_temp[ii]/(2*pi_v))*2*pi_v;
            if(phase[ii] > pi_v) phase[ii] -=2*pi_v;
        }
        omp_set_lock(&file_lock);
        file.seekp((streampos)row*sizeof(float)*lWidth, ios::beg);
        file.write((char *)phase,sizeof(float)*lWidth);
        omp_unset_lock(&file_lock);

        delete[] phase;
        delete[] phase_temp;
        delete[] phase_position;
    } // for row
    omp_destroy_lock(&file_lock);

    file.close();

    //编辑头文件
    CRMGHeader header(imageM.m_oHeader);                        //复制主图像头文件信息
    header.DataType = eFLOAT32;
    header.Save(mOutFileH);
}



//  插值计算相位
void CFlatPhase::pwcint(double *x, double *y, long m, double *xresult, double *yresult, long n)
{
    long i;

    //求出分段的线性导数
    double*del=new double[m-1];     //del----- (m-1) 维
    for(i=0;i<m-1;i++)
    {
        del[i]=(y[i+1]-y[i])/(x[i+1]-x[i]);     
    }
    //首先求每一点的导数derive，保持插值点的单调性
    double *deriv=new double[m];    //用来保持计算的一阶导数值  deriv－－－－－ m 维
    pwcslopes(x,y,m,del,deriv);     //计算一阶导数值

    //接着计算xresult对应的值，xresult 是单调增加的
    long k;     //标志变量
    k=0;
    for(i=0;i<n;i++)
    {
        if(xresult[i]<=x[0])        //在第一个插值点的左边
        {
            double xx=(xresult[i]-x[0])*(xresult[i]-x[0]);
            double F=(y[1]-y[0])/(x[1]-x[0]);
            double Fi1=(F-deriv[0])/(x[1]-x[0]);
            double Fi=(deriv[1]-2*F+deriv[0])/(x[1]-x[0])/(x[1]-x[0]);
            yresult[i]=y[0]+(xresult[i]-x[0])*deriv[0]+xx*Fi1+xx*(xresult[i]-x[1])*Fi;
        }
        else if(xresult[i]<=x[m-1])     //1-m个插值点之间
        {
            while(xresult[i]>x[k])
            {
                k=k+1;
            }
            double xx=(xresult[i]-x[k-1])*(xresult[i]-x[k-1]);
            double F=(y[k]-y[k-1])/(x[k]-x[k-1]);
            double Fi1=(F-deriv[k-1])/(x[k]-x[k-1]);
            double Fi=(deriv[k]-2*F+deriv[k-1])/(x[k]-x[k-1])/(x[k]-x[k-1]);
            yresult[i]=y[k-1]+(xresult[i]-x[k-1])*deriv[k-1]+xx*Fi1+xx*(xresult[i]-x[k])*Fi;
        }
        else        //最后一个点之外
        {
            double xx=(xresult[i]-x[m-2])*(xresult[i]-x[m-2]);
            double F=(y[m-1]-y[m-2])/(x[m-1]-x[m-2]);
            double Fi1=(F-deriv[m-2])/(x[m-1]-x[m-2]);
            double Fi=(deriv[m-1]-2*F+deriv[m-2])/(x[m-1]-x[m-2])/(x[m-1]-x[m-2]);
            yresult[i]=y[m-2]+(xresult[i]-x[m-2])*deriv[m-2]+xx*Fi1+xx*(xresult[i]-x[m-1])*Fi;
        }
    }
    //释放缓存
    delete[] del;
    delete[] deriv;
    return;
}

//求一阶导数
void CFlatPhase::pwcslopes(double*x,double*y,long m,double*del,double*deriv)
{
    //将deriv设置为0
    memset(deriv,0,sizeof(double)*m);
    //如果只有两个点
    if(m==2)
    {
        deriv[1]=del[1];
        deriv[2]=del[1];
        return;
    }

    long i;     //循环变量
    long k;     //临时变量
    double*h=new double[m-1];
    //插值点的x间隔
    for(i=0;i<m-1;i++)
    {
        h[i]=x[i+1]-x[i];
    }
    //中间点的导数值
    for(i=0;i<m-2;i++)
    {
        if(del[i]*del[i+1]>0)
        {
            k=i;
            double hs=h[k]+h[k+1];
            double w1=(h[k]+hs)/(3*hs);
            double w2=(hs+h[k+1])/(3*hs);
            double dmax=fabs(del[k]);
            double dmin=fabs(del[k+1]);
            if(fabs(del[k+1])>fabs(del[k]))
            {
                dmax=fabs(del[k+1]);
                dmin=fabs(del[k]);
            }
            deriv[k+1]=dmin/(w1*(del[k]/dmax)+w2*(del[k+1]/dmax));
        }
    }
    //端点的导数deriv[0],deriv[m-1];
    deriv[0]=((2*h[0]+h[1])*del[0]-h[0]*del[1])/(h[0]+h[1]);
    if(deriv[0]*del[0]<0)
    {
        deriv[0]=0;
    }
    deriv[m-1]=((2*h[m-2]+h[m-3])*del[m-2]-h[m-2]*del[m-3])/(h[m-2]+h[m-3]);
    if(deriv[m-1]*del[m-2]<0)
    {
        deriv[m-1]=0;
    }
    //释放缓存
    delete[] h;
}

