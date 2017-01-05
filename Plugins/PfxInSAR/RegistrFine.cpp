#include "RegistrFine.h"
#include "../../Share/RMGImage.h"
#include <complex>
#include <cstdlib>
#include <iostream>
#include <fstream>
using namespace std;

CRegistrFine::CRegistrFine(void)
    : CONST_BLOCK_DEPTH(200)
    , CONST_BLOCK_DIFF(600)
{
}

CRegistrFine::~CRegistrFine(void)
{
}

void CRegistrFine::Process()
{
/*        CDlgFineReg dlg;*/
        //if(dlg.DoModal()!=IDOK) return;
            //Fine(dlg.m_lpDataIn1,
            //dlg.m_lpHdrIn1,
            //dlg.m_lpDataIn2,
            //dlg.m_lpHdrIn2,
            //dlg.m_lpDataOutM,
            //dlg.m_lpHdrOutM,
            //dlg.m_lpDataOutS,
            /*dlg.m_lpHdrOutS);*/
}
void CRegistrFine::Batch()
{
/*    Fine( m_aryStrs.GetAt(0)*/
        //,  m_aryStrs.GetAt(1)
        //,  m_aryStrs.GetAt(2)
        //,  m_aryStrs.GetAt(3)
        //,  m_aryStrs.GetAt(4)
        //,  m_aryStrs.GetAt(5)
        //,  m_aryStrs.GetAt(6)
        /*,  m_aryStrs.GetAt(7) );*/
}


void CRegistrFine::Fine(string lpDataIn1,string lpHdrIn1,
                        string lpDataIn2,string lpHdrIn2,
                        string lpDataOutM,string lpHdrOutM,
                        string lpDataOut,string lpHdrOut)
{
    CRMGImage mImg(lpDataIn1,lpHdrIn1);
    CRMGImage sImg(lpDataIn2,lpHdrIn2);
    printf("image construct complete!\n");
    int Width;          //主辅图像大小
    int Height;
    float shift_azm = sImg.m_oHeader.Registration.azimuthOffst; //粗配准方位向偏移
    int mdatatype;      //主图像数据类型: 5, 6;

    //获取数据属性信息
    Width = mImg.m_oHeader.Sample;
    Height= mImg.m_oHeader.Line;
    mdatatype= mImg.m_oHeader.DataType;

    //打开主辅图像
    ifstream file1(lpDataIn1, ios::in);
    if (!file1.is_open()) {
        cout << "error open image 1\n";
        exit(1) ;
    }
    ifstream file2(lpDataIn2, ios::in);
    if (!file2.is_open()) {
        cout << "error open image 2\n";
        exit(1);
    }

    //设置匹配窗口空间
    int boxsize1=91;    //主图像窗口大小
    int boxsize2=93;    //1个像素的偏移窗口
    int boxsize3=111;   //10个像素偏移的窗口
    int boxsize4=103;   //6个像素偏移的窗口
    float *mbox=new float[boxsize1*boxsize1];                       //亚像元级配准主窗口
    complex<float>*sbox=new complex<float>[boxsize2*boxsize2];      //亚像元级配准辅窗口
    float *bigslave=new float[boxsize2*boxsize2*8*8];   //插值到亚像元级后的窗口大小            

    //设置方位向和距离向窗口格网的个数
    int numboxAzm=24;       //方位向窗口个数：一般较多，因为方位尺度较小
    int numboxRng=16;       //距离向窗口个数；不要改变这个参数的设置
    
    //窗口中心在整幅图像中的行列坐标
    unsigned int *centerx=new unsigned int[numboxAzm*numboxRng];            //行
    unsigned int *centery=new unsigned int[numboxAzm*numboxRng];            //列
    int  i,j;
    int stepx=(Height-300*2)/numboxAzm;     //方位向窗口中心间隔，边缘空300
    int stepy=(Width-300*2)/numboxRng;      //距离向窗口中心间隔，边缘空300
    for(i=0;i<numboxAzm;i++)
    {
        for(j=0;j<numboxRng;j++)
        {
            centerx[i*numboxRng+j]=300+i*stepx;     //行
            centery[i*numboxRng+j]=300+j*stepy;     //列
        }
    }
    

    //读取numboxAzm*numboxRng个窗口数据，获得配准参数
    //保存偏移数据的矩阵
    int *xs=new int[numboxAzm*numboxRng];   //方位向粗配准偏移
    int *ys=new int[numboxAzm*numboxRng];   //距离向粗配准偏移

    float* xshift=new float[numboxAzm*numboxRng];       //方位向的精配准偏移
    float* yshift=new float[numboxAzm*numboxRng];       //距离向的精配准偏移
    float* fcof=new float[numboxAzm*numboxRng];         //最佳配准位置的相干系数值
    //初始化
    memset(xs,0,sizeof(int)*numboxAzm*numboxRng);
    memset(ys,0,sizeof(int)*numboxAzm*numboxRng);
    memset(xshift,0,sizeof(float)*numboxAzm*numboxRng);
    memset(yshift,0,sizeof(float)*numboxAzm*numboxRng);
    memset(fcof,0,sizeof(float)*numboxAzm*numboxRng);

    //////////////////////////////////////////////////////////////////////////
    //下面这对代码用于在较大的范围内估计像元级配准参数；
    //////////////////////////////////////////////////////////////////////////
    int temp_x=0;       //方位向的偏移
    int temp_y[3];      //距离向: 左，中，右三个部分的偏移；主要考虑距离向
                        //近距，中距和远距具有不同的偏移
    
    //1.读取主辅图像大窗口数据： 考察在距离向德第1个，第4个和第7个窗口，代表了近距，中距和远距
// #pragma region C short Regis
    if(mdatatype== eCINT16) //如果主图像为complex<short>
    {
        complex<short>*master=new complex<short>[boxsize1*boxsize1];    //主图像窗口81*81
        complex<short>*slave= new complex<short>[boxsize3*boxsize3];    //辅图像窗口101*87

        complex<short>*master_block=new complex<short>[boxsize1*Width]; //主图像窗口81*Width
        complex<short>*slave_block= new complex<short>[boxsize3*Width]; //辅图像窗口87*Width
        //

        //下面的代码是为了防止近距，中距，和远距在距离向的偏移像素不同而设计的；
        //在近距，中距，远距分别选三个窗口计算相关系数(设置较大的搜索范围)；分别确定
        //近距，中距，和远距的距离向偏移量(像素级);至于方位向偏移的估计，则是对所有9个
        //窗口的方位结果进行统计考虑，提取2倍标准差偏移的均值作为方位向初始偏移。
        //2007-02-07, 吴涛,雷达微波遥感组

        //初始化临时变量
        int tempnn[9];
        for( i=0;i<9;i++)
            tempnn[i]=0;

        //在列的方向上也是考虑3个窗口
        for(i=0;i<3;i++)
        {
            //初始化临时变量
            int z1[3];          
            float z_cof[3];
            z1[0]=0;
            z1[1]=0;
            z1[2]=0;

            //在行的方向上考虑3个窗口
            for(j=0;j<3;j++)
            {
                //进度条
                /*
                wndProgress.StepIt();
                wndProgress.PeekAndPump();
                if (wndProgress.Cancelled()) 
                {
                    AfxMessageBox("进程被人为中断!");
                    file1.close();
                    file2.close();
                    
                    delete[] master;
                    delete[] slave;
                    delete[] mbox;
                    delete[] sbox;
                    delete[] bigslave;
                    delete[] xs;
                    delete[] ys;
                    delete[] xshift;
                    delete[] yshift;
                    delete[] fcof;

                    delete[] master_block;
                    delete[] slave_block;

                    delete[] centerx;
                    delete[] centery;
                        
                    wndProgress.DestroyWindow();                
                    return;
                }
                */

                //确定窗口的位置
                int tempstep_a=numboxAzm/3; //近似3等分的位置
                int tempstep_r=numboxRng/3; //近似3等分的位置
                //读取主图像窗口数据
                //change seek() from MFC to C++
                // file1.seekg(((centerx[(j*tempstep_a+2)*numboxRng+(i+1)*tempstep_r-1]-boxsize1/2)*Width+
                            // (centery[(j*tempstep_a+2)*numboxRng+(i+1)*tempstep_r-1]-boxsize1/2))*
                           // sizeof(complex<short>),
                           // CFile::begin);
                file1.seekg(((centerx[(j * tempstep_a + 2)*numboxRng + (i + 1)*tempstep_r - 1] - boxsize1 / 2)*Width +
                             (centery[(j * tempstep_a + 2)*numboxRng + (i + 1)*tempstep_r - 1] - boxsize1 / 2))*
                            sizeof(complex<short>),
                            ios::beg);
                for(int k=0;k<boxsize1;k++)
                {
                    file1.read((char *)(master+k*boxsize1),boxsize1*sizeof(complex<short>));
                    // file1.seekg((Width-boxsize1)*sizeof(complex<short>),ios::cur);
                    file1.seekg((Width - boxsize1)*sizeof(complex<short>), ios::cur);
                }
                //读取幅图像窗口数据
                // file2.seekg(((centerx[(j*tempstep_a+2)*numboxRng+(i+1)*tempstep_r-1]-boxsize3/2)*Width+
                            // (centery[(j*tempstep_a+2)*numboxRng+(i+1)*tempstep_r-1]-boxsize3/2))*
                           // sizeof(complex<short>),
                           // CFile::begin);
                file2.seekg(((centerx[(j * tempstep_a + 2)*numboxRng + (i + 1)*tempstep_r - 1] - boxsize3 / 2)*Width +
                             (centery[(j * tempstep_a + 2)*numboxRng + (i + 1)*tempstep_r - 1] - boxsize3 / 2))*
                            sizeof(complex<short>),
                            ios::beg);
                for(int k=0;k<boxsize3;k++)
                {
                    // file2.Read(slave+k*boxsize3,boxsize3*sizeof(complex<short>));
                    file2.read((char *)(slave + k * boxsize3), boxsize3 * sizeof(complex<short>));
                    //file2.seekg((Width-boxsize3)*sizeof(complex<short>),ios::cur);
                    file2.seekg((Width - boxsize3)*sizeof(complex<short>), ios::cur);
                }

                //2.利用大窗口数据，计算粗配准参数
                double temp;
                double temp1,temp2,temp3;
                int cc,dd;
                //
                double msum=0;          //纪录均值
                double m2sum=0;         //纪录平方的均值
                int box=boxsize1;               //匹配窗口大小为81*81
                for(cc=-boxsize1/2;cc<=boxsize1/2;cc++)         //hang
                {
                    for(dd=-boxsize1/2;dd<=boxsize1/2;dd++)     //lie
                    {
                        temp = abs(complex<float>(master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].real(),master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].imag()));
                        //sqrt(master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].real()*master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].real()+
                        //     master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].imag()+master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].imag());//
                        msum=msum+temp;
                        m2sum=m2sum+temp*temp;
                    }
                }
                msum=msum/(box*box);        //均值
                m2sum=m2sum/(box*box);
                temp2=m2sum-msum*msum;      //方差1

                //求相关系数大小; 
                int ee,ff;
                double ssum=0;      //记录均值
                double s2sum=0;     //记录平方的均值
                double mssum=0;     //记录乘积的均值                    
                double maxcof=0;    //记录这一轮比较的最大的相关系数
                double cof=0;       //中间相关系数

                int step=(boxsize3-boxsize1)/2;         //两窗口之间的偏移量
                for(ee=-step;ee<=step;ee++)             //hang
                {   
                    for(ff=-step;ff<=step;ff++)         //lie
                    {
                        ssum=0;
                        s2sum=0;
                        mssum=0;
                        //计算参数
                        for(cc=-boxsize1/2;cc<=boxsize1/2;cc++)     //hang
                        {
                            for(dd=-boxsize1/2;dd<=boxsize1/2;dd++) //lie
                            {
                                temp= abs(complex<float>(slave[(cc+boxsize3/2+ee)*boxsize3+(dd+boxsize3/2+ff)].real(),slave[(cc+boxsize3/2+ee)*boxsize3+(dd+boxsize3/2+ff)].imag()));
                                //sqrt(slave[(cc+boxsize3/2+ee)*boxsize3+(dd+boxsize3/2+ff)].real()*slave[(cc+boxsize3/2+ee)*boxsize3+(dd+boxsize3/2+ff)].real()+
                                //      slave[(cc+boxsize3/2+ee)*boxsize3+(dd+boxsize3/2+ff)].imag()*slave[(cc+boxsize3/2+ee)*boxsize3+(dd+boxsize3/2+ff)].imag());//
                                ssum=ssum+temp;
                                s2sum=s2sum+temp*temp;
                                mssum=mssum+abs(complex<float>(master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].real(),master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].imag()))*temp;
                                //sqrt(master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].real()*master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].real()+
                                //           master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].imag()*master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].imag())*temp;       //
                            }
                        }
                        ssum=ssum/(box*box);
                        s2sum=s2sum/(box*box);
                        mssum=mssum/(box*box);
                        
                        temp3=s2sum-ssum*ssum;      //方差2
                        temp1=mssum-msum*ssum;      //协方差
                        //计算相关系数
                        if(temp2*temp3==0)
                        {
                            cof=0;
                        }
                        else
                        {
                            cof=fabs(temp1)/sqrt(temp2*temp3);
                        }
                        //找到最合适的配准位置
                        if(maxcof<cof)
                        {
                            maxcof=cof;
                            tempnn[i*3+j]=ee;   //hang  
                            z1[j]=ff;           //lie   
                        }
                        if(maxcof==0)           //如果maxcof=0，说明没有改变，不发生偏移
                        {
                            tempnn[i*3+j]=0;    //hang  
                            z1[j]=0;            //lie   
                        }
                    }
                }
                //保存最大配准位置
                z_cof[j]=maxcof;
            }//end j
            double maxtemp=z_cof[0];
            temp_y[i]=z1[0];

            //到一个最合适的距离向偏移
            for(j=0;j<3;j++)
            {
                if(z_cof[j] > maxtemp)
                {
                    maxtemp=z_cof[j];
                    temp_y[i]=z1[j];
                    //temp_x=tempnn[j];
                }
            }
            if(maxtemp < 0.1) {temp_y[i]=0;}//temp_x=0;}
            
        }
        //考虑方位向初始偏移
        double tempnn_mean=0;
        double tempnn_std=0;
        //通过均值和标准差控制误差
        for(i=0;i<9;i++)
        {
            tempnn_mean += tempnn[i];
            tempnn_std  += tempnn[i]*tempnn[i];
        }
        tempnn_mean /=9;
        tempnn_std /=9;

        tempnn_std -= tempnn_mean*tempnn_mean;
        int tempnn_count=0;
        int tempnn_val=0;
        for (i=0;i<9;i++)
        {
            if(tempnn[i] >= tempnn_mean-2*tempnn_std && tempnn[i] <= tempnn_mean + 2*tempnn_std)
            {
                tempnn_count++;
                tempnn_val +=tempnn[i];
            }
        }
        //在方位向,假设有一个统一的偏移
        temp_x=tempnn_val/tempnn_count;

        //释放空间,并重新定义变量大小
        delete[] slave;
        slave= new complex<short>[boxsize4*boxsize4];
        //////////////////////////////////////////////////////////////////////

        //在上面的基础上，进行亚象元级配准
        for(i=0;i<numboxAzm;i++)
        {
            //读取一行条的数据
            //主图像窗口
            file1.seekg(((centerx[i*numboxRng]-boxsize1/2)*Width)*sizeof(complex<short>),
                       ios::beg);
            file1.read((char*)(master_block),boxsize1*Width*sizeof(complex<short>));
            //辅图象窗口
            file2.seekg(((centerx[i*numboxRng]+temp_x-boxsize4/2)*Width)*sizeof(complex<short>),
                       ios::beg);
            file2.read((char *)(slave_block),boxsize4*Width*sizeof(complex<short>));

            //距离向德窗口
            for(j=0;j<numboxRng;j++)
            {
                //进度条
                /*
                wndProgress.StepIt();
                wndProgress.PeekAndPump();
                if (wndProgress.Cancelled()) 
                {
                    AfxMessageBox("进程被人为中断!");
                    file1.close();
                    file2.close();
                    
                    delete[] master;
                    delete[] slave;
                    delete[] mbox;
                    delete[] sbox;
                    delete[] bigslave;
                    delete[] xs;
                    delete[] ys;
                    delete[] xshift;
                    delete[] yshift;
                    delete[] fcof;

                    delete[] master_block;
                    delete[] slave_block;
                    delete[] centerx;
                    delete[] centery;
                    
                    wndProgress.DestroyWindow();                
                    return;
                }
                */

                //临时变量
                double temp;
                double temp1,temp2,temp3;
                int cc,dd;

                double msum=0;          //纪录均值
                double m2sum=0;         //纪录平方的均值
                int box=boxsize1;               //匹配窗口大小为81*81

                int ee,ff;
                double ssum=0;      //记录均值
                double s2sum=0;     //记录平方的均值
                double mssum=0;     //记录乘积的均值                    
                double maxcof=0;    //记录这一轮比较的最大的恶相关系数
                double cof=0;       //中间相关系数
                int step=(boxsize4-boxsize1)/2;
                
                //近距的情况
                if(j<=numboxAzm/3)          //添加temp_y[0]
                {
                    //1.读取主辅图像大窗口数据
                    for(int k=0;k<boxsize1;k++)
                    {
                        memcpy(master+k*boxsize1,
                               master_block+k*Width+(centery[i*numboxRng+j]-boxsize1/2),
                               sizeof(complex<short>)*boxsize1);
                    }                   
                    for(int k=0;k<boxsize4;k++)
                    {
                        memcpy(slave+k*boxsize4,
                               slave_block+k*Width+(centery[i*numboxRng+j]+temp_y[0]-boxsize4/2),
                               sizeof(complex<short>)*boxsize4);
                    }

                    //2.利用大窗口数据，计算粗配准参数
                    //////////////////////
                    for(cc=-boxsize1/2;cc<=boxsize1/2;cc++)         //hang
                    {
                        for(dd=-boxsize1/2;dd<=boxsize1/2;dd++)     //lie
                        {
                            temp = abs(complex<float>(master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].real(),master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].imag())); 
                            //sqrt(master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].real()*master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].real()
                            //     +master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].imag()*master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].imag());//
                            msum=msum+temp;
                            m2sum=m2sum+temp*temp;
                        }
                    }
                    msum=msum/(box*box);        //均值
                    m2sum=m2sum/(box*box);
                    temp2=m2sum-msum*msum;      //方差1
                
                    //求相关系数大小; 
                    for(ee=-step;ee<=step;ee++)             //hang
                    {   
                        for(ff=-step;ff<=step;ff++)         //lie
                        {
                            ssum=0;
                            s2sum=0;
                            mssum=0;
                            //计算相关参数
                            for(cc=-boxsize1/2;cc<=boxsize1/2;cc++)     //hang
                            {
                                for(dd=-boxsize1/2;dd<=boxsize1/2;dd++) //lie
                                {
                                    temp= abs(complex<float>(slave[(cc+boxsize4/2+ee)*boxsize4+(dd+boxsize4/2+ff)].real(),slave[(cc+boxsize4/2+ee)*boxsize4+(dd+boxsize4/2+ff)].imag()));
                                    //sqrt(slave[(cc+boxsize4/2+ee)*boxsize4+(dd+boxsize4/2+ff)].real()*slave[(cc+boxsize4/2+ee)*boxsize4+(dd+boxsize4/2+ff)].real()
                                    //+ slave[(cc+boxsize4/2+ee)*boxsize4+(dd+boxsize4/2+ff)].imag()*slave[(cc+boxsize4/2+ee)*boxsize4+(dd+boxsize4/2+ff)].imag()); //
                                    ssum=ssum+temp;
                                    s2sum=s2sum+temp*temp;
                                    mssum=mssum+ abs(complex<float>(master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].real(),master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].imag()))*temp;
                                    //sqrt(master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].real()*master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].real()
                                    //+master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].imag()*master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].imag())*temp;  //abs(master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2])
                                }
                            }
                            ssum=ssum/(box*box);
                            s2sum=s2sum/(box*box);
                            mssum=mssum/(box*box);
                            
                            temp3=s2sum-ssum*ssum;      //方差2
                            temp1=mssum-msum*ssum;      //协方差
                            //计算相关系数
                            if(temp2*temp3==0)
                            {
                                cof=0;
                            }
                            else
                            {
                                cof=fabs(temp1)/sqrt(temp2*temp3);
                            }
                            //找到最合适的位置
                            if(maxcof<cof)
                            {
                                maxcof=cof;
                                xs[i*numboxRng+j]=ee;   //hang  
                                ys[i*numboxRng+j]=ff;   //lie   
                            }
                            if(maxcof==0)               //如果maxcof=0，说明没有改变，不发生偏移
                            {
                                xs[i*numboxRng+j]=0;    //hang  
                                ys[i*numboxRng+j]=0;    //lie   
                            }
                        }
                    }
                }
                //中距
                else if(j<=numboxAzm/3*2)       //添加temp_y[1]
                {
                    //1.读取主辅图像大窗口数据
                    for(int k=0;k<boxsize1;k++)
                    {
                        memcpy(master+k*boxsize1,
                               master_block+k*Width+(centery[i*numboxRng+j]-boxsize1/2),
                               sizeof(complex<short>)*boxsize1);
                    }                   
                    for(int k=0;k<boxsize4;k++)
                    {
                        memcpy(slave+k*boxsize4,
                               slave_block+k*Width+(centery[i*numboxRng+j]+temp_y[1]-boxsize4/2),
                               sizeof(complex<short>)*boxsize4);
                    }
                    //2.利用大窗口数据，计算粗配准参数
                    //////////////////////
                    for(cc=-boxsize1/2;cc<=boxsize1/2;cc++)         //hang
                    {
                        for(dd=-boxsize1/2;dd<=boxsize1/2;dd++)     //lie
                        {
                            temp = abs(complex<float>(master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].real(),master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].imag()));
                            //sqrt(master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].real()*master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].real()
                            //+ master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].imag()*master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].imag()); //
                            msum=msum+temp;
                            m2sum=m2sum+temp*temp;
                        }
                    }
                    msum=msum/(box*box);        //均值
                    m2sum=m2sum/(box*box);
                    temp2=m2sum-msum*msum;  //方差1
                    
                    //求相关系数大小; 
                    for(ee=-step;ee<=step;ee++)             //hang
                    {   
                        for(ff=-step;ff<=step;ff++)         //lie
                        {
                            ssum=0;
                            s2sum=0;
                            mssum=0;
                            //参数计算
                            for(cc=-boxsize1/2;cc<=boxsize1/2;cc++)     //hang
                            {
                                for(dd=-boxsize1/2;dd<=boxsize1/2;dd++) //lie
                                {
                                    temp= abs(complex<float>(slave[(cc+boxsize4/2+ee)*boxsize4+(dd+boxsize4/2+ff)].real(),slave[(cc+boxsize4/2+ee)*boxsize4+(dd+boxsize4/2+ff)].imag()));
                                    //sqrt(slave[(cc+boxsize4/2+ee)*boxsize4+(dd+boxsize4/2+ff)].real()*slave[(cc+boxsize4/2+ee)*boxsize4+(dd+boxsize4/2+ff)].real()
                                    //+ slave[(cc+boxsize4/2+ee)*boxsize4+(dd+boxsize4/2+ff)].imag()*slave[(cc+boxsize4/2+ee)*boxsize4+(dd+boxsize4/2+ff)].imag());//
                                    ssum=ssum+temp;
                                    s2sum=s2sum+temp*temp;
                                    mssum=mssum+ abs(complex<float>(master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].real(),master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].imag()))*temp;
                                    //sqrt(master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].real()*master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].real()
                                    //+ master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].imag()*master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].imag())*temp;  //
                                }
                            }
                            ssum=ssum/(box*box);
                            s2sum=s2sum/(box*box);
                            mssum=mssum/(box*box);
                            
                            temp3=s2sum-ssum*ssum;      //方差2
                            temp1=mssum-msum*ssum;      //协方差
                            //计算相关系数
                            if(temp2*temp3==0)
                            {
                                cof=0;
                            }
                            else
                            {
                                cof=fabs(temp1)/sqrt(temp2*temp3);
                            }
                            //找到最合适的位置
                            if(maxcof<cof)
                            {
                                maxcof=cof;
                                xs[i*numboxRng+j]=ee;           //hang  
                                ys[i*numboxRng+j]=ff;   //lie   
                            }
                            if(maxcof==0)           //如果maxcof=0，说明没有改变，不发生偏移
                            {
                                xs[i*numboxRng+j]=0;            //hang  
                                ys[i*numboxRng+j]=0;    //lie   
                            }
                        }
                    }
                }
                //远距
                else if(j<numboxAzm)        //添加temp_y[2]
                {
                    //1.读取主辅图像大窗口数据
                    for(int k=0;k<boxsize1;k++)
                    {
                        memcpy(master+k*boxsize1,
                               master_block+k*Width+(centery[i*numboxRng+j]-boxsize1/2),
                               sizeof(complex<short>)*boxsize1);
                    }                   
                    for(int k=0;k<boxsize4;k++)
                    {
                        memcpy(slave+k*boxsize4,
                               slave_block+k*Width+(centery[i*numboxRng+j]+temp_y[2]-boxsize4/2),
                               sizeof(complex<short>)*boxsize4);
                    }
                    //2.利用大窗口数据，计算粗配准参数
                    //////////////////////
                    for(cc=-boxsize1/2;cc<=boxsize1/2;cc++)         //hang
                    {
                        for(dd=-boxsize1/2;dd<=boxsize1/2;dd++)     //lie
                        {
                            temp= abs(complex<float>(master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].real(),master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].imag()));
                            //sqrt(master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].real()*master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].real()
                            //+ master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].imag()*master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].imag());//
                            msum=msum+temp;
                            m2sum=m2sum+temp*temp;
                        }
                    }
                    msum=msum/(box*box);        //均值
                    m2sum=m2sum/(box*box);
                    temp2=m2sum-msum*msum;  //方差1
                    
                    //求相关系数大小; 
                    for(ee=-step;ee<=step;ee++)             //hang
                    {   
                        for(ff=-step;ff<=step;ff++)         //lie
                        {
                            ssum=0;
                            s2sum=0;
                            mssum=0;
                            //计算相关参数
                            for(cc=-boxsize1/2;cc<=boxsize1/2;cc++)     //hang
                            {
                                for(dd=-boxsize1/2;dd<=boxsize1/2;dd++) //lie
                                {
                                    temp= abs(complex<float>(slave[(cc+boxsize4/2+ee)*boxsize4+(dd+boxsize4/2+ff)].real(),slave[(cc+boxsize4/2+ee)*boxsize4+(dd+boxsize4/2+ff)].imag()));
                                    //sqrt(slave[(cc+boxsize4/2+ee)*boxsize4+(dd+boxsize4/2+ff)].real()*slave[(cc+boxsize4/2+ee)*boxsize4+(dd+boxsize4/2+ff)].real()
                                    //+ slave[(cc+boxsize4/2+ee)*boxsize4+(dd+boxsize4/2+ff)].imag()*slave[(cc+boxsize4/2+ee)*boxsize4+(dd+boxsize4/2+ff)].imag());//
                                    ssum=ssum+temp;
                                    s2sum=s2sum+temp*temp;
                                    mssum=mssum+abs(complex<float>(master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].real(),master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].imag()))*temp;
                                    //sqrt(master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].real()*master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].real()
                                    //+ master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].imag()*master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].imag())*temp;      //
                                }
                            }
                            ssum=ssum/(box*box);
                            s2sum=s2sum/(box*box);
                            mssum=mssum/(box*box);
                            
                            temp3=s2sum-ssum*ssum;      //方差2
                            temp1=mssum-msum*ssum;      //协方差
                            //计算相关系数
                            if(temp2*temp3==0)
                            {
                                cof=0;
                            }
                            else
                            {
                                cof=fabs(temp1)/sqrt(temp2*temp3);
                            }
                            //找到最合适的位置
                            if(maxcof<cof)
                            {
                                maxcof=cof;
                                xs[i*numboxRng+j]=ee;           //hang  
                                ys[i*numboxRng+j]=ff;   //lie   
                            }
                            if(maxcof==0)           //如果maxcof=0，说明没有改变，不发生偏移
                            {
                                xs[i*numboxRng+j]=0;            //hang  
                                ys[i*numboxRng+j]=0;    //lie   
                            }
                        }
                    }
                }
                /////////////////////
                //3.在上面的基础上计算精配准偏移:分别读取小窗口数据
                //抽取辅图像窗口数据: 83*83
                for(cc=-boxsize2/2;cc<=boxsize2/2;cc++)
                {
                    for(dd=-boxsize2/2;dd<=boxsize2/2;dd++)
                    {
                        sbox[(cc+boxsize2/2)*boxsize2+dd+boxsize2/2]=complex<float>(
                            slave[(cc+boxsize4/2 +xs[i*numboxRng+j])*boxsize4+dd+boxsize4/2 +ys[i*numboxRng+j]].real(),
                            slave[(cc+boxsize4/2 +xs[i*numboxRng+j])*boxsize4+dd+boxsize4/2 +ys[i*numboxRng+j]].imag());
                    }
                }

                //抽取主图像窗口数据
                int newbox=boxsize1;                //新窗口大小为62
                temp1=0;
                temp2=0;
                temp3=0;
                //
                msum=0;
                m2sum=0;
                for(cc=-boxsize1/2;cc<=boxsize1/2;cc++)
                {
                    for(dd=-boxsize1/2;dd<=boxsize1/2;dd++)
                    {
                        temp = abs(complex<float>(master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].real(),master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].imag()));
                        //sqrt(master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].real()*master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].real()+
                        //  master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].imag()*master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].imag());//
                        mbox[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2]=temp;
                        msum=msum+temp;
                        m2sum=m2sum+temp*temp;
                    }
                }
                msum=msum/(newbox*newbox);
                m2sum=m2sum/(newbox*newbox);
                temp2=m2sum-msum*msum;      //主图像匹配窗口的方差：temp2
                ////////////////////////////////
                //采用双线性插值法, 而不是频域滤波处理 进行扩展
                memset(bigslave,0,sizeof(float)*boxsize2*boxsize2*8*8);     //置零  
                //
                for(cc=0;cc<boxsize2*8;cc++)
                {
                    for(dd=0;dd<boxsize2*8;dd++)
                    {
                        int lowx=dd/8;      //列
                        int lowy=cc/8;      //行
                        //
                        float xx= float(dd)/8-lowx; //lie
                        float yy= float(cc)/8-lowy; //hang
                        //
                        if(lowx < boxsize2-1 && lowy < boxsize2-1)
                        {

                            bigslave[cc*boxsize2*8+dd]=abs(sbox[lowy*boxsize2+lowx]*(1-xx)*(1-yy)
                                                           +sbox[lowy*boxsize2+lowx+1]*xx*(1-yy)
                                                           +sbox[(lowy+1)*boxsize2+lowx]*(1-xx)*yy
                                                           +sbox[(lowy+1)*boxsize2+lowx+1]*xx*yy);
                        }
                        if(cc==(boxsize2-1)*8)      //最后一行
                        {
                            bigslave[cc*boxsize2*8+dd]=abs(sbox[lowy*boxsize2+lowx]);
                        }
                        if(dd==(boxsize2-1)*8)
                        {
                            bigslave[cc*boxsize2*8+dd]=abs(sbox[lowy*boxsize2+lowx]);
                        }
                    }
                }
                //          
                //求相关系数
                double xsh,ysh;
                maxcof=0;
                cof=0;
                int k,t;
                //
                for(xsh=-8;xsh<=8;xsh++)
                {
                    for(ysh=-8;ysh<=8;ysh++)
                    {
                        temp1=0;
                        temp3=0;

                        double ssum=0;      //记录均值
                        double s2sum=0;     //记录平方的均值
                        double mssum=0;     //记录乘积的均值
                        
                        //从fineTDresult中提取62*62的数据区出来
                        int p,q;
                        p=0;
                        for(k=xsh+8;k<xsh+8+boxsize1*8;k=k+8)
                        {
                            q=0;
                            for(t=ysh+8;t<ysh+8+boxsize1*8;t=t+8)
                            {
                                temp=bigslave[k*boxsize2*8+t];
                                ssum=ssum+temp;
                                s2sum=s2sum+temp*temp;
                                mssum=mssum+temp*mbox[p*boxsize1+q];

                                q++;
                            }
                            p++;
                        }
                        ssum=ssum/(newbox*newbox);
                        s2sum=s2sum/(newbox*newbox);
                        mssum=mssum/(newbox*newbox);
                        //计算temp3和temp1
                        temp3=s2sum-ssum*ssum;          //辅图像窗口的方差
                        temp1=mssum-msum*ssum;          //主辅窗口的协方差
                        //
                        if(temp2*temp3==0)
                        {
                            cof=0;
                        }
                        else
                        {
                            cof=fabs(temp1)/sqrt(temp2*temp3);
                        }
                        //
                        if(maxcof<cof)      //有偏移的情况
                        {
                            maxcof=cof;
                            xshift[i*numboxRng+j]=temp_x+xs[i*numboxRng+j]+xsh/8;
                            yshift[i*numboxRng+j]=ys[i*numboxRng+j]+ysh/8;
                            if(j<=numboxAzm/3) yshift[i*numboxRng+j]+=temp_y[0];
                            else if(j<=numboxAzm/3*2) yshift[i*numboxRng+j]+=temp_y[1];
                            else if(j<numboxAzm) yshift[i*numboxRng+j]+=temp_y[2];
                        }
                        if(maxcof==0)       //没有偏移的情况
                        {
                            xshift[i*numboxRng+j]=temp_x+xs[i*numboxRng+j];
                            yshift[i*numboxRng+j]=ys[i*numboxRng+j];
                            if(j<=numboxAzm/3) yshift[i*numboxRng+j]+=temp_y[0];
                            else if(j<=numboxAzm/3*2) yshift[i*numboxRng+j]+=temp_y[1];
                            else if(j<numboxAzm) yshift[i*numboxRng+j]+=temp_y[2];
                        }                   
                    }
                }
                fcof[i*numboxRng+j]=maxcof;                 //每个窗的最佳匹配位置的相干fcof值
            }
        }
        delete[] master;
        delete[] slave;

        delete[] master_block;
        delete[] slave_block;
    }
// #pragma endregion C short Regis

// #pragma region cFloat Regis
    else if(eCFLOAT32 == mdatatype)
    {
        printf("ok, eCFLOAT32 == mdatatype\n");
        short dSize = sizeof(complex<float>);
        complex<float>*master=new complex<float>[boxsize1*boxsize1];    //主图像窗口81*81
        complex<float>*slave= new complex<float>[boxsize3*boxsize3];    //辅图像窗口101*87

        complex<float>*master_block=new complex<float>[boxsize1*Width]; //主图像窗口81*Width
        complex<float>*slave_block= new complex<float>[boxsize3*Width]; //辅图像窗口87*Width
        //

        //下面的代码是为了防止近距，中距，和远距在距离向的偏移像素不同而设计的；
        //在近距，中距，远距分别选三个窗口计算相关系数(设置较大的搜索范围)；分别确定
        //近距，中距，和远距的距离向偏移量(像素级);至于方位向偏移的估计，则是对所有9个
        //窗口的方位结果进行统计考虑，提取2倍标准差偏移的均值作为方位向初始偏移。
        //2007-02-07, 吴涛,雷达微波遥感组

        //初始化临时变量
        int tempnn[9];
        for( i=0;i<9;i++)
            tempnn[i]=0;

        //在列的方向上也是考虑3个窗口
        for(i=0;i<3;i++)
        {
            //初始化临时变量
            int z1[3];          
            float z_cof[3];
            z1[0]=0;z1[1]=0;z1[2]=0;

            //在行的方向上考虑3个窗口
            for(j=0;j<3;j++)
            {

                //确定窗口的位置
                int tempstep_a=numboxAzm/3; //近似3等分的位置
                int tempstep_r=numboxRng/3; //近似3等分的位置
                //读取主图像窗口数据
                file1.seekg(((centerx[(j*tempstep_a+2)*numboxRng+(i+1)*tempstep_r-1]-boxsize1/2)*Width+
                            (centery[(j*tempstep_a+2)*numboxRng+(i+1)*tempstep_r-1]-boxsize1/2))*
                           dSize,
                           ios::beg);
                for(int k=0;k<boxsize1;k++)
                {
                    file1.read((char *)(master+k*boxsize1),boxsize1*dSize);
                    file1.seekg((Width-boxsize1)*dSize,ios::cur);
                }
                //读取辅图像窗口数据
                file2.seekg(((centerx[(j*tempstep_a+2)*numboxRng+(i+1)*tempstep_r-1]-boxsize3/2)*Width+
                            (centery[(j*tempstep_a+2)*numboxRng+(i+1)*tempstep_r-1]-boxsize3/2))*
                           sizeof(complex<float>),
                           ios::beg);
                for(int k=0;k<boxsize3;k++)
                {
                    file2.read((char *)(slave+k*boxsize3),boxsize3*sizeof(complex<float>));
                    file2.seekg((Width-boxsize3)*sizeof(complex<float>),ios::cur);
                }

                //2.利用大窗口数据，计算粗配准参数
                double temp;
                double temp1,temp2,temp3;
                int cc,dd;
                //
                double msum=0;          //纪录均值
                double m2sum=0;         //纪录平方的均值
                int box=boxsize1;               //匹配窗口大小为81*81
                for(cc=-boxsize1/2;cc<=boxsize1/2;cc++)         //hang
                {
                    for(dd=-boxsize1/2;dd<=boxsize1/2;dd++)     //lie
                    {
                        temp = abs(master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2]);
                        //temp = abs(complex<float>(master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].real(),master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].imag()));
                        
                        msum=msum+temp;
                        m2sum=m2sum+temp*temp;
                    }
                }
                msum=msum/(box*box);        //均值
                m2sum=m2sum/(box*box);
                temp2=m2sum-msum*msum;      //方差1

                //求相关系数大小; 
                int ee,ff;
                double ssum=0;      //记录均值
                double s2sum=0;     //记录平方的均值
                double mssum=0;     //记录乘积的均值                    
                double maxcof=0;    //记录这一轮比较的最大的相关系数
                double cof=0;       //中间相关系数

                int step=(boxsize3-boxsize1)/2;         //两窗口之间的偏移量
                for(ee=-step;ee<=step;ee++)             //hang
                {   
                    for(ff=-step;ff<=step;ff++)         //lie
                    {
                        ssum=0;
                        s2sum=0;
                        mssum=0;
                        //计算参数
                        for(cc=-boxsize1/2;cc<=boxsize1/2;cc++)     //hang
                        {
                            for(dd=-boxsize1/2;dd<=boxsize1/2;dd++) //lie
                            {
                                temp = abs(slave[(cc+boxsize3/2+ee)*boxsize3+(dd+boxsize3/2+ff)]);
                                //temp= abs(complex<float>(slave[(cc+boxsize3/2+ee)*boxsize3+(dd+boxsize3/2+ff)].real(),slave[(cc+boxsize3/2+ee)*boxsize3+(dd+boxsize3/2+ff)].imag()));
                                    
                                ssum=ssum+temp;
                                s2sum=s2sum+temp*temp;
                                mssum = mssum + abs(master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2])*temp;
                                //mssum=mssum+abs(complex<float>(master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].real(),master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].imag()))*temp;                                    
                            }
                        }
                        ssum=ssum/(box*box);
                        s2sum=s2sum/(box*box);
                        mssum=mssum/(box*box);
                        
                        temp3=s2sum-ssum*ssum;      //方差2
                        temp1=mssum-msum*ssum;      //协方差
                        //计算相关系数
                        if(temp2*temp3==0)
                        {
                            cof=0;
                        }
                        else
                        {
                            cof=fabs(temp1)/sqrt(temp2*temp3);
                        }
                        //找到最合适的配准位置
                        if(maxcof<cof)
                        {
                            maxcof=cof;
                            tempnn[i*3+j]=ee;   //hang  
                            z1[j]=ff;           //lie   
                        }
                        if(maxcof==0)           //如果maxcof=0，说明没有改变，不发生偏移
                        {
                            tempnn[i*3+j]=0;    //hang  
                            z1[j]=0;            //lie   
                        }
                    }
                }
                //保存最大配准位置
                z_cof[j]=maxcof;
            }//end j
            double maxtemp=z_cof[0];
            temp_y[i]=z1[0];

            //到一个最合适的距离向偏移
            for(j=0;j<3;j++)
            {
                if(z_cof[j] > maxtemp)
                {
                    maxtemp=z_cof[j];
                    temp_y[i]=z1[j];
                    //temp_x=tempnn[j];
                }
            }
            if(maxtemp < 0.1) {temp_y[i]=0;}//temp_x=0;}
            
        }
        //考虑方位向初始偏移
        double tempnn_mean=0;
        double tempnn_std=0;
        //通过均值和标准差控制误差
        for(i=0;i<9;i++)
        {
            tempnn_mean += tempnn[i];
            tempnn_std  += tempnn[i]*tempnn[i];
        }
        tempnn_mean /=9;
        tempnn_std /=9;

        tempnn_std -= tempnn_mean*tempnn_mean;
        int tempnn_count=0;
        int tempnn_val=0;
        for (i=0;i<9;i++)
        {
            if(tempnn[i] >= tempnn_mean-2*tempnn_std && tempnn[i] <= tempnn_mean + 2*tempnn_std)
            {
                tempnn_count++;
                tempnn_val +=tempnn[i];
            }
        }
        //在方位向,假设有一个统一的偏移
        temp_x=tempnn_val/tempnn_count;

        //释放空间,并重新定义变量大小
        delete[] slave;
        slave= new complex<float>[boxsize4*boxsize4];
        //////////////////////////////////////////////////////////////////////

        //在上面的基础上，进行亚象元级配准
        for(i=0;i<numboxAzm;i++)
        {
            //读取一行条的数据
            //主图像窗口
            file1.seekg(((centerx[i*numboxRng]-boxsize1/2)*Width)*dSize,ios::beg);
            file1.read((char *)master_block,boxsize1*Width*dSize);          
            //辅图象窗口
            file2.seekg(((centerx[i*numboxRng]+temp_x-boxsize4/2)*Width)*dSize,ios::beg);
            file2.read((char *)slave_block,boxsize4*Width*dSize);

            //距离向德窗口
            for(j=0;j<numboxRng;j++)
            {

                //临时变量
                double temp;
                double temp1,temp2,temp3;
                int cc,dd;

                double msum=0;          //纪录均值
                double m2sum=0;         //纪录平方的均值
                int box=boxsize1;               //匹配窗口大小为81*81

                int ee,ff;
                double ssum=0;      //记录均值
                double s2sum=0;     //记录平方的均值
                double mssum=0;     //记录乘积的均值                    
                double maxcof=0;    //记录这一轮比较的最大的恶相关系数
                double cof=0;       //中间相关系数
                int step=(boxsize4-boxsize1)/2;
                
                //近距的情况
                if(j<=numboxAzm/3)          //添加temp_y[0]
                {
                    //1.读取主辅图像大窗口数据
                    for(int k=0;k<boxsize1;k++)
                    {
                        memcpy(master+k*boxsize1,
                               master_block+k*Width+(centery[i*numboxRng+j]-boxsize1/2),
                               dSize*boxsize1);
                    }                   
                    for(int k=0;k<boxsize4;k++)
                    {
                        memcpy(slave+k*boxsize4,
                               slave_block+k*Width+(centery[i*numboxRng+j]+temp_y[0]-boxsize4/2),
                               dSize*boxsize4);
                    }

                    //2.利用大窗口数据，计算粗配准参数
                    //////////////////////
                    for(cc=-boxsize1/2;cc<=boxsize1/2;cc++)         //hang
                    {
                        for(dd=-boxsize1/2;dd<=boxsize1/2;dd++)     //lie
                        {
                            //temp = abs(complex<float>(master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].real(),master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].imag())); 
                            temp = abs(master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2]);
                            msum=msum+temp;
                            m2sum=m2sum+temp*temp;
                        }
                    }
                    msum=msum/(box*box);        //均值
                    m2sum=m2sum/(box*box);
                    temp2=m2sum-msum*msum;      //方差1
                
                    //求相关系数大小; 
                    for(ee=-step;ee<=step;ee++)             //hang
                    {   
                        for(ff=-step;ff<=step;ff++)         //lie
                        {
                            ssum=0;
                            s2sum=0;
                            mssum=0;
                            //计算相关参数
                            for(cc=-boxsize1/2;cc<=boxsize1/2;cc++)     //hang
                            {
                                for(dd=-boxsize1/2;dd<=boxsize1/2;dd++) //lie
                                {
                                    //temp= abs(complex<float>(slave[(cc+boxsize4/2+ee)*boxsize4+(dd+boxsize4/2+ff)].real(),slave[(cc+boxsize4/2+ee)*boxsize4+(dd+boxsize4/2+ff)].imag()));
                                    temp = abs(slave[(cc+boxsize4/2+ee)*boxsize4+(dd+boxsize4/2+ff)]);
                                    ssum=ssum+temp;
                                    s2sum=s2sum+temp*temp;
                                    //mssum=mssum+ abs(complex<float>(master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].real(),master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].imag()))*temp;
                                    mssum=mssum+ abs(master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2])*temp;
                                }
                            }
                            ssum=ssum/(box*box);
                            s2sum=s2sum/(box*box);
                            mssum=mssum/(box*box);
                            
                            temp3=s2sum-ssum*ssum;      //方差2
                            temp1=mssum-msum*ssum;      //协方差
                            //计算相关系数
                            if(temp2*temp3==0)
                            {
                                cof=0;
                            }
                            else
                            {
                                cof=fabs(temp1)/sqrt(temp2*temp3);
                            }
                            //找到最合适的位置
                            if(maxcof<cof)
                            {
                                maxcof=cof;
                                xs[i*numboxRng+j]=ee;   //hang  
                                ys[i*numboxRng+j]=ff;   //lie   
                            }
                            if(maxcof==0)               //如果maxcof=0，说明没有改变，不发生偏移
                            {
                                xs[i*numboxRng+j]=0;    //hang  
                                ys[i*numboxRng+j]=0;    //lie   
                            }
                        }
                    }
                }
                //中距
                else if(j<=numboxAzm/3*2)       //添加temp_y[1]
                {
                    //1.读取主辅图像大窗口数据
                    for(int k=0;k<boxsize1;k++)
                    {
                        memcpy(master+k*boxsize1,
                               master_block+k*Width+(centery[i*numboxRng+j]-boxsize1/2),
                               dSize*boxsize1);
                    }                   
                    for(int k=0;k<boxsize4;k++)
                    {
                        memcpy(slave+k*boxsize4,
                               slave_block+k*Width+(centery[i*numboxRng+j]+temp_y[1]-boxsize4/2),
                               dSize*boxsize4);
                    }
                    //2.利用大窗口数据，计算粗配准参数
                    //////////////////////
                    for(cc=-boxsize1/2;cc<=boxsize1/2;cc++)         //hang
                    {
                        for(dd=-boxsize1/2;dd<=boxsize1/2;dd++)     //lie
                        {
                            //temp = abs(complex<float>(master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].real(),master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].imag()));
                            temp = abs(master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2]);
                            msum=msum+temp;
                            m2sum=m2sum+temp*temp;
                        }
                    }
                    msum=msum/(box*box);        //均值
                    m2sum=m2sum/(box*box);
                    temp2=m2sum-msum*msum;  //方差1
                    
                    //求相关系数大小; 
                    for(ee=-step;ee<=step;ee++)             //hang
                    {   
                        for(ff=-step;ff<=step;ff++)         //lie
                        {
                            ssum=0;
                            s2sum=0;
                            mssum=0;
                            //参数计算
                            for(cc=-boxsize1/2;cc<=boxsize1/2;cc++)     //hang
                            {
                                for(dd=-boxsize1/2;dd<=boxsize1/2;dd++) //lie
                                {
                                    //temp= abs(complex<float>(slave[(cc+boxsize4/2+ee)*boxsize4+(dd+boxsize4/2+ff)].real(),slave[(cc+boxsize4/2+ee)*boxsize4+(dd+boxsize4/2+ff)].imag()));
                                    temp = abs(slave[(cc+boxsize4/2+ee)*boxsize4+(dd+boxsize4/2+ff)]);  
                                    ssum=ssum+temp;
                                    s2sum=s2sum+temp*temp;
                                    //mssum=mssum+ abs(complex<float>(master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].real(),master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].imag()))*temp;
                                    mssum = mssum + abs(master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2])*temp;
                                }
                            }
                            ssum=ssum/(box*box);
                            s2sum=s2sum/(box*box);
                            mssum=mssum/(box*box);
                            
                            temp3=s2sum-ssum*ssum;      //方差2
                            temp1=mssum-msum*ssum;      //协方差
                            //计算相关系数
                            if(temp2*temp3==0)
                            {
                                cof=0;
                            }
                            else
                            {
                                cof=fabs(temp1)/sqrt(temp2*temp3);
                            }
                            //找到最合适的位置
                            if(maxcof<cof)
                            {
                                maxcof=cof;
                                xs[i*numboxRng+j]=ee;           //hang  
                                ys[i*numboxRng+j]=ff;   //lie   
                            }
                            if(maxcof==0)           //如果maxcof=0，说明没有改变，不发生偏移
                            {
                                xs[i*numboxRng+j]=0;            //hang  
                                ys[i*numboxRng+j]=0;    //lie   
                            }
                        }
                    }
                }
                //远距
                else if(j<numboxAzm)        //添加temp_y[2]
                {
                    //1.读取主辅图像大窗口数据
                    for(int k=0;k<boxsize1;k++)
                    {
                        memcpy(master+k*boxsize1,
                               master_block+k*Width+(centery[i*numboxRng+j]-boxsize1/2),
                               dSize*boxsize1);
                    }                   
                    for(int k=0;k<boxsize4;k++)
                    {
                        memcpy(slave+k*boxsize4,
                               slave_block+k*Width+(centery[i*numboxRng+j]+temp_y[2]-boxsize4/2),
                               dSize*boxsize4);
                    }
                    //2.利用大窗口数据，计算粗配准参数
                    //////////////////////
                    for(cc=-boxsize1/2;cc<=boxsize1/2;cc++)         //hang
                    {
                        for(dd=-boxsize1/2;dd<=boxsize1/2;dd++)     //lie
                        {
                            //temp= abs(complex<float>(master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].real(),master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].imag()));
                            temp = abs(master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2]);
                            msum=msum+temp;
                            m2sum=m2sum+temp*temp;
                        }
                    }
                    msum=msum/(box*box);        //均值
                    m2sum=m2sum/(box*box);
                    temp2=m2sum-msum*msum;  //方差1
                    
                    //求相关系数大小; 
                    for(ee=-step;ee<=step;ee++)             //hang
                    {   
                        for(ff=-step;ff<=step;ff++)         //lie
                        {
                            ssum=0;
                            s2sum=0;
                            mssum=0;
                            //计算相关参数
                            for(cc=-boxsize1/2;cc<=boxsize1/2;cc++)     //hang
                            {
                                for(dd=-boxsize1/2;dd<=boxsize1/2;dd++) //lie
                                {
                                    //temp= abs(complex<float>(slave[(cc+boxsize4/2+ee)*boxsize4+(dd+boxsize4/2+ff)].real(),slave[(cc+boxsize4/2+ee)*boxsize4+(dd+boxsize4/2+ff)].imag()));
                                    temp =abs(slave[(cc+boxsize4/2+ee)*boxsize4+(dd+boxsize4/2+ff)]);
                                    ssum=ssum+temp;
                                    s2sum=s2sum+temp*temp;
                                    //mssum=mssum+abs(complex<float>(master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].real(),master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].imag()))*temp;
                                    mssum = mssum + abs(master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2]);
                                }
                            }
                            ssum=ssum/(box*box);
                            s2sum=s2sum/(box*box);
                            mssum=mssum/(box*box);
                            
                            temp3=s2sum-ssum*ssum;      //方差2
                            temp1=mssum-msum*ssum;      //协方差
                            //计算相关系数
                            if(temp2*temp3==0)
                            {
                                cof=0;
                            }
                            else
                            {
                                cof=fabs(temp1)/sqrt(temp2*temp3);
                            }
                            //找到最合适的位置
                            if(maxcof<cof)
                            {
                                maxcof=cof;
                                xs[i*numboxRng+j]=ee;           //hang  
                                ys[i*numboxRng+j]=ff;   //lie   
                            }
                            if(maxcof==0)           //如果maxcof=0，说明没有改变，不发生偏移
                            {
                                xs[i*numboxRng+j]=0;            //hang  
                                ys[i*numboxRng+j]=0;    //lie   
                            }
                        }
                    }
                }
                /////////////////////
                //3.在上面的基础上计算精配准偏移:分别读取小窗口数据
                //抽取辅图像窗口数据: 83*83
                for(cc=-boxsize2/2;cc<=boxsize2/2;cc++)
                {
                    for(dd=-boxsize2/2;dd<=boxsize2/2;dd++)
                    {
                        //sbox[(cc+boxsize2/2)*boxsize2+dd+boxsize2/2]=complex<float>(
                        //                          slave[(cc+boxsize4/2 +xs[i*numboxRng+j])*boxsize4+dd+boxsize4/2 +ys[i*numboxRng+j]].real(),
                        //                          slave[(cc+boxsize4/2 +xs[i*numboxRng+j])*boxsize4+dd+boxsize4/2 +ys[i*numboxRng+j]].imag());
                        sbox[(cc+boxsize2/2)*boxsize2+dd+boxsize2/2]= slave[(cc+boxsize4/2 +xs[i*numboxRng+j])*boxsize4+dd+boxsize4/2 +ys[i*numboxRng+j]];
                    }
                }

                //抽取主图像窗口数据
                int newbox=boxsize1;                //新窗口大小为62
                temp1=0;
                temp2=0;
                temp3=0;
                //
                msum=0;
                m2sum=0;
                for(cc=-boxsize1/2;cc<=boxsize1/2;cc++)
                {
                    for(dd=-boxsize1/2;dd<=boxsize1/2;dd++)
                    {
                        //temp = abs(complex<float>(master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].real(),master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2].imag()));
                        temp = abs(master[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2]);
                        mbox[(cc+boxsize1/2)*boxsize1+dd+boxsize1/2]=temp;
                        msum=msum+temp;
                        m2sum=m2sum+temp*temp;
                    }
                }
                msum=msum/(newbox*newbox);
                m2sum=m2sum/(newbox*newbox);
                temp2=m2sum-msum*msum;      //主图像匹配窗口的方差：temp2
                ////////////////////////////////
                //采用双线性插值法, 而不是频域滤波处理 进行扩展
                memset(bigslave,0,sizeof(float)*boxsize2*boxsize2*8*8);     //置零  
                //
                for(cc=0;cc<boxsize2*8;cc++)
                {
                    for(dd=0;dd<boxsize2*8;dd++)
                    {
                        int lowx=dd/8;      //列
                        int lowy=cc/8;      //行
                        //
                        float xx= float(dd)/8-lowx; //lie
                        float yy= float(cc)/8-lowy; //hang
                        //
                        if(lowx < boxsize2-1 && lowy < boxsize2-1)
                        {

                            bigslave[cc*boxsize2*8+dd]=abs(sbox[lowy*boxsize2+lowx]*(1-xx)*(1-yy)
                                                           +sbox[lowy*boxsize2+lowx+1]*xx*(1-yy)
                                                           +sbox[(lowy+1)*boxsize2+lowx]*(1-xx)*yy
                                                           +sbox[(lowy+1)*boxsize2+lowx+1]*xx*yy);
                        }
                        if(cc==(boxsize2-1)*8)      //最后一行
                        {
                            bigslave[cc*boxsize2*8+dd]=abs(sbox[lowy*boxsize2+lowx]);
                        }
                        if(dd==(boxsize2-1)*8)
                        {
                            bigslave[cc*boxsize2*8+dd]=abs(sbox[lowy*boxsize2+lowx]);
                        }
                    }
                }
                //          
                //求相关系数
                double xsh,ysh;
                maxcof=0;
                cof=0;
                int k,t;
                //
                for(xsh=-8;xsh<=8;xsh++)
                {
                    for(ysh=-8;ysh<=8;ysh++)
                    {
                        temp1=0;
                        temp3=0;

                        double ssum=0;      //记录均值
                        double s2sum=0;     //记录平方的均值
                        double mssum=0;     //记录乘积的均值
                        
                        //从fineTDresult中提取62*62的数据区出来
                        int p,q;
                        p=0;
                        for(k=xsh+8;k<xsh+8+boxsize1*8;k=k+8)
                        {
                            q=0;
                            for(t=ysh+8;t<ysh+8+boxsize1*8;t=t+8)
                            {
                                temp=bigslave[k*boxsize2*8+t];
                                ssum=ssum+temp;
                                s2sum=s2sum+temp*temp;
                                mssum=mssum+temp*mbox[p*boxsize1+q];

                                q++;
                            }
                            p++;
                        }
                        ssum=ssum/(newbox*newbox);
                        s2sum=s2sum/(newbox*newbox);
                        mssum=mssum/(newbox*newbox);
                        //计算temp3和temp1
                        temp3=s2sum-ssum*ssum;          //辅图像窗口的方差
                        temp1=mssum-msum*ssum;          //主辅窗口的协方差
                        //
                        if(temp2*temp3==0)
                        {
                            cof=0;
                        }
                        else
                        {
                            cof=fabs(temp1)/sqrt(temp2*temp3);
                        }
                        //
                        if(maxcof<cof)      //有偏移的情况
                        {
                            maxcof=cof;
                            xshift[i*numboxRng+j]=temp_x+xs[i*numboxRng+j]+xsh/8;
                            yshift[i*numboxRng+j]=ys[i*numboxRng+j]+ysh/8;
                            if(j<=numboxAzm/3) yshift[i*numboxRng+j]+=temp_y[0];
                            else if(j<=numboxAzm/3*2) yshift[i*numboxRng+j]+=temp_y[1];
                            else if(j<numboxAzm) yshift[i*numboxRng+j]+=temp_y[2];
                        }
                        if(maxcof==0)       //没有偏移的情况
                        {
                            xshift[i*numboxRng+j]=temp_x+xs[i*numboxRng+j];
                            yshift[i*numboxRng+j]=ys[i*numboxRng+j];
                            if(j<=numboxAzm/3) yshift[i*numboxRng+j]+=temp_y[0];
                            else if(j<=numboxAzm/3*2) yshift[i*numboxRng+j]+=temp_y[1];
                            else if(j<numboxAzm) yshift[i*numboxRng+j]+=temp_y[2];
                        }                   
                    }
                }
                fcof[i*numboxRng+j]=maxcof;                 //每个窗的最佳匹配位置的相干fcof值
            }
        }
        delete[] master;
        delete[] slave;

        delete[] master_block;
        delete[] slave_block;

    }
// #pragma endregion cFloat Regis
    //关闭文件
    file1.close();
    file2.close();

    delete[] bigslave;
    //
    delete[] xs;
    delete[] ys;
    delete[] mbox;
    delete[] sbox;

    //找配准系数
    double meanfcof=0;          //fcof的平均值
    double stdvarfcof=0.0;      //fcof的标准差
    int tempn=0;
    for(i=0;i<numboxAzm;i++)
    {
        for(j=0;j<numboxRng;j++)
        {
            if(fcof[i*numboxRng+j] <1 && fcof[i*numboxRng+j] > 1e-6)    //大于0，小于1 
            {
                meanfcof   += fcof[i*numboxRng+j];
                stdvarfcof += fcof[i*numboxRng+j]*fcof[i*numboxRng+j];
                tempn++;
            }
        }
    }
    meanfcof=meanfcof/tempn;
    //if(meanfcof<0.1) meanfcof=0.1;

    stdvarfcof=stdvarfcof/tempn;
    stdvarfcof -=meanfcof*meanfcof; //方差
    stdvarfcof = sqrt(stdvarfcof);  //标准差

    int num=0;                          //大于(均值+标准差) 的个数
    for(i=0;i<numboxAzm*numboxRng;i++)
    {
        if(meanfcof+stdvarfcof<fcof[i] && fcof[i] <1 )//
            num=num+1;
    }

    float *basePosX,*basePosY,*slavePosX,*slavePosY;    //X：列方向，Y：行方向
    basePosX  = new float[num];
    basePosY  = new float[num];
    slavePosX = new float[num];
    slavePosY = new float[num];

    float *tempCof = new float[num];
    int pointI=0;
    double azimuth=0;
    for(int i=0;i<numboxAzm;i++)
    {
        for(int j=0;j<numboxRng;j++)
        {
            if(fcof[i*numboxRng+j]>meanfcof+stdvarfcof && fcof[i*numboxRng+j] <1  )//
            {
                slavePosX[pointI] = centery[i*numboxRng+j]+yshift[i*numboxRng+j];       ////X: lie,Y:hang; x;hang y lie
                slavePosY[pointI] = centerx[i*numboxRng+j]+xshift[i*numboxRng+j];   //

                basePosX[pointI]= centery[i*numboxRng+j]; 
                basePosY[pointI]= centerx[i*numboxRng+j]; 

                tempCof[pointI] = fcof[i*numboxRng+j];

                azimuth=azimuth+xshift[i*numboxRng+j];      //准备计算平均方位偏移
                pointI++;
            }
        }       
    }

    azimuth=azimuth/num;            //平均方位偏移
    azimuth=azimuth+shift_azm;      //新的方位向偏移参数

    CSpMatrix<double> colCoef(6,1);
    CSpMatrix<double> rowCoef(6,1);
    cout << "enter Get_para_gcp\n";
    Get_para_gcp(basePosX,basePosY,slavePosX,slavePosY,colCoef,rowCoef,pointI);
    cout << "exit Get_para_gcp\n";
    delete[] basePosX;
    delete[] basePosY;
    delete[] slavePosX;
    delete[] slavePosY;

    delete[]xshift;
    delete[]yshift;
    delete[]fcof;
    delete[] centerx;
    delete[] centery;//



    cout << "enter Resample\n";
    Resample(lpDataIn1,lpHdrIn1,lpDataIn2,lpHdrIn2,
             lpDataOut,lpHdrOut,colCoef,rowCoef,azimuth);
    cout << "exit Resample\n";

    cout << "enter ReSampleImg_Master\n";
    ReSampleImg_Master(lpDataIn1,lpHdrIn1,lpDataOutM,lpHdrOutM,Width,Height,mImg.m_oHeader.DataType);
    cout << "exit ReSampleImg_Master\n";



    return;
}//void Fine

/************************************
 *X:column; Y:row
 *
 *
 *
 *
 *
 *******************************************/
void CRegistrFine::Get_para_gcp(float *basePosX,float *basePosY,float *slavePosX,float *slavePosY,CSpMatrix<double> &colCoef,CSpMatrix<double> &rowCoef,int pointN)
{
    const int coefNo =6;
    //
    //CSpMatrix<double> a1(coefNo,coefNo);
    //CSpMatrix<double> a2(coefNo,pointN);
    //CSpMatrix<double> a3(coefNo,pointN);
    
    CSpMatrix<double> L(pointN,coefNo);
    CSpMatrix<double> Lx(pointN,1);
    CSpMatrix<double> Ly(pointN,1);

    for(int i=0;i<pointN;i++)   //a0+ a1*x + a2*y + a3*x*x + a4*x*y + a5*y*y 
    {
        L.Set(i,0,1);
        L.Set(i,1,basePosX[i]);
        L.Set(i,2,basePosY[i]);
        L.Set(i,3,basePosX[i]*basePosX[i]);
        L.Set(i,4,basePosX[i]*basePosY[i]);
        L.Set(i,5,basePosY[i]*basePosY[i]);

        Lx.Set(i,0,slavePosX[i]);
        Ly.Set(i,0,slavePosY[i]);
    }
    CSpMatrix<double> a2 = L.Transpose();   //6*N
    CSpMatrix<double> a1 = a2*L;            //6*6
    a1 = a1.Inverse();  //6*6
    CSpMatrix<double> a3 = a1*a2;           //6*N
    colCoef = a3*Lx;
    rowCoef = a3*Ly;    
    return;
}


void CRegistrFine::Resample(string lpDataIn1,string lpHdrIn1,
                            string lpDataIn2,string lpHdrIn2,
                            string lpDataOut,string lpHdrOut,
                            CSpMatrix<double> xMtxCoef,CSpMatrix<double> yMtxCoef,double newAoffset)
{
        
    CRMGImage mImg(lpDataIn1,lpHdrIn1);
    CRMGImage sImg(lpDataIn2,lpHdrIn2);
    //CSpMatrix<double> xMtxCoef(6,1);                  方位向拟合矩阵
    //CSpMatrix<double> yMtxCoef(6,1);                  距离向拟合矩阵
    CSpMatrix<double> L(1,6);
    /******test************/
    int Width= sImg.m_oHeader.Sample;
    int Height=sImg.m_oHeader.Line;
    int datatype=sImg.m_oHeader.DataType;

    /*
    CFile file1,file2;
    CFileException ex1;
    if(!file1.Open(lpDataI2,CFile::modeRead | CFile::shareDenyWrite,ex1))
    {
        TCHAR szError[1024];
        ex1.GetErrorMessage(szError, 1024);
        AfxMessageBox(szError);
        return ;
    }
    if(!file2.Open(lpDataOutCFile::modeWrite |
                   CFile::shareExclusive | CFile::modeCreate, &ex1))
    {
        TCHAR szError[1024];
        ex1.GetErrorMessage(szError,1024);
        AfxMessageBox(szError);
        return;
    }
    */



    ifstream file1(lpDataIn2, ios::in |ios::binary);
    if (!file1.is_open()) {
        cout << "error open input image 2\n";
        exit(1) ;
    }
    ofstream file2(lpDataOut, ios::out|ios::binary);
    if (!file2.is_open()) {
        cout << "error open output image \n";
        exit(1);
    }
    // CProgressBar wndProgress(Height,"正在进行辅图像重采用...");
    
    // wndProgress.SetStep(1);
    printf("ReSampling...\n");
    int i,j;
    //
    double dx,dy;
    long t1,t2,t3,t4;
    float alpha,belta;

    int delth   =   200;                    //
    int temp_H  =   600+2*delth;            //

    int temp_N  =   Height/600;         //

    int NewH=600;
    int bnum=0;

// #pragma region Resample cShort
    if(datatype==eCINT16)           //
    {       
        complex<short>*bslave   =   new complex<short>[Width*temp_H];
        complex<float>*slave1=new complex<float>[Width*NewH];

        for(bnum=0;bnum<temp_N;bnum++)  //
        {
            if(bnum < temp_N-1)         //
            {
                int shift_w=0;          
                if(bnum==0)             
                {
                    file1.read((char *)bslave,sizeof(complex<short>)*Width*(temp_H-delth));
                    
                    shift_w=bnum*600;   
                }
                else                    
                {                       
                    file1.seekg(-2*delth*Width*sizeof(complex<short>),ios::cur);
                    file1.read((char *)bslave,sizeof(complex<short>)*Width*temp_H);

                    shift_w=bnum*600-delth;
                }
                //
                for(i=bnum*600;i<(bnum+1)*600 && i< Height ;i++)                    
                {
                    /*
                    wndProgress.StepIt();
                    wndProgress.PeekAndPump();
                    if (wndProgress.Cancelled()) 
                    {
                        AfxMessageBox("quxiao!");
                        file1.close();
                        file2.close();
                        _unlink(lpDataIn2);

                        delete[] slave1;
                        delete[] bslave;                            

                        wndProgress.DestroyWindow();                
                        return;
                    }
                    */
                    int ii=i-bnum*600;

                    for(j=0;j<Width;j++)
                    {
                        //L.Set(0,0,1);L.Set(0,1,j);L.Set(0,2,i);L.Set(0,3,j*j);L.Set(0,4,i*j);L.Set(0,5,i*i);
                        dx = yMtxCoef[0]+ yMtxCoef[1]*j+yMtxCoef[2]*i+yMtxCoef[3]*j*j+yMtxCoef[4]*i*j+yMtxCoef[5]*i*i; 
                        dy = xMtxCoef[0]+ xMtxCoef[1]*j+xMtxCoef[2]*i+xMtxCoef[3]*j*j+xMtxCoef[4]*i*j+xMtxCoef[5]*i*i;

                        t1=floor(dx);t2=floor(dy);t3=t1+1;t4=t2+1;  
                        alpha=dx-t1;    //
                        belta=dy-t2;    
                                                
                        if((t1==0 && t2>=0 && t2<Width-1)||(t2==0 && t1>=0 && t1<Height-1)
                           ||(t1==Height-2 && t2>=0 && t2<Width-1)||(t2==Width-2 && t1>=0 && t1<Height-1))
                        {
                            float temp_real=(1-alpha)*(1-belta)*(bslave[(t1-shift_w)*Width+t2].real())
                                +alpha*(1-belta)*(bslave[(t3-shift_w)*Width+t2].real())+(1-alpha)*belta*(bslave[(t1-shift_w)*Width+t4].real())
                                +alpha*belta*(bslave[(t3-shift_w)*Width+t4].real());
                            float temp_imag=(1-alpha)*(1-belta)*(bslave[(t1-shift_w)*Width+t2].imag())
                                +alpha*(1-belta)*(bslave[(t3-shift_w)*Width+t2].imag())+(1-alpha)*belta*(bslave[(t1-shift_w)*Width+t4].imag())
                                +alpha*belta*(bslave[(t3-shift_w)*Width+t4].imag());
                            slave1[ii*Width+j]=complex<float>(temp_real,temp_imag);
                        }
                        else if(t1<0 || t2<0 ||
                                t1>Height-2 || t2>Width-2 ||
                                t1-shift_w < 1 || t1-shift_w > temp_H-2)
                        {
                            slave1[ii*Width+j]=0;
                        }
                        else
                        {
                            double A[4];
                            double C[4];
                            complex<float> B[4][4];
                            A[0]=4-8*(1+alpha)+5*(1+alpha)*(1+alpha)-(1+alpha)*(1+alpha)*(1+alpha);
                            A[1]=1-2*alpha*alpha+alpha*alpha*alpha;
                            A[2]=1-2*(1-alpha)*(1-alpha)+(1-alpha)*(1-alpha)*(1-alpha);
                            A[3]=4-8*(2-alpha)+5*(2-alpha)*(2-alpha)-(2-alpha)*(2-alpha)*(2-alpha);
                            C[0]=4-8*(1+belta)+5*(1+belta)*(1+belta)-(1+belta)*(1+belta)*(1+belta);
                            C[1]=1-2*belta*belta+belta*belta*belta;
                            C[2]=1-2*(1-belta)*(1-belta)+(1-belta)*(1-belta)*(1-belta);
                            C[3]=4-8*(2-belta)+5*(2-belta)*(2-belta)-(2-belta)*(2-belta)*(2-belta);
                            for(int k=0;k<4;k++)
                            {
                                for(int t=0;t<4;t++)
                                {
                                    float temp_real=bslave[(t1-shift_w-1+k)*Width+t2-1+t].real();
                                    float temp_imag=bslave[(t1-shift_w-1+k)*Width+t2-1+t].imag();
                                    B[k][t]=complex<float>(temp_real,temp_imag);
                                }
                            }
                            complex<float> D[4];
                            for(int k=0;k<4;k++)
                            {
                                D[k]=(float)A[0]*B[0][k]+(float)A[1]*B[1][k]+(float)A[2]*B[2][k]+(float)A[3]*B[3][k];
                            }
                            slave1[ii*Width+j]=D[0]*(float)C[0]+D[1]*(float)C[1]+D[2]*(float)C[2]+D[3]*(float)C[3];
                        }
                    }
                }
                file2.write((char *)(slave1), sizeof(complex<float>)*Width * NewH);
            }
            else
            {
                int shift_w=0;
                int last_H=Height-bnum*NewH+delth;  
                complex<short>*bslave_2=new complex<short>[last_H*Width];
                complex<float>*slave_2=new complex<float>[(last_H-delth)*Width];    
                if(bnum==0) 
                {
                    shift_w=0;
                    file1.read((char *)bslave_2,sizeof(complex<short>)*(Height-bnum*NewH)*Width);
                }   
                else    
                {
                    shift_w=bnum*600-delth;

                    file1.seekg(-2*delth*Width*sizeof(complex<short>),ios::cur);
                    file1.read((char *)bslave_2,sizeof(complex<short>)*last_H*Width);       
                }

                for(i=bnum*600; i< Height ;i++)
                {
                    /*
                    wndProgress.StepIt();
                    wndProgress.PeekAndPump();
                    if (wndProgress.Cancelled()) 
                    {
                        AfxMessageBox("cancel!");
                        file1.close();
                        file2.close();
                        _unlink(lpDataOut);

                        delete[] slave1;
                        delete[] bslave;

                        delete[] slave_2;
                        delete[] bslave_2;                      

                        wndProgress.DestroyWindow();                
                        return;
                    }
                    */

                    int ii=i-bnum*600;
                    for(j=0;j<Width;j++)
                    {
                        //L.Set(0,0,1);L.Set(0,1,j);L.Set(0,2,i);L.Set(0,3,j*j);L.Set(0,4,i*j);L.Set(0,5,i*i);
                        dx = yMtxCoef[0]+ yMtxCoef[1]*j+yMtxCoef[2]*i+yMtxCoef[3]*j*j+yMtxCoef[4]*i*j+yMtxCoef[5]*i*i; 
                        dy = xMtxCoef[0]+ xMtxCoef[1]*j+xMtxCoef[2]*i+xMtxCoef[3]*j*j+xMtxCoef[4]*i*j+xMtxCoef[5]*i*i;
                        
                        t1=floor(dx);t2=floor(dy);t3=t1+1;t4=t2+1;  
                        alpha=dx-t1;    
                        belta=dy-t2;                        
                        if((t1==0 && t2>=0 && t2<Width-1)||(t2==0 && t1>=0 && t1<Height-1)
                           ||(t1==Height-2 && t2>=0 && t2<Width-1)||(t2==Width-2 && t1>=0 &&t1<Height-1))
                        {
                            float temp_real=(1-alpha)*(1-belta)*(bslave_2[(t1-shift_w)*Width+t2].real())
                                +alpha*(1-belta)*(bslave_2[(t3-shift_w)*Width+t2].real())+(1-alpha)*belta*(bslave_2[(t1-shift_w)*Width+t4].real())
                                +alpha*belta*(bslave_2[(t3-shift_w)*Width+t4].real());
                            float temp_imag=(1-alpha)*(1-belta)*(bslave_2[(t1-shift_w)*Width+t2].imag())
                                +alpha*(1-belta)*(bslave_2[(t3-shift_w)*Width+t2].imag())+(1-alpha)*belta*(bslave_2[(t1-shift_w)*Width+t4].imag())
                                +alpha*belta*(bslave_2[(t3-shift_w)*Width+t4].imag());
                            slave_2[ii*Width+j]=complex<float>(temp_real,temp_imag);
                        }
                        else if(t1<0 || t2<0 || t1>Height-2 || t2>Width-2 ||
                                t1-shift_w < 1 || t1-shift_w > (last_H-delth)-2)    //temp_H
                        {
                            slave_2[ii*Width+j]=0;
                        }
                        else
                        {
                            double A[4];
                            double C[4];
                            complex<float> B[4][4];
                            A[0]=4-8*(1+alpha)+5*(1+alpha)*(1+alpha)-(1+alpha)*(1+alpha)*(1+alpha);
                            A[1]=1-2*alpha*alpha+alpha*alpha*alpha;
                            A[2]=1-2*(1-alpha)*(1-alpha)+(1-alpha)*(1-alpha)*(1-alpha);
                            A[3]=4-8*(2-alpha)+5*(2-alpha)*(2-alpha)-(2-alpha)*(2-alpha)*(2-alpha);
                            C[0]=4-8*(1+belta)+5*(1+belta)*(1+belta)-(1+belta)*(1+belta)*(1+belta);
                            C[1]=1-2*belta*belta+belta*belta*belta;
                            C[2]=1-2*(1-belta)*(1-belta)+(1-belta)*(1-belta)*(1-belta);
                            C[3]=4-8*(2-belta)+5*(2-belta)*(2-belta)-(2-belta)*(2-belta)*(2-belta);
                            for(int k=0;k<4;k++)
                            {
                                for(int t=0;t<4;t++)
                                {
                                    float temp_real=bslave_2[(t1-shift_w-1+k)*Width+t2-1+t].real();
                                    float temp_imag=bslave_2[(t1-shift_w-1+k)*Width+t2-1+t].imag();
                                    B[k][t]=complex<float>(temp_real,temp_imag);
                                }
                            }
                            complex<float> D[4];
                            for(int k=0;k<4;k++)
                            {
                                D[k]=(float)A[0]*B[0][k]+(float)A[1]*B[1][k]+(float)A[2]*B[2][k]+(float)A[3]*B[3][k];
                            }
                            slave_2[ii*Width+j]=D[0]*(float)C[0]+D[1]*(float)C[1]+D[2]*(float)C[2]+D[3]*(float)C[3];
                        }
                    }
                }
                file2.write((char *)slave_2,sizeof(complex<float>)*Width*(Height-bnum*NewH));
                //
                delete[] slave_2;
                delete[] bslave_2;
            }
        }
        //
        delete[] bslave;
        delete[] slave1;
        //
    }
// #pragma endregion Resample cShort

// #pragma region Resample cFloat
    else if(eCFLOAT32 == datatype )
    {
        short dSize = sizeof(complex<float>);
        complex<float>*bslave   =   new complex<float>[Width*temp_H];   ;
        complex<float>*slave1=new complex<float>[Width*NewH];   

        for(bnum=0;bnum<temp_N;bnum++)  //
        {
            if(bnum < temp_N-1)         //
            {
                int shift_w=0;          
                if(bnum==0)             
                {
                    file1.read((char *)bslave,dSize*Width*(temp_H-delth));
                    shift_w=bnum*600;   
                }
                else                    
                {                       
                    file1.seekg(-2*delth*Width*dSize,ios::cur);
                    file1.read((char *)bslave,dSize*Width*temp_H);

                    shift_w=bnum*600-delth;
                }
                //
                for(i=bnum*600;i<(bnum+1)*600 && i< Height ;i++)                    
                {
                    /*
                    wndProgress.StepIt();
                    wndProgress.PeekAndPump();
                    if (wndProgress.Cancelled()) 
                    {
                        AfxMessageBox("quxiao!");
                        file1.close();
                        file2.close();
                        _unlink(lpDataIn2);

                        delete[] slave1;
                        delete[] bslave;                            

                        wndProgress.DestroyWindow();                
                        return;
                    }
                    */
                    int ii=i-bnum*600;

                    for(j=0;j<Width;j++)
                    {
                        //L.Set(0,0,1);L.Set(0,1,j);L.Set(0,2,i);L.Set(0,3,j*j);L.Set(0,4,i*j);L.Set(0,5,i*i);
                        dx = yMtxCoef[0]+ yMtxCoef[1]*j+yMtxCoef[2]*i+yMtxCoef[3]*j*j+yMtxCoef[4]*i*j+yMtxCoef[5]*i*i; 
                        dy = xMtxCoef[0]+ xMtxCoef[1]*j+xMtxCoef[2]*i+xMtxCoef[3]*j*j+xMtxCoef[4]*i*j+xMtxCoef[5]*i*i;

                        t1=floor(dx);t2=floor(dy);t3=t1+1;t4=t2+1;  
                        alpha=dx-t1;    //
                        belta=dy-t2;    
                                                
                        if((t1==0 && t2>=0 && t2<Width-1)||(t2==0 && t1>=0 && t1<Height-1)
                           ||(t1==Height-2 && t2>=0 && t2<Width-1)||(t2==Width-2 && t1>=0 && t1<Height-1))
                        {
                            float temp_real=(1-alpha)*(1-belta)*(bslave[(t1-shift_w)*Width+t2].real())
                                +alpha*(1-belta)*(bslave[(t3-shift_w)*Width+t2].real())+(1-alpha)*belta*(bslave[(t1-shift_w)*Width+t4].real())
                                +alpha*belta*(bslave[(t3-shift_w)*Width+t4].real());
                            float temp_imag=(1-alpha)*(1-belta)*(bslave[(t1-shift_w)*Width+t2].imag())
                                +alpha*(1-belta)*(bslave[(t3-shift_w)*Width+t2].imag())+(1-alpha)*belta*(bslave[(t1-shift_w)*Width+t4].imag())
                                +alpha*belta*(bslave[(t3-shift_w)*Width+t4].imag());
                            slave1[ii*Width+j]=complex<float>(temp_real,temp_imag);
                        }
                        else if(t1<0 || t2<0 ||
                                t1>Height-2 || t2>Width-2 ||
                                t1-shift_w < 1 || t1-shift_w > temp_H-2)
                        {
                            slave1[ii*Width+j]=0;
                        }
                        else
                        {
                            double A[4];
                            double C[4];
                            complex<float> B[4][4];
                            A[0]=4-8*(1+alpha)+5*(1+alpha)*(1+alpha)-(1+alpha)*(1+alpha)*(1+alpha);
                            A[1]=1-2*alpha*alpha+alpha*alpha*alpha;
                            A[2]=1-2*(1-alpha)*(1-alpha)+(1-alpha)*(1-alpha)*(1-alpha);
                            A[3]=4-8*(2-alpha)+5*(2-alpha)*(2-alpha)-(2-alpha)*(2-alpha)*(2-alpha);
                            C[0]=4-8*(1+belta)+5*(1+belta)*(1+belta)-(1+belta)*(1+belta)*(1+belta);
                            C[1]=1-2*belta*belta+belta*belta*belta;
                            C[2]=1-2*(1-belta)*(1-belta)+(1-belta)*(1-belta)*(1-belta);
                            C[3]=4-8*(2-belta)+5*(2-belta)*(2-belta)-(2-belta)*(2-belta)*(2-belta);
                            for(int k=0;k<4;k++)
                            {
                                for(int t=0;t<4;t++)
                                {
                                    float temp_real=bslave[(t1-shift_w-1+k)*Width+t2-1+t].real();
                                    float temp_imag=bslave[(t1-shift_w-1+k)*Width+t2-1+t].imag();
                                    B[k][t]=complex<float>(temp_real,temp_imag);
                                }
                            }
                            complex<float> D[4];
                            for(int k=0;k<4;k++)
                            {
                                D[k]=(float)A[0]*B[0][k]+(float)A[1]*B[1][k]+(float)A[2]*B[2][k]+(float)A[3]*B[3][k];
                            }
                            slave1[ii*Width+j]=D[0]*(float)C[0]+D[1]*(float)C[1]+D[2]*(float)C[2]+D[3]*(float)C[3];
                        }
                    }
                }
                file2.write((char *)slave1,dSize*Width*NewH);
            }
            else //bnum == tempN-1
            {
                int shift_w=0;
                int last_H=Height-bnum*NewH+delth;  
                complex<float>*bslave_2=new complex<float>[last_H*Width];
                complex<float>*slave_2=new complex<float>[(last_H-delth)*Width];    
                if(bnum==0) 
                {
                    shift_w=0;
                    file1.read((char *)bslave_2,dSize*(Height-bnum*NewH)*Width);
                }   
                else    
                {
                    shift_w=bnum*600-delth;

                    file1.seekg(-2*delth*Width*dSize,ios::cur);
                    file1.read((char *)bslave_2,dSize*last_H*Width);        
                }

                for(i=bnum*600; i< Height ;i++)
                {
                    /*
                    wndProgress.StepIt();
                    wndProgress.PeekAndPump();
                    if (wndProgress.Cancelled()) 
                    {
                        AfxMessageBox("cancel!");
                        file1.close();
                        file2.close();
                        _unlink(lpDataOut);

                        delete[] slave1;
                        delete[] bslave;

                        delete[] slave_2;
                        delete[] bslave_2;                      

                        wndProgress.DestroyWindow();                
                        return;
                    }   
                    */
                    int ii=i-bnum*600;
                    for(j=0;j<Width;j++)
                    {
                        //L.Set(0,0,1);L.Set(0,1,j);L.Set(0,2,i);L.Set(0,3,j*j);L.Set(0,4,i*j);L.Set(0,5,i*i);
                        dx = yMtxCoef[0]+ yMtxCoef[1]*j+yMtxCoef[2]*i+yMtxCoef[3]*j*j+yMtxCoef[4]*i*j+yMtxCoef[5]*i*i; 
                        dy = xMtxCoef[0]+ xMtxCoef[1]*j+xMtxCoef[2]*i+xMtxCoef[3]*j*j+xMtxCoef[4]*i*j+xMtxCoef[5]*i*i;
                        
                        t1=floor(dx);t2=floor(dy);t3=t1+1;t4=t2+1;  
                        alpha=dx-t1;    
                        belta=dy-t2;                        
                        if((t1==0 && t2>=0 && t2<Width-1)||(t2==0 && t1>=0 && t1<Height-1)
                           ||(t1==Height-2 && t2>=0 && t2<Width-1)||(t2==Width-2 && t1>=0 &&t1<Height-1))
                        {
                            float temp_real=(1-alpha)*(1-belta)*(bslave_2[(t1-shift_w)*Width+t2].real())
                                +alpha*(1-belta)*(bslave_2[(t3-shift_w)*Width+t2].real())+(1-alpha)*belta*(bslave_2[(t1-shift_w)*Width+t4].real())
                                +alpha*belta*(bslave_2[(t3-shift_w)*Width+t4].real());
                            float temp_imag=(1-alpha)*(1-belta)*(bslave_2[(t1-shift_w)*Width+t2].imag())
                                +alpha*(1-belta)*(bslave_2[(t3-shift_w)*Width+t2].imag())+(1-alpha)*belta*(bslave_2[(t1-shift_w)*Width+t4].imag())
                                +alpha*belta*(bslave_2[(t3-shift_w)*Width+t4].imag());
                            slave_2[ii*Width+j]=complex<float>(temp_real,temp_imag);
                        }
                        else if(t1<0 || t2<0 || t1>Height-2 || t2>Width-2 ||
                                t1-shift_w < 1 || t1-shift_w > (last_H-delth)-2)    //temp_H
                        {
                            slave_2[ii*Width+j]=0;
                        }
                        else
                        {
                            double A[4];
                            double C[4];
                            complex<float> B[4][4];
                            A[0]=4-8*(1+alpha)+5*(1+alpha)*(1+alpha)-(1+alpha)*(1+alpha)*(1+alpha);
                            A[1]=1-2*alpha*alpha+alpha*alpha*alpha;
                            A[2]=1-2*(1-alpha)*(1-alpha)+(1-alpha)*(1-alpha)*(1-alpha);
                            A[3]=4-8*(2-alpha)+5*(2-alpha)*(2-alpha)-(2-alpha)*(2-alpha)*(2-alpha);
                            C[0]=4-8*(1+belta)+5*(1+belta)*(1+belta)-(1+belta)*(1+belta)*(1+belta);
                            C[1]=1-2*belta*belta+belta*belta*belta;
                            C[2]=1-2*(1-belta)*(1-belta)+(1-belta)*(1-belta)*(1-belta);
                            C[3]=4-8*(2-belta)+5*(2-belta)*(2-belta)-(2-belta)*(2-belta)*(2-belta);
                            for(int k=0;k<4;k++)
                            {
                                for(int t=0;t<4;t++)
                                {
                                    float temp_real=bslave_2[(t1-shift_w-1+k)*Width+t2-1+t].real();
                                    float temp_imag=bslave_2[(t1-shift_w-1+k)*Width+t2-1+t].imag();
                                    B[k][t]=complex<float>(temp_real,temp_imag);
                                }
                            }
                            complex<float> D[4];
                            for(int k=0;k<4;k++)
                            {
                                D[k]=(float)A[0]*B[0][k]+(float)A[1]*B[1][k]+(float)A[2]*B[2][k]+(float)A[3]*B[3][k];
                            }
                            slave_2[ii*Width+j]=D[0]*(float)C[0]+D[1]*(float)C[1]+D[2]*(float)C[2]+D[3]*(float)C[3];
                        }
                    }
                }
                file2.write((char *)slave_2,sizeof(complex<float>)*Width*(Height-bnum*NewH));
                //
                delete[] slave_2;
                delete[] bslave_2;
            }
        }
        //
        delete[] bslave;
        delete[] slave1;
    }
// #pragma endregion Resample cFloat
    file1.close();
    file2.close();
    // wndProgress.DestroyWindow();

    /***************end Test*********************/

// #pragma region New Header export
    CRMGHeader header(sImg.m_oHeader);
    header.DataType = eCFLOAT32;
    header.Sample = mImg.m_oHeader.Sample;
    header.Line = mImg.m_oHeader.Line;
    header.Registration.azimuthOffst = newAoffset;
    header.Registration.registrType =1;
    header.Save(lpHdrOut);
// #pragma endregion New Header export

}

bool CRegistrFine::ReSampleImg_Master(string inMfile,string inHdrfile,string outMfile,string outHdrfile,int colM,int rowM,int dtype)
{
    if(eCFLOAT32 == dtype)
    {
        /*
        CopyFile(inMfile,outMfile,FALSE);
        CopyFile(inHdrfile,outHdrfile,FALSE);
        */

        //CopyFile is API of copy file in Win
        //by liuhuan 2015/4/9
        char cmd[128];
        memset(cmd, 0, 128);
        sprintf(cmd, "cp %s %s", inMfile.c_str(), outMfile.c_str());
        system(cmd);

        memset(cmd, 0, 128);
        sprintf(cmd, "cp %s %s", inHdrfile.c_str(), outHdrfile.c_str());
        system(cmd);
    }
    else if(eCINT16 == dtype)
    {
        /*
        CFile mFileIn,mFileOut;
        CFileException ex;
        if(!mFileIn.Open(inMfile,CFile::modeRead | CFile::typeBinary
                         |CFile::shareDenyNone,&ex))
        {
            TCHAR szError[1024];
            ex.GetErrorMessage(szError, 1024);
            AfxMessageBox(szError);
            return FALSE;
        }

        if(!mFileOut.Open(outMfile,File::modeWrite | CFile::modeCreate | CFile::mdeNoTruncate|
                         CFile::typeBinary|CFile::shareDenyWrite,&ex))   2279:                mFileOut.write((char *)(outdatasizeof(complex<float>)*colM));
        {
            TCHAR szError[1024];
            ex.GetErrorMessage(szError, 1024);
            AfxMessageBox(szError);
            mFileIn.close();
            return FALSE;
        }
        */

//open file in linux
        //byliuhuan 2015/4/9
        //iofstream seems do not have much option for multithread read/write file,which CFile has.

        ifstream mFileIn(inMfile, ios::in | ios::binary);
        if (!mFileIn.is_open()) {
            cout << "error open image mFileIn\n";
            return false;
        }
        ofstream mFileOut(outMfile, ios::out | ios::binary);
        if (!mFileOut.is_open()) {
            cout << "error open image mFileOut\n";
            return false;
        }


        complex <short> *indata;
        complex<float> *outdata;
        int dsize = sizeof(complex<short>);
        int iBlock,eachRow;
        int lastRow;
        if(rowM < 3000)
        {
            iBlock =1;
            eachRow = rowM;
            lastRow =0;
        }
        else
        {
            eachRow = 2000;
            iBlock = rowM/eachRow;
            lastRow = rowM - iBlock*eachRow;
        }
        outdata = new complex<float>[colM];
        for(int b=0;b<iBlock;b++)
        {
            indata = new complex<short>[eachRow*colM];
            mFileIn.read((char *)indata,dsize*eachRow*colM);
            for(int m=0;m<eachRow;m++)
            {               
                for(int n=0;n<colM;n++)
                    outdata[n] = complex<float>(indata[m*colM+n].real(),indata[m*colM+n].imag());
                mFileOut.write((char *)outdata,sizeof(complex<float>)*colM);
            }
            delete[] indata;
        }
        if(0!= lastRow)
        {
            indata = new complex<short>[lastRow*colM];
            mFileIn.read((char *)indata,dsize*lastRow*colM);
            for(int i=0;i<lastRow;i++)
            {               
                for(int n=0;n<colM;n++)
                    outdata[n] = complex<float>(indata[i*colM+n].real(),indata[i*colM+n].imag());
               mFileOut.write((char *)outdata,sizeof(complex<float>)*colM);
            }
            delete[] indata;
            mFileIn.close();
            mFileOut.close();
            delete[] outdata;
        }
        CRMGImage mImg(inMfile,inHdrfile);
        CRMGHeader header(mImg.m_oHeader);
        header.DataType = eCFLOAT32;
        header.Save(outHdrfile);
    }
    else if(eFLOAT32 == dtype)
    {
        /*
        CopyFile(inMfile,outMfile,FALSE);
        CopyFile(inHdrfile,outHdrfile,FALSE);
        */

//CopyFile is API of copy file in Win
        //by liuhuan 2015/4/9
        char cmd[128];
        memset(cmd, 0, 128);
        sprintf(cmd, "cp %s %s", inMfile.c_str(), outMfile.c_str());
        system(cmd);

        memset(cmd, 0, 128);
        sprintf(cmd, "cp %s %s", inHdrfile.c_str(), outHdrfile.c_str());
        system(cmd);
    }
    return true;
}

void CRegistrFine::oversample(float *A, short factor,short l,short p,float *Result)
{

    //l:  行数；p:  列数
    float *complA;
    complA = new float[l*p];
    int ierr;

    ierr = fft2d(A,complA,l,p);             // A=fft2d(A)

    // ______Correct A because length iseven______
    unsigned int halfl = l/2;
    unsigned int halfp = p/2;
    if (l!=1)
    {
        for (register int i=0;i<p;i++)
        {
            A[halfl*p+i] *=0.5;
            //A(halfl,i) *= .5;
            complA[halfl*p+i] *=.5;
            //complA(halfl,i) *= .5;
        }
    }
    if (p!=1)
    {
        for (register int j=0;j<l;j++)
        {
            A[j*p+halfp] *=.5;
            complA[j*p+halfp] *=.5;
        }
    }

    // ______Insert zeros______
    unsigned int padl = l*factor - l;           // number of zeros to be inserted
    unsigned int padp = p*factor - p;           // number of zeros to be inserted
    if (l==1) 
        padl=0;
    if (p==1) 
        padp=0;

    //CRect is a class of MFC.probably need to write one RECT class in linux
    //by liuhuan 2015/4/9

    //CRect winA1;
    //winA1.SetRect(0,0,halfp,halfl);

    Rect winA1(0, 0, halfp, halfl);
    ////window winA1 = {0, halfl, 0, halfp};
    if (l==1)    
        winA1.BottomRight().y=0;        // prevent l/2-1=-1=undef.
    else if (p==1) 
        winA1.BottomRight().x = 0;   // winA1.pixhi  = 0;       // prevent p/2-1=-1=undef.
    
    //window winA2 = {0, winA1.linehi, halfp, p-1};
    //window winA3 = {halfl, l-1, 0, winA1.pixhi};
    //window winA4 = {winA3.linelo, winA3.linehi, winA2.pixlo, winA2.pixhi};
    //window winR2 = {0, winA1.linehi, halfp + padp, p+padp-1};
    //window winR3 = {halfl+padl, l+padl-1, 0, winA1.pixhi};
    //window winR4 = {winR3.linelo, winR3.linehi, winR2.pixlo, winR2.pixhi};
    
    //CRect winA2; winA2.SetRect(halfp,0,p-1,winA1.BottomRight().y);
    //CRect winA3; winA3.SetRect(0,halfl,winA1.BottomRight().x,l-1);
    //CRect winA4; winA4.SetRect(winA2.TopLeft().x,winA3.TopLeft().y,winA2.BottomRight().x,winA3.BottomRight().y);
    //CRect winR2; winR2.SetRect(halfp+padp,0,p+padp-1,winA1.BottomRight().y);
    //CRect winR3;winR3.SetRect(0,halfl+padl,winA1.BottomRight().x,l+padl-1);
    //CRect winR4; winR4.SetRect(winR2.TopLeft().x,winR3.BottomRight().y,winR2.BottomRight().x,winR3.BottomRight().y);
    
    //real4 *Result;
    //Result =new real4[(l*factor)*(p*factor)];     // initialized to 0

    //ZeroMemory is API of WIN
    //by liuhuan 2015/4/9
    //ZeroMemory(Result,(l*factor*p*factor)*sizeof(float));
    memset(Result, 0, (l * factor * p * factor)*sizeof(float));

    float *ComplR;
    ComplR = new float[(l*factor)*(p*factor)];      // initialized to 0
    // ZeroMemory(ComplR,(l*factor*p*factor)*sizeof(float));
    memset(ComplR, 0, (l * factor * p * factor)*sizeof(float));

    //for(register int m=0;m < winA1.linehi;m++)
    for(register int m=0;m < winA1.BottomRight().y;m++)
    {
//      for(register int n=0;n<winA1.pixhi;n++)
        for(register int n=0;n<winA1.BottomRight().x;n++)
        {
            Result[m*p*factor+n] = A[m*l+n];
            Result[m*p*factor+(p*factor-halfp+n)] = A[m*p+halfp+n];
            Result[(l*factor-halfl+m)*p*factor+n] =A[(halfl+m)*p+n];
            Result[(l*factor-halfl+m)*p*factor+(p*factor-halfp+n)] = A[(halfl+m)*p+halfp+n];

            ComplR[m*p*factor+n] = complA[m*l+n];
            ComplR[m*p*factor+(p*factor-halfp+n)] = complA[m*p+halfp+n];
            ComplR[(l*factor-halfl+m)*p*factor+n] = complA[(halfl+m)*p+n];
            ComplR[(l*factor-halfl+m)*p*factor+(p*factor-halfp+n)] = complA[(halfl+m)*p+halfp+n];
        }
    }

    ifft2d(Result,ComplR,l*factor,p*factor);                // Result =ifft2d
    if (l==1 || p==1)
    {
        for(register int aa = 0;aa<l*factor*p*factor;aa++)
            Result[aa] *= (float)factor;
    }
    else
    {
        for(register int aa=0;aa<l*factor*p*factor;aa++)
            Result[aa]  = Result[aa]*(float)factor*(float)factor;
    }
    //Result *= (real4(factor)*real4(factor));
    delete[] complA;
    delete[] ComplR;

    // ______Don't return extrapolated part______
    //window win = {0, Result.lines()-factor, 0, Result.pixels()-factor};
    //matrix<real4> R2(win,Result);
    //return R2;
} // END oversample
//二维快速付里叶正变换
int CRegistrFine::fft2d(float *xr,float *xi,int nx,int ny)
{
    int a,b;
    int lx = getIndex(nx);
    if(lx < 0) 
        return -1;      //行数不为 2**n
    int ly = getIndex(ny);
    if(ly < 0) 
        return -2;      //列数不为 2**n

    double *dr,*di;

    //对列进行变换

    dr = new double[ny];
    di = new double[ny];
    for(a=0;a<nx;a++)
    {
        for(b=0;b<ny;b++)
        {
            dr[b] = xr[a*ny+b];
            di[b] = xi[a*ny+b];
        }
        fft1d(dr,di,ly);
        for(b=0;b<ny;b++){
            xr[a*ny+b] = dr[b];
            xi[a*ny+b] = di[b];
        }
    }
    delete []dr;
    delete []di;

    //对行进行变换
    dr = new double[nx];
    di = new double[nx];
    for(b=0;b<ny;b++)
    {
        for(a=0;a<nx;a++)
        {
            dr[a] = xr[a*ny+b];
            di[a] = xi[a*ny+b];
        }
        fft1d(dr,di,lx);
        for(a=0;a<nx;a++){
            xr[a*ny+b] = dr[a];
            xi[a*ny+b] = di[a];
        }
    }

    delete []dr;
    delete []di;

    return 1;
}
int CRegistrFine::getIndex(int i)
{
    int j = 0;

    int k = i;
    while(k >= 2){
        k = k >> 1;
        j ++;
    }
    k = 1;
    for(int l = 0;l<j;l++)
        k = k << 1;
    if(k == i) return j;
    else return -1;
}
//一维快速付里叶变换
void  CRegistrFine::fft1d(double *xr,double *xi,int ln)
{
    double tr,ti,ur,ui,wr,wi,yr,yi;     //pi,
    int   i,j,k,l,n,nv2,nm1,le,le1,ip;

    n = 1;
    for(i=0;i<ln;i++) n *= 2 ;
    const double pi = 3.1415;
    nv2 = n/2;
    nm1 = n-1;
    j= 1;
    for(i=1;i<=nm1;i++)
    {
        if(i<j) 
        {
            tr = xr[j-1];
            ti = xi[j-1];
            xr[j-1] = xr[i-1];
            xi[j-1] = xi[i-1];
            xr[i-1] = tr;
            xi[i-1] = ti;
        }
        k = nv2;
        while(k<j) 
        {
            j = j-k;
            k = k/2;
        }
        j = j+k;
    }
    for(l = 1;l<=ln;l++)
    {
        le = 1;
        for(i=0;i<l;i++) le *= 2 ;
        le1 = le/2;
        ur = 1.0;
        ui = 0.0;
        wr = cos(pi/le1);
        wi = -sin(pi/le1);
        for(j=0;j<le1;j++)
        {
            for( i=j;i<n;i+=le)
            {
                ip = i+le1;
                tr = xr[ip]*ur-xi[ip]*ui;
                ti = xi[ip]*ur+xr[ip]*ui;
                xr[ip] = xr[i]-tr;
                xi[ip] = xi[i]-ti;
                xr[i] = xr[i]+ tr;
                xi[i] = xi[i] + ti;
            }
            yr = ur*wr-ui*wi;
            yi = ui*wr+ur*wi;
            ur = yr;
            ui = yi;
        }
    }
}
//二维付里叶逆变换
int CRegistrFine::ifft2d(float *xr,float *xi,int nx,int ny)
{
    int a,b;
    int lx = getIndex(nx);
    if(lx < 0) 
        return -1;  //行数不为 2**n
    int ly = getIndex(ny);
    if(ly < 0) 
        return -2;  //列数不为 2**n

    double *dr,*di;

    //对列进行变换
    dr = new double[ny];
    di = new double[ny];
    for(a=0;a<nx;a++)
    {
        for(b=0;b<ny;b++)
        {
            dr[b] = xr[a*ny+b];
            di[b] = -xi[a*ny+b];
        }
        fft1d(dr,di,ly);
        for(b=0;b<ny;b++)
        {
            xr[a*ny+b] = dr[b];
            xi[a*ny+b] = di[b];
        }
    }
    delete []dr;
    delete []di;

    //对行进行变换
    dr = new double[nx];
    di = new double[nx];
    for(b=0;b<ny;b++){
        for(a=0;a<nx;a++){
            dr[a] = xr[a*ny+b];
            di[a] = xi[a*ny+b];
        }
        fft1d(dr,di,lx);
        for(a=0;a<nx;a++){
            xr[a*ny+b] = dr[a]/((double)(nx*ny));
            xi[a*ny+b] = -di[a]/((double)(nx*ny));
        }
    }

    delete []dr;
    delete []di;
    return 1;
}
