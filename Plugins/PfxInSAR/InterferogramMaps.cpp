#include "InterferogramMaps.h"
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <fstream>

CInterferogramMaps::CInterferogramMaps(void)
{
}

CInterferogramMaps::~CInterferogramMaps(void)
{
}

void CInterferogramMaps::Batch()
{
}
void CInterferogramMaps::Process()
{
       /* CDlgInterferogramMaps dlg;*/
        //if(dlg.DoModal()!=IDOK)
            //return;
        /*InterferoGramMapAnalysis(dlg.m_strSARs,dlg.m_strOutput,dlg.m_fBv,dlg.m_fBt);*/
}

void CInterferogramMaps::InterferoGramMapAnalysis(string inFiles,string outMaps,float BvT,float BtT)
{
    FILE *outFp;
    ifstream fp (inFiles.c_str(), ios::in);

    if (!fp.is_open())
    {
        printf("open SAR-list file failed\n");
        return;
    }
    string bufString;
    vector<string> fileNames;
    while (getline(fp, bufString)) {
        fileNames.push_back(bufString);
    }
    fp.close();
    int fileNo = fileNames.size();
    int *Bt = new int[fileNo];
    float *Bv = new float[fileNo];

    float baselineV;
    int baselineT;
    
    Bv[0] = 0;  Bt[0]=0;
    for(int i=1;i<fileNo;i++)
    {
        cout << "enter BaselineCal\n";
        BaselineCal(fileNames[0],fileNames[i],baselineV, baselineT);
        cout << "exit BaselineCal\n";
        Bv[i] = baselineV;
        Bt[i] = baselineT;
        cout << "Bv[" << i << "]:" << Bv[i] << " " << "Bt[" << i << "]:" << Bt[i] << endl;
    }
    if((outFp=fopen(outMaps.c_str(),"wt"))==NULL)
    {
        delete[] Bt;
        delete[] Bv;
        return;
    }
    int insarNo=0;
    for(int i=0;i<fileNo-1;i++)
    {
        for(int j=i+1;j<fileNo;j++)
        {
            if(fabs(Bv[j]-Bv[i])<BvT && abs(Bt[j]-Bt[i])<BtT)
                fprintf(outFp,"%d\t%s\t%s\t%.2f\t%d\n",insarNo++,fileNames[i].c_str(),fileNames[j].c_str(),Bv[j]-Bv[i],Bt[j]-Bt[i]);
        }
    }
    fprintf(outFp,"[干涉组合个数]\n\t\t%d",insarNo);

    fclose(outFp);
}

void CInterferogramMaps::BaselineCal(string strInM,string strInS,float &baselineNorm, int &baselineT)
{
    string strInMldr = strInM;
    cout << "strInMldr: " << strInMldr << endl;
    int dot  = strInMldr.find_last_of('.');
    if(dot == -1)
        strInMldr = strInMldr+".ldr";
    else
        strInMldr = strInMldr.substr(0, dot)+".ldr";
    cout << "strInMldr: " << strInMldr << endl;

    string strInSldr = strInS;
    cout << "strInSldr: " << strInSldr<< endl;
    dot  = strInSldr.find_last_of('.');
    if(dot == -1)
        strInSldr = strInSldr+".ldr";
    else
        strInSldr = strInSldr.substr(0, dot)+".ldr";
    cout << "strInSldr: " << strInSldr<< endl;

    CRMGImage mImg(strInM,strInMldr);
    cout << "mImg constructor successful" << endl;
    double x=0,y=0,z=0;
    mImg.LonLat2Coordinate(x,y,z);      //图像中心点经纬度对应的x,y,z坐标
    //图像轨道拟合系数  
    CSpMatrix<double> xPolyCoef(4,1);                       //----**aef
    CSpMatrix<double> yPolyCoef(4,1);                       //----**bef
    CSpMatrix<double> zPolyCoef(4,1);                       //----**cef

    cout << "mImg before OrbitCoef" << endl;
    mImg.OrbitCoef(mImg.m_oHeader.StateVector,xPolyCoef,yPolyCoef,zPolyCoef);
    cout << "mImg after OrbitCoef" << endl;

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
    cout << "sImg constructor successful" << endl;

    cout << "sImg before OrbitCoef" << endl;
    sImg.OrbitCoef(sImg.m_oHeader.StateVector,xPolyCoef,yPolyCoef,zPolyCoef);
    cout << "sImg after OrbitCoef" << endl;

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
    //float baselineNorm;
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

    baselineT = nd-od;

    return;
}

