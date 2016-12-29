#include "RegistrCoarse.h"
#include <fstream>
#include <cmath>
#include <cstdio>
#include <omp.h>

//#define _FILE_OFFSET_BITS 64

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
    double x3d=0,y3d=0,z3d=0;
    image1.LonLat2Coordinate(x3d,y3d,z3d);

    //计算主图像行、列号
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
    ifstream fp1(image2.m_lpFullname, ios::in | ios::binary);
    ofstream fp2(lpDataExport, ios::out | ios::binary);

    omp_lock_t r_lock, w_lock;
    omp_init_lock(&r_lock);
    omp_init_lock(&w_lock);
    
    if(eCINT16 == image2.m_oHeader.DataType)
    {
#pragma omp parallel for
        for(int i=0;i<image1.m_oHeader.Line;i++)
        {
            complex<INT16> *data=new complex<INT16>[image2.m_oHeader.Sample];
            complex<INT16> *outdata=new complex<INT16>[image1.m_oHeader.Sample];
            memset(data,0,sizeof(complex<INT16>)*image2.m_oHeader.Sample);
            memset(outdata,0,sizeof(complex<INT16>)*image1.m_oHeader.Sample);

            unsigned int sRow = i+rowOffset;
            if(sRow>=0 && sRow<image2.m_oHeader.Line)
            {
                omp_set_lock(&r_lock);
                fp1.seekg((streampos)(sRow*image2.m_oHeader.Sample)*sizeof(complex<INT16>), ios::beg);
                fp1.read((char *)data, sizeof(complex<INT16>)*image2.m_oHeader.Sample);
                omp_unset_lock(&r_lock);
                for(int j=0;j<image1.m_oHeader.Sample;j++)
                {
                    int colS = j + colOffset;
                    if(colS<0 || colS>=image2.m_oHeader.Sample)
                        outdata[j] = complex<INT16>(0,0);
                    else
                        outdata[j] = data[colS];
                }
            }
            omp_set_lock(&w_lock);
            fp2.seekp((streampos)i*sizeof(complex<INT16>)*image1.m_oHeader.Sample, ios::beg);
            fp2.write((char *)outdata, sizeof(complex<INT16>)*image1.m_oHeader.Sample);
            omp_unset_lock(&w_lock);
            delete[] data;
            delete[] outdata;
        }   
    }
    else if(eCFLOAT32 == image2.m_oHeader.DataType)
    {
#pragma omp parallel for
        for(int i=0;i<image1.m_oHeader.Line;i++)
        {
            complex<float> *data=new complex<float>[image2.m_oHeader.Sample];
            complex<float> *outdata=new complex<float>[image1.m_oHeader.Sample];
            memset(data,0,sizeof(complex<float>)*image2.m_oHeader.Sample);
            memset(outdata,0,sizeof(complex<float>)*image1.m_oHeader.Sample);
            unsigned int sRow = i+rowOffset;
            if(sRow>=0 && sRow<image2.m_oHeader.Line)
            {
                omp_set_lock(&r_lock);
                fp1.seekg((streampos)(sRow*image2.m_oHeader.Sample)*sizeof(complex<float>), ios::beg);
                fp1.read((char *)data, sizeof(complex<float>)*image2.m_oHeader.Sample);
                omp_unset_lock(&r_lock);
                for(int j=0;j<image1.m_oHeader.Sample;j++)
                {
                    int colS = j + colOffset;
                    if(colS<0 || colS>=image2.m_oHeader.Sample)
                        outdata[j] = complex<float>(0.0,0.0);
                    else
                        outdata[j] = data[colS];
                }
            }
            //fwrite(outdata,sizeof(complex<float>),image1.m_oHeader.Sample,fp2);
            omp_set_lock(&w_lock);
            fp2.seekp((streampos)i*sizeof(complex<float>)*image1.m_oHeader.Sample, ios::beg);
            fp2.write((char *)outdata, sizeof(complex<float>)*image1.m_oHeader.Sample);
            omp_unset_lock(&w_lock);
            delete[] data;
            delete[] outdata;
        }   
    }
    
    omp_destroy_lock(&r_lock);
    omp_destroy_lock(&w_lock);
    
    fp1.close();
    fp2.close();

    CRMGHeader header(image2.m_oHeader);                        //复制主图像头文件信息
    header.Registration.isRegistr=true;
    header.Registration.registrType=0;
    header.Registration.azimuthOffst=rowOffset;
    header.Registration.rangeOffset=colOffset;
    header.Sample = image1.m_oHeader.Sample;
    header.Line = image1.m_oHeader.Line;
    header.Save(lpHdrExport);
}

/*
 * add openmp by liutao
 * resize blockBuffer[] to blockWidth*blockHeight 
 * step1: I/O to read to blockBuffer[]
 * step2: parallel generate block[]
 */
float* CRegistrCoarse::CoarseBlock(CRMGImage &image,int blockWidth,int blockHeight,Rect window)
{
    float *block=new float[blockWidth*blockHeight];                 // 子区域数据
    complex<INT16>* blockBuffer=new complex<INT16>[blockWidth*blockHeight];     //子区一行数据
    
    FILE* fp1=fopen(image.m_lpFullname.c_str(),"rb+");
    long offset=(window.top*image.m_oHeader.Sample+window.left)*sizeof(complex<INT16>);
    fseek(fp1,offset,SEEK_SET);
    // read sub-region data to blockBuffer[]
    for(int i=0;i<blockHeight;i++)
    {       
        fread(blockBuffer+i*blockWidth, sizeof(complex<INT16>),blockWidth,fp1);
        fseek(fp1,(image.m_oHeader.Sample-blockWidth)*sizeof(complex<INT16>),SEEK_CUR);
    }//for i
    // generate data to block[]
#pragma omp parallel for
    for(int i=0;i<blockHeight*blockWidth;i++)
    {       
        block[i]=sqrtf(blockBuffer[i].real()*blockBuffer[i].real()+blockBuffer[i].imag()*blockBuffer[i].imag());       
    }//for i
    delete[] blockBuffer;
    return  block;
}

void CRegistrCoarse::CoarseOffset(CRMGImage &image1,CRMGImage &image2,int &colOffset,int &rowOffset)
{
    int gridRno = int(sqrtf(100*image1.m_oHeader.Line/image1.m_oHeader.Sample));        //行上窗口个数
    int gridCno = int(100/gridRno);                                                 //列上窗口个数
    int gridNo   = gridCno*gridRno;                                 //网格总个数
    const int gridSize =            90;                             //网格大小
    const int gridSearchRange=      50;                             //搜索范围
    
    int gridRangeSpace=(image1.m_oHeader.Sample -2*(gridSize+gridSearchRange+100)-2*abs(colOffset))/gridCno;        //网格距离向距离
    int gridAzimuthSpace=(image1.m_oHeader.Line -2*(gridSize+gridSearchRange+100)-2*abs(rowOffset))/gridRno;        //网格方位向距离
    const int gridBoyndaryOffset_C = (gridSize+gridSearchRange+100) + abs(colOffset); 
    const int gridBoyndaryOffset_R = (gridSize+gridSearchRange+100) + abs(rowOffset);
    //Point gridCenter;
    int *gridOffsetX = new int[gridNo];                         //距离偏移
    int *gridOffsetY = new int[gridNo];                         //方位偏移
    double *gridMaxCoefAry = new double[gridNo];                        //最大相关系数组
    memset(gridMaxCoefAry,0,sizeof(double)*gridNo);     

    omp_lock_t img1_lock, img2_lock;
    omp_init_lock(&img1_lock);
    omp_init_lock(&img2_lock);
    
#pragma omp parallel for collapse(2)
    for(int i=0;i<gridRno;i++)                                          //窗口循环
    {
        for(int j=0;j<gridCno;j++)
        {
            Rect dataWin1, dataWin2;
            float *grid1=new float[gridSize*gridSize];                                      //主网格数据
            float *grid2=new float[(gridSize+2*gridSearchRange+1)*(gridSize+2*gridSearchRange+1)];  //辅网格搜索窗口大小的数据
            float *grid22 = new float[gridSize*gridSize];                   //辅图像对应的匹配窗口大小的数据
            CSpStatistics<float> spStat;                                    //统计对象
            float gridCoef=0,gridMaxCoef=0;                                                 //相关系数

            dataWin1.SetRect(gridBoyndaryOffset_C+i*gridRangeSpace,gridBoyndaryOffset_R+j*gridAzimuthSpace,
                            gridBoyndaryOffset_C+i*gridRangeSpace+gridSize-1,gridBoyndaryOffset_R+j*gridAzimuthSpace+gridSize-1);

            dataWin2.SetRect(gridBoyndaryOffset_C+i*gridRangeSpace+colOffset-gridSearchRange,gridBoyndaryOffset_R+j*gridAzimuthSpace+rowOffset-gridSearchRange,
                gridBoyndaryOffset_C+i*gridRangeSpace+colOffset+gridSize+gridSearchRange,
                gridBoyndaryOffset_R+j*gridAzimuthSpace+rowOffset+gridSize+gridSearchRange);
            
            // CoarseBlock contains I/O operations, to avoid disk pointer busy, make it a critical section
            omp_set_lock(&img1_lock);
            grid1 = CoarseBlock(image1,gridSize,gridSize,dataWin1);
            omp_unset_lock(&img1_lock);
            omp_set_lock(&img2_lock);
            grid2 = CoarseBlock(image2,gridSize+2*gridSearchRange+1,gridSize+2*gridSearchRange+1,dataWin2);
            omp_unset_lock(&img2_lock);

        
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
                            grid22[iii*gridSize+jjj]=grid2[(ii+iii)*(gridSize+gridSearchRange)+(jj+jjj)];
                        }//inner for jjj
                    }//inner for iii

                    //计算两网格统计值 及其 协方差
                    spStat.Set(grid1,gridSize*gridSize,grid22);
                    spStat.DoAdvanced();
                    
                    //计算相关系数 --这里采用了统计对象  如严重影响效率可单独写统计代码
                    gridCoef=spStat.m_xVar*spStat.m_yVar==0?0:fabs(spStat.m_Covariance)/sqrt(spStat.m_xVar*spStat.m_yVar);
                    gridOffsetX[i*gridCno+j]=0;
                    gridOffsetY[i*gridCno+j]=0;
                    if(gridMaxCoef<gridCoef) 
                    {
                        gridMaxCoef=gridCoef;
                        gridOffsetX[i*gridCno+j]=jj - gridSearchRange;
                        gridOffsetY[i*gridCno+j]=ii - gridSearchRange;
                    }//if
                }//for jj
            }//for ii
            gridMaxCoefAry[i*gridCno+j]=gridMaxCoef;//循环开始不用初始化？

            delete[] grid1;
            delete[] grid2;
            delete[] grid22;
        }//for j
    }//for i

    omp_destroy_lock(&img1_lock);
    omp_destroy_lock(&img2_lock);

    CSpStatistics<double> spStatCoef(gridMaxCoefAry,gridNo);                            //最大相关系数统计量
    int stLength=0;                                                                             //统计量  有效个数
    float stMean=0,stStdevX=0,stStdevY=0,stSumX=0,stSumY=0,stMeanX=0,stMeanY=0;

#pragma omp parallel for reduction(+: stSumX, stSumY, stStdevX, stStdevY, stLength)
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

    //返回偏移参数
    colOffset+=int(stMeanX);
    rowOffset+=int(stMeanY);

    delete[] gridOffsetX;
    delete[] gridOffsetY;
    delete[] gridMaxCoefAry;
}
