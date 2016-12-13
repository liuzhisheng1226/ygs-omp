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
    * ����õ�����׼ƫ����
    */
    //����������ͼ��
    //int row1,col1,row2,col2;                              
    double x3d=0,y3d=0,z3d=0;
    image1.LonLat2Coordinate(x3d,y3d,z3d);

    //������ͼ���С��к�
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
        if(!image1.m_oHeader.PassDirection) //��λ��ʱ��
            m_azi_t+=(image1.m_oHeader.StateVector[0].timePoint-image1.m_oHeader.ZeroDopplerTimeLastLine);//����
        else
            m_azi_t+=(image1.m_oHeader.StateVector[0].timePoint-image1.m_oHeader.ZeroDopplerTimeFirstLine);//Descend
    }
    else if(image1.m_oHeader.Sensor == 7)
        m_azi_t+=(image1.m_oHeader.StateVector[0].timePoint-image1.m_oHeader.ZeroDopplerTimeFirstLine);//Descend
    

    int rowM= int(m_azi_t*image1.m_oHeader.PulseRepetitionFrequency);                   //int(timeAzimuth*timeSpan);

    double r=sqrt((satX-x3d)*(satX-x3d)+(satY-y3d)*(satY-y3d)+(satZ-z3d)*(satZ-z3d));                                                   //�м����
    int colM=(r-image1.m_oHeader.SlantRangeTimeToFirstRangeSample/2*image1.CONST_MATH_LIGHT_VELOCITY)/image1.m_oHeader.SampledPixelSpacing;

    //���㸨ͼ���С��к�
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
        if(!image2.m_oHeader.PassDirection) //��λ��ʱ��
            s_azi_t+=(image2.m_oHeader.StateVector[0].timePoint-image2.m_oHeader.ZeroDopplerTimeLastLine);//����
        else
            s_azi_t+=(image2.m_oHeader.StateVector[0].timePoint-image2.m_oHeader.ZeroDopplerTimeFirstLine);//Descend
    }
    else if(image2.m_oHeader.Sensor == 7)
        s_azi_t+=(image2.m_oHeader.StateVector[0].timePoint-image2.m_oHeader.ZeroDopplerTimeFirstLine); 

    int rowS= int(s_azi_t*image2.m_oHeader.PulseRepetitionFrequency);                   //int(timeAzimuth*timeSpan);

    r=sqrt((satX-x3d)*(satX-x3d)+(satY-y3d)*(satY-y3d)+(satZ-z3d)*(satZ-z3d));                                                  //�м����
    int colS=(r-image2.m_oHeader.SlantRangeTimeToFirstRangeSample/2*image1.CONST_MATH_LIGHT_VELOCITY)/image2.m_oHeader.SampledPixelSpacing;

    /*
    * Setp 5.
    * ����õ�����׼ƫ����
    */
    int rowOffset,colOffset;
    if(image1.m_oHeader.Sensor == 0)
    {
        rowOffset= image1.m_oHeader.PassDirection?rowS-rowM:(image2.m_oHeader.Line-rowS)-(image1.m_oHeader.Line-rowM);      //PassDirection=ASCEND��0����descending:ascend, ����׼��ƫ��----------shift_l
        colOffset= image1.m_oHeader.PassDirection?(image2.m_oHeader.Sample-colS)-(image1.m_oHeader.Sample-colM):colS-colM;  
    }
    else if(image1.m_oHeader.Sensor == 7)
    {
        rowOffset = rowS-rowM;
        colOffset = colS-colM;
    }
    
                            //����׼��ƫ��----------shift_p
    
//  ����R2���ݣ�������������ƫ�����Ƚ�׼�����Կ��ǲ�����CoarseOffset�Ĵ�����׼�������Ҫ�������ݼ���
//  CoarseOffset(image1,image2,colOffset,rowOffset);

    // ��ͼ��Ĺ���������� 
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

    CRMGHeader header(image2.m_oHeader);                        //������ͼ��ͷ�ļ���Ϣ
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
    float *block=new float[blockWidth*blockHeight];                 // ����������
    complex<INT16>* blockBuffer=new complex<INT16>[blockWidth];     //����һ������
    
    FILE* fp1=fopen(image.m_lpFullname.c_str(),"rb+");
    long offset=(window.top*image.m_oHeader.Sample+window.left)*sizeof(complex<INT16>);
    fseek(fp1,offset,SEEK_SET);
    //��ȡͼ������������
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
    //int blockWidth=1000;                                                                  //��������
    //int blockHeight=2000;                                                                 //������߶�
    
    //double x3d=0,y3d=0,z3d=0;
    //image1.LonLat2Coordinate(x3d,y3d,z3d);

//  float *block1=CoarseBlock(image1,blockWidth,blockHeight,0,0,rect);                      //��ͼ�� ����������
//  float *block2=CoarseBlock(image2,blockWidth,blockHeight,rowOffset,colOffset,rect);      //��ͼ�� ����������

    int gridRno = int(sqrtf(100*image1.m_oHeader.Line/image1.m_oHeader.Sample));        //���ϴ��ڸ���
    int gridCno = int(100/gridRno);                                                 //���ϴ��ڸ���
    int gridNo   = gridCno*gridRno;                                 //�����ܸ���
    const int gridSize =            90;                             //�����С
    const int gridSearchRange=      50;                             //������Χ
    
    int gridRangeSpace=(image1.m_oHeader.Sample -2*(gridSize+gridSearchRange+100)-2*abs(colOffset))/gridCno;        //������������
    int gridAzimuthSpace=(image1.m_oHeader.Line -2*(gridSize+gridSearchRange+100)-2*abs(rowOffset))/gridRno;        //����λ�����
    const int gridBoyndaryOffset_C = (gridSize+gridSearchRange+100) + abs(colOffset); 
    const int gridBoyndaryOffset_R = (gridSize+gridSearchRange+100) + abs(rowOffset);
    // CPoint gridCenter;                                              //�������ĵ�
    Point gridCenter;
    int gridCount=0;                                                //�Ѽ������������
    int *gridOffsetX = new int[gridNo];                         //����ƫ��
    int *gridOffsetY = new int[gridNo];                         //��λƫ��
    double *gridMaxCoefAry = new double[gridNo];                        //������ϵ����
    //��ʼ��
    memset(gridMaxCoefAry,0,sizeof(double)*gridNo);     
    float gridCoef=0,gridMaxCoef=0;                                                 //���ϵ��

    float *grid1=new float[gridSize*gridSize];                                      //����������
    float *grid2=new float[(gridSize+2*gridSearchRange+1)*(gridSize+2*gridSearchRange+1)];  //�������������ڴ�С������
    float *grid22 = new float[gridSize*gridSize];                   //��ͼ���Ӧ��ƥ�䴰�ڴ�С������

    
    //float stSum=0,stVar=0,stStdev,stSqSum=0,stMean=0;             //ͳ�Ʊ�������
    //float stSqMean=0;
    CSpStatistics<float> spStat;                                    //ͳ�ƶ���
    
    float tmpValue=0;                                               //��ʱ�м����

    // CRect dataWin1,dataWin2;
    Rect dataWin1, dataWin2;
    for(int i=0;i<gridRno;i++)                                          //����ѭ��
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

        
            //Ѱ�� ��ͼ�� ��������ϵ��
            for(int ii=0;ii<2*gridSearchRange+1;ii++)
            {
                for(int jj=0;jj<2*gridSearchRange+1;jj++)
                {
                    //��ȡ����ͼ�񴰿ڶ�Ӧ��С������
                    for(int iii=0;iii<gridSize;iii++)
                    {
                        for(int jjj=0;jjj<gridSize;jjj++)
                        {
                            tmpValue=grid2[(ii+iii)*(gridSize+gridSearchRange)+(jj+jjj)];
                            grid22[iii*gridSize+jjj]=tmpValue;
                        }//inner for jjj
                    }//inner for iii
                    //����������ͳ��ֵ ���� Э����
                    spStat.Set(grid1,gridSize*gridSize,grid22);
                    spStat.DoAdvanced();
                    
                    //�������ϵ�� --���������ͳ�ƶ���  ������Ӱ��Ч�ʿɵ���дͳ�ƴ���
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
            gridMaxCoefAry[gridCount]=gridMaxCoef;//ѭ����ʼ���ó�ʼ����
            gridCount++;
        }//for j
    }//for i
    CSpStatistics<double> spStatCoef(gridMaxCoefAry,gridNo);                            //������ϵ��ͳ����
    int stLength=0;                                                                             //ͳ����  ��Ч����
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
    //����ƫ�Ʋ���
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
    //ͼ�������ϵ��  
    CSpMatrix<double> xPolyCoef(4,1);                       //----**aef
    CSpMatrix<double> yPolyCoef(4,1);                       //----**bef
    CSpMatrix<double> zPolyCoef(4,1);                       //----**cef
    
    image.OrbitCoef(image.m_oHeader.StateVector,xPolyCoef,yPolyCoef,zPolyCoef);

    //double x3d,y3d,z3d;                                           //�������꣬������ͼ���ø�����
    //��γ��ת������ά����
    //image.LonLat2Coordinate(x3d,y3d,z3d);

    double timeSpan=image.m_oHeader.StateVector[1].timePoint-image.m_oHeader.StateVector[0].timePoint;
    double ft1= 0;
    //double ft2=(image.CONST_ORBIT_SAPMLING_POINT_COUNT-1)*timeSpan+5; //ʱ����
    double ft2=(image.m_oHeader.StateVector.size()-1)*timeSpan+5;   //ʱ����
    double ft3=(ft1+ft2)/2; 
    
    while(fabs(ft1-ft2)>1e-6)
    {   
        
        //�������ʽֵ
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
    //����ͼ���Ӧ����
    double x=Polyfit(xPolyCoef,timeAzimuth,0);          
    double y=Polyfit(yPolyCoef,timeAzimuth,0);          
    double z=Polyfit(zPolyCoef,timeAzimuth,0);
    if(!image.m_oHeader.PassDirection)  //��λ��ʱ��
        timeAzimuth+=(image.m_oHeader.StateVector[0].timePoint-image.m_oHeader.ZeroDopplerTimeLastLine);//����
    else
        timeAzimuth+=(image.m_oHeader.StateVector[0].timePoint-image.m_oHeader.ZeroDopplerTimeFirstLine);//����

    row= int(timeAzimuth*image.m_oHeader.PulseRepetitionFrequency);                 //int(timeAzimuth*timeSpan);

    double r=sqrt((x-x3d)*(x-x3d)+(y-y3d)*(y-y3d)+(z-z3d)*(z-z3d));                                                 //�м����
    col=(r-image.m_oHeader.SlantRangeTimeToFirstRangeSample/2*image.CONST_MATH_LIGHT_VELOCITY)/image.m_oHeader.SampledPixelSpacing;
}
*/


// #pragma region Poly����ʽ���
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
    //  //��������������ʽ����
    //  //t�󵼺�����
    //  item=(i-order)>=0?pow(t,i-order):0;         
    //  //����ϵ��
    //  coefficient=PolyCoef(i,order);
    //  //�������ʽֵ
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
