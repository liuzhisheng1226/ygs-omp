#include "RegistrCoarse.h"
#include <fstream>
#include <cmath>
#include <cstdio>

#define _FILE_OFFSET_BITS 64

CRegistrCoarse::CRegistrCoarse(void)
{
    
}

CRegistrCoarse::~CRegistrCoarse(void)
{

}

void CRegistrCoarse::Process()
{
/*    CDlgRegistrCoarse dlg;*/
    //if(dlg.DoModal()==IDOK)
        //Coarse(dlg.m_lpDataIn1,
            //dlg.m_lpHdrIn1,
            //dlg.m_lpDataIn2,
            //dlg.m_lpHdrIn2,
            //dlg.m_lpDataOut,
            /*dlg.m_lpHdrOut);*/
}

void CRegistrCoarse::Batch()
{
/*    Coarse( m_aryStrs.GetAt(0)*/
        //,  m_aryStrs.GetAt(1)
        //,  m_aryStrs.GetAt(2)
        //,  m_aryStrs.GetAt(3)
        //,  m_aryStrs.GetAt(4)
        /*,  m_aryStrs.GetAt(5));*/
}

void CRegistrCoarse::Coarse(string lpDataFullname1,string lpHdrFullname1,string lpDataFullname2,string lpHdrFullname2,string lpDataExport,string lpHdrExport)
{
    CRMGImage image1(lpDataFullname1,lpHdrFullname1);
    CRMGImage image2(lpDataFullname2,lpHdrFullname2);

    /*
    * Setp 1-4.
    * 计算得到粗配准偏移量
    */
    //定义主、辅图像
    //int row1,col1,row2,col2;                              
    double x3d=0,y3d=0,z3d=0;
    image1.LonLat2Coordinate(x3d,y3d,z3d);

    //计算主图像行、列号
    //LocCoordinate(image1,row1,col1,x3d,y3d,z3d);
    CSpMatrix<double> xPolyCoef(4,1);                       //----**aef
    CSpMatrix<double> yPolyCoef(4,1);                       //----**bef
    CSpMatrix<double> zPolyCoef(4,1);                       //----**cef

    image1.OrbitCoef(image1.m_oHeader.StateVector,xPolyCoef,yPolyCoef,zPolyCoef);

    double m_azi_t = image1.getSatposT(x3d,y3d,z3d,xPolyCoef,yPolyCoef,zPolyCoef);

    double satX = image1.Polyfit(xPolyCoef,m_azi_t,0);
    double satY = image1.Polyfit(yPolyCoef,m_azi_t,0);
    double satZ = image1.Polyfit(zPolyCoef,m_azi_t,0);

    if(image1.m_oHeader.Sensor == 0)
    {
        if(!image1.m_oHeader.PassDirection) //方位向时间
            m_azi_t+=(image1.m_oHeader.StateVector[0].timePoint-image1.m_oHeader.ZeroDopplerTimeLastLine);//升轨
        else
            m_azi_t+=(image1.m_oHeader.StateVector[0].timePoint-image1.m_oHeader.ZeroDopplerTimeFirstLine);//Descend
    }
    else if(image1.m_oHeader.Sensor == 7)
        m_azi_t+=(image1.m_oHeader.StateVector[0].timePoint-image1.m_oHeader.ZeroDopplerTimeFirstLine);//Descend
    

    int rowM= int(m_azi_t*image1.m_oHeader.PulseRepetitionFrequency);                   //int(timeAzimuth*timeSpan);

    double r=sqrt((satX-x3d)*(satX-x3d)+(satY-y3d)*(satY-y3d)+(satZ-z3d)*(satZ-z3d));                                                   //中间变量
    int colM=(r-image1.m_oHeader.SlantRangeTimeToFirstRangeSample/2*image1.CONST_MATH_LIGHT_VELOCITY)/image1.m_oHeader.SampledPixelSpacing;

    //计算辅图像行、列号
    //LocCoordinate(image2,row2,col2,x3d,y3d,z3d);  
    //CSpMatrix<double> xPolyCoef(4,1);                     //----**aef
    //CSpMatrix<double> yPolyCoef(4,1);                     //----**bef
    //CSpMatrix<double> zPolyCoef(4,1);                     //----**cef

    image2.OrbitCoef(image2.m_oHeader.StateVector,xPolyCoef,yPolyCoef,zPolyCoef);

    double s_azi_t = image2.getSatposT(x3d,y3d,z3d,xPolyCoef,yPolyCoef,zPolyCoef);

    satX = image2.Polyfit(xPolyCoef,m_azi_t,0);
    satY = image2.Polyfit(yPolyCoef,m_azi_t,0);
    satZ = image2.Polyfit(zPolyCoef,m_azi_t,0);

    if(image2.m_oHeader.Sensor==0)
    {
        if(!image2.m_oHeader.PassDirection) //方位向时间
            s_azi_t+=(image2.m_oHeader.StateVector[0].timePoint-image2.m_oHeader.ZeroDopplerTimeLastLine);//升轨
        else
            s_azi_t+=(image2.m_oHeader.StateVector[0].timePoint-image2.m_oHeader.ZeroDopplerTimeFirstLine);//Descend
    }
    else if(image2.m_oHeader.Sensor == 7)
        s_azi_t+=(image2.m_oHeader.StateVector[0].timePoint-image2.m_oHeader.ZeroDopplerTimeFirstLine); 

    int rowS= int(s_azi_t*image2.m_oHeader.PulseRepetitionFrequency);                   //int(timeAzimuth*timeSpan);

    r=sqrt((satX-x3d)*(satX-x3d)+(satY-y3d)*(satY-y3d)+(satZ-z3d)*(satZ-z3d));                                                  //中间变量
    int colS=(r-image2.m_oHeader.SlantRangeTimeToFirstRangeSample/2*image1.CONST_MATH_LIGHT_VELOCITY)/image2.m_oHeader.SampledPixelSpacing;

    /*
    * Setp 5.
    * 计算得到粗配准偏移量
    */
    int rowOffset,colOffset;
    if(image1.m_oHeader.Sensor == 0)
    {
        rowOffset= image1.m_oHeader.PassDirection?rowS-rowM:(image2.m_oHeader.Line-rowS)-(image1.m_oHeader.Line-rowM);      //PassDirection=ASCEND（0），descending:ascend, 粗配准行偏移----------shift_l
        colOffset= image1.m_oHeader.PassDirection?(image2.m_oHeader.Sample-colS)-(image1.m_oHeader.Sample-colM):colS-colM;  
    }
    else if(image1.m_oHeader.Sensor == 7)
    {
        rowOffset = rowS-rowM;
        colOffset = colS-colM;
    }
    
                            //粗配准列偏移----------shift_p
    
//  对于R2数据，经过轨道计算的偏移量比较准，可以考虑不进行CoarseOffset的窗口配准，这个需要更多数据检验
//  CoarseOffset(image1,image2,colOffset,rowOffset);

    // 辅图像的公共区域输出 
    FILE* fp1=fopen(image2.m_lpFullname.c_str(),"rb");
    FILE* fp2=fopen(lpDataExport.c_str(),"wb");
    unsigned int sRow;
    if(eCINT16 == image2.m_oHeader.DataType)
    {
        complex<INT16> *data=new complex<INT16>[image2.m_oHeader.Sample];
        complex<INT16> *outdata=new complex<INT16>[image1.m_oHeader.Sample];

        //fseek(fp1,((windowRect.top+rowOffset)*image2.m_oHeader.Sample+windowRect.left+colOffset)*sizeof(complex<INT16>),SEEK_SET);
        for(int i=0;i<image1.m_oHeader.Line;i++)
        {
            memset(data,0,sizeof(complex<INT16>)*image2.m_oHeader.Sample);
            memset(outdata,0,sizeof(complex<INT16>)*image1.m_oHeader.Sample);
            sRow = i+rowOffset;
            if(sRow>=0 && sRow<image2.m_oHeader.Line)
            {
                fseeko(fp1,(sRow*image2.m_oHeader.Sample)*sizeof(complex<INT16>),SEEK_SET);
                fread(data,sizeof(complex<INT16>),image2.m_oHeader.Sample,fp1);
                for(int j=0;j<image1.m_oHeader.Sample;j++)
                {
                    int colS = j + colOffset;
                    if(colS<0 || colS>=image2.m_oHeader.Sample)
                        outdata[j] = complex<INT16>(0,0);
                    else
                        outdata[j] = data[colS];
                }
            }
            fwrite(outdata,sizeof(complex<INT16>),image1.m_oHeader.Sample,fp2);
        }   
        delete[] data;
        delete[] outdata;
    }
    else if(eCFLOAT32 == image2.m_oHeader.DataType)
    {
        complex<float> *data=new complex<float>[image2.m_oHeader.Sample];
        complex<float> *outdata=new complex<float>[image1.m_oHeader.Sample];

        for(int i=0;i<image1.m_oHeader.Line;i++)
        {
            memset(data,0,sizeof(complex<float>)*image2.m_oHeader.Sample);
            memset(outdata,0,sizeof(complex<float>)*image1.m_oHeader.Sample);
            sRow = i+rowOffset;
            if(sRow>=0 && sRow<image2.m_oHeader.Line)
            {
                fseeko(fp1,(sRow*image2.m_oHeader.Sample)*sizeof(complex<float>),SEEK_SET);
                fread(data,sizeof(complex<float>),image2.m_oHeader.Sample,fp1);
                for(int j=0;j<image1.m_oHeader.Sample;j++)
                {
                    int colS = j + colOffset;
                    if(colS<0 || colS>=image2.m_oHeader.Sample)
                        outdata[j] = complex<float>(0.0,0.0);
                    else
                        outdata[j] = data[colS];
                }
            }
            fwrite(outdata,sizeof(complex<float>),image1.m_oHeader.Sample,fp2);
        }   
        delete[] data;
        delete[] outdata;
    }
    
    fclose(fp1);
    fclose(fp2);

    CRMGHeader header(image2.m_oHeader);                        //复制主图像头文件信息
    header.Registration.isRegistr=true;
    header.Registration.registrType=0;
    header.Registration.azimuthOffst=rowOffset;
    header.Registration.rangeOffset=colOffset;
    header.Sample = image1.m_oHeader.Sample;
    header.Line = image1.m_oHeader.Line;
    header.Save(lpHdrExport);
}



float* CRegistrCoarse::CoarseBlock(CRMGImage &image,int blockWidth,int blockHeight,Rect window)
{
    float *block=new float[blockWidth*blockHeight];                 // 子区域数据
    complex<INT16>* blockBuffer=new complex<INT16>[blockWidth];     //子区一行数据
    
    FILE* fp1=fopen(image.m_lpFullname.c_str(),"rb+");
    long offset=(window.top*image.m_oHeader.Sample+window.left)*sizeof(complex<INT16>);
    fseek(fp1,offset,SEEK_SET);
    //读取图像子区域数据
    for(int i=0;i<blockHeight;i++)
    {       
        fread(blockBuffer, sizeof(complex<INT16>),blockWidth,fp1);
        for(int j=0;j<blockWidth;j++)
            block[i*blockWidth+j]=sqrtf(blockBuffer[j].real()*blockBuffer[j].real()+blockBuffer[j].imag()*blockBuffer[j].imag());       
        fseek(fp1,(image.m_oHeader.Sample-blockWidth)*sizeof(complex<INT16>),SEEK_CUR);
    }//for i
    delete[] blockBuffer;
    return  block;
}

void CRegistrCoarse::CoarseOffset(CRMGImage &image1,CRMGImage &image2,int &colOffset,int &rowOffset)
{
    //int blockWidth=1000;                                                                  //子区域宽度
    //int blockHeight=2000;                                                                 //子区域高度
    
    //double x3d=0,y3d=0,z3d=0;
    //image1.LonLat2Coordinate(x3d,y3d,z3d);

//  float *block1=CoarseBlock(image1,blockWidth,blockHeight,0,0,rect);                      //主图像 子区域数据
//  float *block2=CoarseBlock(image2,blockWidth,blockHeight,rowOffset,colOffset,rect);      //辅图像 子区域数据

    int gridRno = int(sqrtf(100*image1.m_oHeader.Line/image1.m_oHeader.Sample));        //行上窗口个数
    int gridCno = int(100/gridRno);                                                 //列上窗口个数
    int gridNo   = gridCno*gridRno;                                 //网格总个数
    const int gridSize =            90;                             //网格大小
    const int gridSearchRange=      50;                             //搜索范围
    
    int gridRangeSpace=(image1.m_oHeader.Sample -2*(gridSize+gridSearchRange+100)-2*abs(colOffset))/gridCno;        //网格距离向距离
    int gridAzimuthSpace=(image1.m_oHeader.Line -2*(gridSize+gridSearchRange+100)-2*abs(rowOffset))/gridRno;        //网格方位向距离
    const int gridBoyndaryOffset_C = (gridSize+gridSearchRange+100) + abs(colOffset); 
    const int gridBoyndaryOffset_R = (gridSize+gridSearchRange+100) + abs(rowOffset);
    // CPoint gridCenter;                                              //网格中心点
    Point gridCenter;
    int gridCount=0;                                                //已计算的网格数量
    int *gridOffsetX = new int[gridNo];                         //距离偏移
    int *gridOffsetY = new int[gridNo];                         //方位偏移
    double *gridMaxCoefAry = new double[gridNo];                        //最大相关系数组
    //初始化
    memset(gridMaxCoefAry,0,sizeof(double)*gridNo);     
    float gridCoef=0,gridMaxCoef=0;                                                 //相关系数

    float *grid1=new float[gridSize*gridSize];                                      //主网格数据
    float *grid2=new float[(gridSize+2*gridSearchRange+1)*(gridSize+2*gridSearchRange+1)];  //辅网格搜索窗口大小的数据
    float *grid22 = new float[gridSize*gridSize];                   //辅图像对应的匹配窗口大小的数据

    
    //float stSum=0,stVar=0,stStdev,stSqSum=0,stMean=0;             //统计变量定义
    //float stSqMean=0;
    CSpStatistics<float> spStat;                                    //统计对象
    
    float tmpValue=0;                                               //临时中间变量

    // CRect dataWin1,dataWin2;
    Rect dataWin1, dataWin2;
    for(int i=0;i<gridRno;i++)                                          //窗口循环
    {
        for(int j=0;j<gridCno;j++)
        {
            dataWin1.SetRect(gridBoyndaryOffset_C+i*gridRangeSpace,gridBoyndaryOffset_R+j*gridAzimuthSpace,
                            gridBoyndaryOffset_C+i*gridRangeSpace+gridSize-1,gridBoyndaryOffset_R+j*gridAzimuthSpace+gridSize-1);

            dataWin2.SetRect(gridBoyndaryOffset_C+i*gridRangeSpace+colOffset-gridSearchRange,gridBoyndaryOffset_R+j*gridAzimuthSpace+rowOffset-gridSearchRange,
                gridBoyndaryOffset_C+i*gridRangeSpace+colOffset+gridSize+gridSearchRange,
                gridBoyndaryOffset_R+j*gridAzimuthSpace+rowOffset+gridSize+gridSearchRange);
            
            grid1 = CoarseBlock(image1,gridSize,gridSize,dataWin1);
            grid2 = CoarseBlock(image2,gridSize+2*gridSearchRange+1,gridSize+2*gridSearchRange+1,dataWin2);

        
            //寻找 辅图像 最大求相关系数
            for(int ii=0;ii<2*gridSearchRange+1;ii++)
            {
                for(int jj=0;jj<2*gridSearchRange+1;jj++)
                {
                    //读取与主图像窗口对应大小的数据
                    for(int iii=0;iii<gridSize;iii++)
                    {
                        for(int jjj=0;jjj<gridSize;jjj++)
                        {
                            tmpValue=grid2[(ii+iii)*(gridSize+gridSearchRange)+(jj+jjj)];
                            grid22[iii*gridSize+jjj]=tmpValue;
                        }//inner for jjj
                    }//inner for iii
                    //计算两网格统计值 及其 协方差
                    spStat.Set(grid1,gridSize*gridSize,grid22);
                    spStat.DoAdvanced();
                    
                    //计算相关系数 --这里采用了统计对象  如严重影响效率可单独写统计代码
                    gridCoef=spStat.m_xVar*spStat.m_yVar==0?0:fabs(spStat.m_Covariance)/sqrt(spStat.m_xVar*spStat.m_yVar);
                    gridOffsetX[gridCount]=0;
                    gridOffsetY[gridCount]=0;
                    if(gridMaxCoef<gridCoef) 
                    {
                        gridMaxCoef=gridCoef;
                        gridOffsetX[gridCount]=jj - gridSearchRange;
                        gridOffsetY[gridCount]=ii - gridSearchRange;
                    }//if
                }//for jj
            }//for ii
            gridMaxCoefAry[gridCount]=gridMaxCoef;//循环开始不用初始化？
            gridCount++;
        }//for j
    }//for i
    CSpStatistics<double> spStatCoef(gridMaxCoefAry,gridNo);                            //最大相关系数统计量
    int stLength=0;                                                                             //统计量  有效个数
    float stMean=0,stStdevX=0,stStdevY=0,stSumX=0,stSumY=0,stMeanX=0,stMeanY=0;

    for(int i=0;i<gridNo;i++)
    {
        if(gridMaxCoefAry[i]>=spStatCoef.m_xMean)
        {
            stSumX+=gridOffsetX[i];
            stSumY+=gridOffsetY[i];
            stStdevX+=gridOffsetX[i]*gridOffsetX[i];
            stStdevY+=gridOffsetY[i]*gridOffsetY[i];
            stLength++;
        }//if
    }//for i
    stMeanX=stSumX/stLength;
    stMeanY=stSumY/stLength;
    //stStdevX=sqrt(stStdevX-stMeanX*stMeanX);
    //stStdevY=sqrt(stStdevY-stMeanY*stMeanY);

    /*stSumX=0;
    stSumY=0;
    stLength=0;
    for(int i=0;i<gridBoundaryOffset;i++)
    {
        if(gridMaxCoefAry[i]>=spStat.m_xMean
         &&abs(gridOffsetX[i]-stMeanX)<=stStdevX
         &&abs(gridOffsetY[i]-stMeanY)<=stStdevY)
        {
            stSumX+=gridOffsetX[i];
            stSumY+=gridOffsetY[i];
            stLength++;
        }//if
    }//for i
    stMeanX=stSumX/stLength;
    stMeanY=stSumY/stLength;*/
    //返回偏移参数
    colOffset+=int(stMeanX);
    rowOffset+=int(stMeanY);

    delete[] gridOffsetX;
    delete[] gridOffsetY;
    delete[] gridMaxCoefAry;
    delete[] grid1;
    delete[] grid2;
    delete[] grid22;
}

/*DOUBLE CRegistrCoarse::LocCoordinate(CRMGImage &image,int &row,int &col,double x3d,double y3d,double z3d)
{
    //图像轨道拟合系数  
    CSpMatrix<double> xPolyCoef(4,1);                       //----**aef
    CSpMatrix<double> yPolyCoef(4,1);                       //----**bef
    CSpMatrix<double> zPolyCoef(4,1);                       //----**cef
    
    image.OrbitCoef(image.m_oHeader.StateVector,xPolyCoef,yPolyCoef,zPolyCoef);

    //double x3d,y3d,z3d;                                           //中心坐标，主、辅图像都用该坐标
    //经纬度转换到三维坐标
    //image.LonLat2Coordinate(x3d,y3d,z3d);

    double timeSpan=image.m_oHeader.StateVector[1].timePoint-image.m_oHeader.StateVector[0].timePoint;
    double ft1= 0;
    //double ft2=(image.CONST_ORBIT_SAPMLING_POINT_COUNT-1)*timeSpan+5; //时间间隔
    double ft2=(image.m_oHeader.StateVector.size()-1)*timeSpan+5;   //时间间隔
    double ft3=(ft1+ft2)/2; 
    
    while(fabs(ft1-ft2)>1e-6)
    {   
        
        //计算多项式值
        double p1=Polynomial(xPolyCoef,yPolyCoef,zPolyCoef,ft1,x3d,y3d,z3d);
        double p2=Polynomial(xPolyCoef,yPolyCoef,zPolyCoef,ft2,x3d,y3d,z3d);
        double p3=Polynomial(xPolyCoef,yPolyCoef,zPolyCoef,ft3,x3d,y3d,z3d);
    
        if(p1*p3>=0&&p2*p3<=0) {ft1=ft3;ft3=(ft1+ft2)/2;}
        else if(p1*p3<=0&&p2*p3>=0)  {ft2=ft3;ft3=(ft1+ft2)/2;}
        else 
        {
            AfxMessageBox("Polyfit Fail!");
            exit(0);
        }
    }//while fabs
    double timeAzimuth= ft3;    
    //计算图像对应坐标
    double x=Polyfit(xPolyCoef,timeAzimuth,0);          
    double y=Polyfit(yPolyCoef,timeAzimuth,0);          
    double z=Polyfit(zPolyCoef,timeAzimuth,0);
    if(!image.m_oHeader.PassDirection)  //方位向时间
        timeAzimuth+=(image.m_oHeader.StateVector[0].timePoint-image.m_oHeader.ZeroDopplerTimeLastLine);//升轨
    else
        timeAzimuth+=(image.m_oHeader.StateVector[0].timePoint-image.m_oHeader.ZeroDopplerTimeFirstLine);//升轨

    row= int(timeAzimuth*image.m_oHeader.PulseRepetitionFrequency);                 //int(timeAzimuth*timeSpan);

    double r=sqrt((x-x3d)*(x-x3d)+(y-y3d)*(y-y3d)+(z-z3d)*(z-z3d));                                                 //中间变量
    col=(r-image.m_oHeader.SlantRangeTimeToFirstRangeSample/2*image.CONST_MATH_LIGHT_VELOCITY)/image.m_oHeader.SampledPixelSpacing;
}
*/


// #pragma region Poly多项式拟合
/*DOUBLE CRegistrCoarse::Polynomial(CSpMatrix<double> &aef,CSpMatrix<double> &bef,CSpMatrix<double> &cef,double t,double x,double y,double z)
{   
    return  (Polyfit(aef,t,0)-x)*Polyfit(aef,t,1)+                  //(X(t)-x0)*Vx(t)
            (Polyfit(bef,t,0)-y)*Polyfit(bef,t,1)+                  //(Y(t)-y0)*Vy(t)
            (Polyfit(cef,t,0)-z)*Polyfit(cef,t,1);                  //(Z(t)-z0)*Vz(t)
}
DOUBLE CRegistrCoarse::Polyfit(CSpMatrix<double> &mat,double t,int order)
{
    double value=0,item=0,coefficient=1;
    if(order==0) 
        value=mat.Get(0,0)+mat.Get(1,0)*t+mat.Get(2,0)*t*t+mat.Get(3,0)*t*t*t;
    else if(order==1) 
        value=mat.Get(1,0)+ mat.Get(2,0)*t*2 + mat.Get(3,0)*t*t*3;  

    //for(int i=0;i<=3;i++)
    //{
    //  //以下两项计算多项式导数
    //  //t求导后多次幂
    //  item=(i-order)>=0?pow(t,i-order):0;         
    //  //导数系数
    //  coefficient=PolyCoef(i,order);
    //  //计算多项式值
    //  value+=mat.Get(i,0)*coefficient*item;
    //}
    return value;
}
LONG CRegistrCoarse::PolyCoef(int n,int order)
{
    long value=1;
    for(int i=0;i<order;i++)
        value*=n-i;     
    return value>0?value:1; 
}*/
// #pragma endregion
