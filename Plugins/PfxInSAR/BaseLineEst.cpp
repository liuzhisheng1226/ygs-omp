#include "BaseLineEst.h"
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <boost/format.hpp>


CBaseLineEst::CBaseLineEst(void)
{
}

void CBaseLineEst::Process()
{
   /* CDlgBaselineEst dlg;*/
	//if(dlg.DoModal()!=IDOK)
		//return;

	/*BaselineEstimate(dlg.m_strInMaster,dlg.m_strSlave);*/
}

void CBaseLineEst::Batch()
{
}

CBaseLineEst::~CBaseLineEst(void)
{
}

void CBaseLineEst::BaselineEstimate(string strInM,string strInS)
{
    string strInMldr = strInM;
    int dot  = strInMldr.find_last_of('.');
    if(dot == -1)
        strInMldr = strInMldr+".ldr";
    else
        strInMldr = strInMldr.substr(0, dot)+".ldr";

    string strInSldr = strInS;
    dot  = strInSldr.find_last_of('.');
    if(dot == -1)
        strInSldr = strInSldr+".ldr";
    else
        strInSldr = strInSldr.substr(0, dot)+".ldr";

    CRMGImage mImg(strInM,strInMldr);
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

    //计算辅图像的卫星位置
    //LocCoordinate(image2,row2,col2,x3d,y3d,z3d);  
    //CSpMatrix<double> xPolyCoef(4,1);                     //----**aef
    //CSpMatrix<double> yPolyCoef(4,1);                     //----**bef
    //CSpMatrix<double> zPolyCoef(4,1);                     //----**cef
    CRMGImage sImg(strInS,strInSldr);

    sImg.OrbitCoef(sImg.m_oHeader.StateVector,xPolyCoef,yPolyCoef,zPolyCoef);

    double s_azi_t = sImg.getSatposT(x,y,z,xPolyCoef,yPolyCoef,zPolyCoef);

    double satXs = sImg.Polyfit(xPolyCoef,m_azi_t,0);
    double satYs = sImg.Polyfit(yPolyCoef,m_azi_t,0);
    double satZs = sImg.Polyfit(zPolyCoef,m_azi_t,0);

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
    if(Incflag==true)
    {
        //垂直基线
        baselineNorm = baseline_temp * sin(angle);
    }
    else 
    {
        //垂直基线
        baselineNorm = -baseline_temp * sin(angle);
    }

    //计算时间基线
    int nd, nm, ny; //new_day, new_month, new_year  
    int od, om, oy; //old_day, oldmonth, old_year  
    
    nm = (sImg.m_oHeader.ImagingTime.month+9)%12;   //nm = (month2 + 9) % 12;  
    ny = sImg.m_oHeader.ImagingTime.year-nm/10; //ny = year2 - nm/10;  
    nd = 365*ny + ny/4 - ny/100 + ny/400 + (nm*306 + 5)/10 + (sImg.m_oHeader.ImagingTime.day - 1);  
    
    om = (mImg.m_oHeader.ImagingTime.month+9)%12;    //om = (month1 + 9) % 12;  
    oy = mImg.m_oHeader.ImagingTime.year-om/10;     //oy = year1 - om/10;  
    od = 365*oy + oy/4 - oy/100 + oy/400 + (om*306 + 5)/10 + (mImg.m_oHeader.ImagingTime.day - 1);

    int baselineT = nd-od;

    // string baselineInfo;
    cout << boost::format("垂直基线：%1$.2f米; 时间基线：%2%天") % baselineNorm % baselineT << endl;
}
