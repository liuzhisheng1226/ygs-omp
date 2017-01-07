#include "Baseline.h"
#include <fstream>
#include <iostream>
#include <cmath>
#include <omp.h>

using namespace std;

CBaseline::CBaseline(void)
{
}

void CBaseline::Batch()
{
/*    string mFileInpath = m_aryStrs.GetAt(0);*/
    //string sFileInpath = m_aryStrs.GetAt(1);
    //int bCheckPreOrb = atoi(m_aryStrs.GetAt(2));
    //string mOrbPath,sOrbPath,baselinePath,anglePath,rangePath;
    //int azmLooks,rngLooks;
    //if(bCheckPreOrb)
    //{
        //mOrbPath = m_aryStrs.GetAt(3);
        //sOrbPath = m_aryStrs.GetAt(4);
        //azmLooks = atoi(m_aryStrs.GetAt(5));
        //rngLooks = atoi(m_aryStrs.GetAt(6));
        //baselinePath = m_aryStrs.GetAt(7);
        //anglePath = m_aryStrs.GetAt(8);
        //rangePath = m_aryStrs.GetAt(9);
    //}
    //else
    //{
        //mOrbPath = "";      //m_aryStrs.GetAt(3);
        //sOrbPath = "";      //m_aryStrs.GetAt(4);
        //azmLooks = atoi(m_aryStrs.GetAt(3));
        //rngLooks = atoi(m_aryStrs.GetAt(4));
        //baselinePath = m_aryStrs.GetAt(5);
        //anglePath = m_aryStrs.GetAt(6);
        //rangePath = m_aryStrs.GetAt(7);
    //}
    //BaselineInangleSlate(mFileInpath,sFileInpath,
                        //bCheckPreOrb,mOrbPath,sOrbPath,
                        //azmLooks,rngLooks,
                        /*baselinePath,anglePath,rangePath);*/
}
void CBaseline::Process()
{
/*    CDlgBaseline dlg;*/
    //if(dlg.DoModal()!=IDOK) return;

    //BaselineInangleSlate(dlg.m_MFileIn,dlg.m_SFileIn,dlg.m_CheckPreOrb,dlg.m_MOrb,dlg.m_SOrb,dlg.m_AzmLooks,dlg.m_RanLooks,
        /*dlg.m_BaselienFileOut,dlg.m_InAngleFileOut,dlg.m_SlateRangeFileOut); */
}
void CBaseline::BaselineInangleSlate(string MFileIn,string SFileIn,bool CheckPreOrt,string MFineOrt,string SFineOrt,int AziLooks,int RanLooks,
        string BaselineOut,string InAngleOut,string SlateOut)
{
        //flag=true,表示主图像入射角大于辅图像入射角;flag=false,表示主图像入射角小于辅图像入射角
        bool flag=true;   
        double baseline_norm_temp=0;
        AnalyseGeometry(MFileIn,SFileIn,baseline_norm_temp,flag);  //计算垂直基线，并分析主辅图的几何关系

        string MFileInH=MFileIn;
        string SFileInH=SFileIn;
        MFileInH.replace(MFileInH.find("rmg"), 3, "ldr");
        SFileInH.replace(SFileInH.find("rmg"), 3, "ldr");
        CRMGImage mImg(MFileIn,MFileInH);   //主图像
        CRMGImage sImg(SFileIn,SFileInH);   //辅图像

        //图像轨道拟合系数  
        CSpMatrix<double> xPolyCoef(4,1);                       //----**aef
        CSpMatrix<double> yPolyCoef(4,1);                       //----**bef
        CSpMatrix<double> zPolyCoef(4,1);                       //----**cef
        mImg.OrbitCoef(mImg.m_oHeader.StateVector,xPolyCoef,yPolyCoef,zPolyCoef);

        CSpMatrix<double> xPolyCoef1(4,1);                      //----**aef
        CSpMatrix<double> yPolyCoef1(4,1);                      //----**bef
        CSpMatrix<double> zPolyCoef1(4,1);                      //----**cef
        sImg.OrbitCoef(sImg.m_oHeader.StateVector,xPolyCoef1,yPolyCoef1,zPolyCoef1);

        //定义参数
        double wl=mImg.CONST_MATH_LIGHT_VELOCITY/mImg.m_oHeader.RadarCenterFrequency;
        double prf=mImg.m_oHeader.PulseRepetitionFrequency;
        double semia=mImg.m_oHeader.Ellipse.major;
        double semib=mImg.m_oHeader.Ellipse.minor;
        double m_Da=mImg.m_oHeader.SampledLineSpacing;
        double m_Dr=mImg.m_oHeader.SampledPixelSpacing;
        double interval=mImg.m_oHeader.StateVector[2].timePoint.precision-mImg.m_oHeader.StateVector[1].timePoint.precision;  //卫星采样点间隔
        bool isAscend = mImg.m_oHeader.PassDirection;

        double azimuth_t1_m=mImg.m_oHeader.ZeroDopplerTimeFirstLine.precision;  //第一行成像时间
        double sensor_t0_m=mImg.m_oHeader.StateVector[0].timePoint.precision;   //卫星第一个采样点时间

        double azimuth_t1_s=sImg.m_oHeader.ZeroDopplerTimeFirstLine.precision;
        double sensor_t0_s=sImg.m_oHeader.StateVector[0].timePoint.precision;

        if(CheckPreOrt)
        {
            mImg.m_oPreOrbit.LoadOrbfile();
            sImg.m_oPreOrbit.LoadOrbfile();
            
            //拟合精轨数据
            mImg.PreorbCoef(mImg.m_oPreOrbit.preOrbitPoint,xPolyCoef,yPolyCoef,zPolyCoef);
            sImg.PreorbCoef(sImg.m_oPreOrbit.preOrbitPoint,xPolyCoef1,yPolyCoef1,zPolyCoef1);

            sensor_t0_m= mImg.m_oPreOrbit.preOrbitPoint[0].timePoint.precision;
            sensor_t0_s= sImg.m_oPreOrbit.preOrbitPoint[0].timePoint.precision;  //更新第一卫星采样点时间

            interval = mImg.m_oPreOrbit.m_fIntervalT;
        }

        //计算斜距
        int lFirstClm,lFirstRow;    //主图像在原图像中的位置
        int blockclm,blockrow;      //子区在粗配准图像中的位置
        int lWidth,lHeight;         //数据大小

        lWidth=mImg.m_oHeader.Sample;
        lHeight=mImg.m_oHeader.Line;

        lFirstClm=mImg.m_oHeader.Registration.rangeOffset;
        lFirstRow=mImg.m_oHeader.Registration.azimuthOffst;
         
        blockclm=mImg.m_oHeader.leftUpC;
        blockrow=mImg.m_oHeader.leftUpL;

        lFirstClm=lFirstClm+blockclm;   //总的起始像元行
        lFirstRow=lFirstRow+blockrow;   //总的起始像元列

        int originPixel,originLine;     //图像初始大小
        originPixel=mImg.m_oHeader.SampleOri;
        originLine=mImg.m_oHeader.LineOri;

        double azimuth_tn_m=mImg.m_oHeader.ZeroDopplerTimeLastLine.precision;       //主图像最后行方位时间
        double azimuth_tn_s=sImg.m_oHeader.ZeroDopplerTimeLastLine.precision;       //辅图像最后行方位时间

        double time_near_m=mImg.m_oHeader.SlantRangeTimeToFirstRangeSample;     //主图像近斜距时间
        double slt_near_m=time_near_m*mImg.CONST_MATH_LIGHT_VELOCITY/2;     //主图像近斜距

        double f=1-mImg.m_oHeader.Ellipse.minor/mImg.m_oHeader.Ellipse.major;       //椭球体扁率
        double e=2*f-f*f;           //偏心率
        //double c;

        //利用中心经纬度计算地面三维坐标  x0,y0,z0 为地面坐标
        double x0=0,y0=0,z0=0;    
        mImg.LonLat2Coordinate(x0,y0,z0);

        //计算 
        double R1;     //主图像斜距
        double R2;     //辅图像斜距

        //打开文件
        ofstream file1 (SlateOut, ios::out);
        if (!file1.is_open()) {
            cout << "error open file\n";
            return;
        }
        ofstream file2 (InAngleOut, ios::out);
        if (!file2.is_open()) {
            cout << "error open file\n";
            return;
        }
        ofstream file3 (BaselineOut, ios::out);
        if (!file3.is_open()) {
            cout << "error open file\n";
            return;
        }

        long box_Width=lWidth;
        int step=box_Width/(5-1);   //每个点之间的近似间隔
        double interp_position[5]={0,(double)step,2*(double)step,3*(double)step,(double)box_Width-1};   //插值点位置
        double interp_slt[5];       //插值点处的斜距
        double interp_inc[5];       //插值点处的入射角
        double interp_base[5];      //插值点处的基线据

        //分配缓存
        double *slt_temp=new double[box_Width];
        double *inc_temp=new double[box_Width];
        double *base_temp=new double[box_Width];
        double *position=new double [box_Width];
        for(int ii=0;ii<box_Width;ii++)
        {
            position[ii]=ii;
        }

        //多视处理的大小
        int w=lWidth/RanLooks;
        int h=lHeight/AziLooks;

        //为输出数据分配空间
        float *out_bas=new float[w];
        float *out_inc=new float[w];
        double *out_slt=new double[w];
        memset(out_bas,0,sizeof(float)*w);
        memset(out_inc,0,sizeof(float)*w);
        memset(out_slt,0,sizeof(double)*w);

        int numrow=0;
        int numclm=0;
        //主辅卫星的三维坐标
        double satXm,satYm,satZm,satXs,satYs,satZs;
        //按行处理 
        for(long row=0;row<h*AziLooks;row++)
        {
            numclm++;
            //计算子区的第一点对应的轨道坐标，并计算对应的三维坐标      
            double t0 = isAscend?(azimuth_t1_m+(lFirstRow+row)/prf-sensor_t0_m):(azimuth_tn_m+(originLine-lFirstRow-row)/prf-sensor_t0_m);     //成像时间
            double t_poi=t0;

            //轨道坐标
            satXm=mImg.Polyfit(xPolyCoef,t0,0);
            satYm=mImg.Polyfit(yPolyCoef,t0,0);
            satZm=mImg.Polyfit(zPolyCoef,t0,0);

            //地面坐标
            mImg.Newton(t0,semia,semib,xPolyCoef,yPolyCoef,zPolyCoef,slt_near_m,x0,y0,z0,20);   //x0,y0,z0 为地面坐标

            //辅图像
            t0 = sImg.getSatposT(x0,y0,z0,xPolyCoef1,yPolyCoef1,zPolyCoef1,CheckPreOrt);
            satXs = sImg.Polyfit(xPolyCoef1,t0,0);
            satYs = sImg.Polyfit(yPolyCoef1,t0,0);
            satZs = sImg.Polyfit(zPolyCoef1,t0,0);

            //基线距
            double baseline=sqrt((satXm-satXs)*(satXm-satXs)+
                                 (satYm-satYs)*(satYm-satYs)+
                                 (satZm-satZs)*(satZm-satZs));

            //定义变量 
            double Rs;
            double Rp;
            double angle;   //三角形中，垂直基线对应的锐角
            double Baseline_norm;           //垂直基线距

            //先计算5个采样点数据
            for(int ii=0;ii<5;ii++)
            {
                R1=slt_near_m+lFirstClm*m_Dr+interp_position[ii]*m_Dr;  //主图像斜距
                mImg.Newton(t_poi,semia,semib,xPolyCoef,yPolyCoef,zPolyCoef,R1,x0,y0,z0,10);
                R2=sqrt((satXs-x0)*(satXs-x0)+(satYs-y0)*(satYs-y0)+(satZs-z0)*(satZs-z0));

                Rs=sqrt(satXm*satXm+satYm*satYm+satZm*satZm);
                Rp=sqrt(x0*x0+y0*y0+z0*z0);

                interp_slt[ii]=R1;
                interp_inc[ii]=acos((R1*R1+Rs*Rs-Rp*Rp)/(2*R1*Rs));

                angle=acos((R1*R1+baseline*baseline-R2*R2)/(2*R1*baseline));    //计算角度          
                if(flag==true)
                    Baseline_norm = baseline * sin(angle);
                else 
                    Baseline_norm = -baseline * sin(angle);

                //垂直基线距
                interp_base[ii]=Baseline_norm;
            }

            //差值到一行数据上
            pwcint(interp_position,interp_slt,5,position,slt_temp,box_Width);
            pwcint(interp_position,interp_inc,5,position,inc_temp,box_Width);
            pwcint(interp_position,interp_base,5,position,base_temp,box_Width);

            //多视处理的结果
            for(int j=0;j<w*RanLooks;j++)
            {
                out_bas[j/RanLooks] += base_temp[j];
                out_inc[j/RanLooks] += inc_temp[j];
                out_slt[j/RanLooks] += slt_temp[j];
            }
            //方位多视处理，并输出
            if(numclm==AziLooks)
            {
                for(int j=0;j<w;j++)
                {
                    out_bas[j]=out_bas[j]/(AziLooks*RanLooks);
                    out_inc[j]=out_inc[j]/(AziLooks*RanLooks);
                    out_slt[j]=out_slt[j]/(AziLooks*RanLooks);
                }
                file1.write((char *)out_slt, sizeof(double)*w);
                file2.write((char *)out_inc, sizeof(float)*w);
                file3.write((char *)out_bas, sizeof(float)*w);

                memset(out_slt,0,sizeof(double)*w);
                memset(out_inc,0,sizeof(float)*w);
                memset(out_bas,0,sizeof(float)*w);
                numclm=0;
            }
        }  // end for 按行处理

        //关闭文件
        file1.close();
        file2.close();
        file3.close();

        delete[] slt_temp;
        delete[] inc_temp;
        delete[] base_temp;
        delete[] position;
        delete[] out_slt;
        delete[] out_inc;
        delete[] out_bas;

        //输出头文件
        CRMGHeader header(mImg.m_oHeader);                      //复制主图像头文件信息
        header.DataType=eFLOAT32;
        BaselineOut.replace(BaselineOut.find("rmg"), 3, "ldr");
        header.Save(BaselineOut);

        InAngleOut.replace(InAngleOut.find("rmg"), 3, "ldr");
        header.Save(InAngleOut);

        header.DataType=eFLOAT64;
        SlateOut.replace(SlateOut.find("rmg"), 3, "ldr");
        header.Save(SlateOut);
}

void CBaseline::AnalyseGeometry(string mfile,string sfile,
                            double &baseline_norm,bool& flag)
{
    CRMGImage mImg(mfile);
    double x=0,y=0,z=0;
    mImg.LonLat2Coordinate(x,y,z);      //图像中心点经纬度对应的x,y,z坐标
    //图像轨道拟合系数  
    CSpMatrix<double> xPolyCoef(4,1);                       //----**aef
    CSpMatrix<double> yPolyCoef(4,1);                       //----**bef
    CSpMatrix<double> zPolyCoef(4,1);                       //----**cef

    mImg.OrbitCoef(mImg.m_oHeader.StateVector,xPolyCoef,yPolyCoef,zPolyCoef);

    double m_azi_t = mImg.getSatposT(x,y,z,xPolyCoef,yPolyCoef,zPolyCoef);

    double satXm = mImg.Polyfit(xPolyCoef,m_azi_t,0);
    double satYm = mImg.Polyfit(yPolyCoef,m_azi_t,0);
    double satZm = mImg.Polyfit(zPolyCoef,m_azi_t,0);

    CRMGImage sImg(sfile);

    sImg.OrbitCoef(sImg.m_oHeader.StateVector,xPolyCoef,yPolyCoef,zPolyCoef);

    double s_azi_t = sImg.getSatposT(x,y,z,xPolyCoef,yPolyCoef,zPolyCoef);

    double satXs = sImg.Polyfit(xPolyCoef,s_azi_t,0);
    double satYs = sImg.Polyfit(yPolyCoef,s_azi_t,0);
    double satZs = sImg.Polyfit(zPolyCoef,s_azi_t,0);

    //定义变量
    double lon1,phi1,h1;
    double lon2,phi2,h2;

    double f=1-mImg.m_oHeader.Ellipse.minor/mImg.m_oHeader.Ellipse.major;       //椭球体扁率
    double e=2*f-f*f;           //偏心率
    double c;

    //计算主图像卫星坐标转换（x,y,z）--->(lon1,phi1,h1)
    lon1=atan2(satYm,satXm);  //经度，单位弧度
    phi1=atan2(satZm,sqrt(satXm*satXm+satYm*satYm));   //纬度，单位弧度
    for(int i=0;i<3;i++)        //3´Î
    {
        double phi=phi1;
        c=1/sqrt(1-e*e*sin(phi)*sin(phi));
        phi1=atan2(satZm+mImg.m_oHeader.Ellipse.major*c*e*e*sin(phi),sqrt(satXm*satXm+satYm*satYm));  //迭代计算
    }
    c=1/sqrt(1-e*e*sin(phi1)*sin(phi1));
    h1=sqrt(satXm*satXm+satYm*satYm)/cos(phi1)-mImg.m_oHeader.Ellipse.major*c;     //主图像卫星高度

    //计算辅图像卫星坐标转换（x,y,z）--->(lon1,phi1,h1)
    lon2=atan2(satYs,satXs);  
    phi2=atan2(satZs,sqrt(satXs*satXs+satYs*satYs));   
    for(int k=0;k<3;k++)    //3
    {
        double phi=phi2;
        c=1/sqrt(1-e*e*sin(phi)*sin(phi));
        phi2=atan2(satZs+sImg.m_oHeader.Ellipse.major*c*e*e*sin(phi),sqrt(satXs*satXs+satYs*satYs));  //µü´ú¼ÆËã3´Î£¬¾ÍÄÜ´ïµ½¾«¶ÈÒªÇóÁË
    }
    c=1/sqrt(1-e*e*sin(phi2)*sin(phi2));
    h2=sqrt(satXs*satXs+satYs*satYs)/cos(phi2)-sImg.m_oHeader.Ellipse.major*c;

    //计算斜距
    double R1=sqrt((satXm-x)*(satXm-x)+(satYm-y)*(satYm-y)+(satZm-z)*(satZm-z));    //Ð±¾à
    double R2=sqrt((satXs-x)*(satXs-x)+(satYs-y)*(satYs-y)+(satZs-z)*(satZs-z));    //Ð±¾à
    //地距
    double L1=sqrt(R1*R1-h1*h1);
    double L2=sqrt(R2*R2-h2*h2);

    //判断卫星成像的几何模型
    //当主图像入射角大于辅图像时，Incflag = ture
    bool Incflag;
    if(L1/h1>L2/h2)
    {
        Incflag=true;
    }
    else
    {
        Incflag=false;
    }

    //基线距
    double baseline_temp=sqrt((satXs-satXm)*(satXs-satXm)+  
        (satYs-satYm)*(satYs-satYm)+
        (satZs-satZm)*(satZs-satZm));   
    //基线夹角
    double angle=acos((R1*R1+baseline_temp*baseline_temp-R2*R2)/(2*R1*baseline_temp));  
    double baselineNorm;
    if(Incflag)
    {
        //垂直基线
        baselineNorm = baseline_temp * sin(angle);
    }
    else 
    {
        //垂直基线
        baselineNorm = -baseline_temp * sin(angle);
    }
    baseline_norm=baselineNorm;
    flag=Incflag;
}


//  插值计算相位
void CBaseline::pwcint(double *x, double *y, long m, double *xresult, double *yresult, long n)
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
void CBaseline::pwcslopes(double*x,double*y,long m,double*del,double*deriv)
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

CBaseline::~CBaseline(void)
{

}
