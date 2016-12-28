#include "Uonlinear.h"
#include "Lsqr.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <fstream>

#define const_Pi 3.1415926

CUonlinear::CUonlinear(void)
{
}

void CUonlinear::Batch()
{
}

void CUonlinear::Process()
{
   /* CDlgUonlinear dlg;*/
	//if(dlg.DoModal()!=IDOK) return;
	//CString pscFileIn=dlg.m_PSCFileIn;					//PSC点文件
	//CString pscFileInH=dlg.m_PSCFileInH;
	//CString inteFileIn=dlg.m_integrationFileIn;			//线性形变速度
	//CString inteFileInH=dlg.m_integrationFileInH;
	//CString unwResidueFileIn=dlg.m_residueUnwFileIn;	//解缠后残余相位文件
	//CString unwResidueFileInH=dlg.m_residueUnwFileInH;
	//CString interGroup=dlg.m_interferGroupFileIn;		//干涉组合文件
	//CString sarimageTimefile=dlg.m_imageTimeFileIn;		//SAR图像成像时间，相对于第一幅图像
	//CString uonlinearOut=dlg.m_FileUoninearOut;
	//CString deformationOut=dlg.m_deformationFileOut;
	//CString actmosOut=dlg.m_FileActmosOut;
	//CString velFileOut=dlg.m_velFileOut;

	//UonlinearPro(pscFileIn,pscFileInH,inteFileIn,inteFileInH,sarimageTimefile,unwResidueFileIn,
		/*unwResidueFileInH,interGroup,actmosOut,velFileOut,deformationOut);*/
}

//  用于判断点是否在一定范围内
class pointOutCircle{

public:
    pointOutCircle(pointResiduUnwPha p,float distance):mPoint(p),mDistance(distance){}
    bool operator()(const pointResiduUnwPha &p)
    {
        bool b=false;
        if(p.X>(mPoint.X+mDistance))
        { if(p.Y>(mPoint.Y+mDistance))
            b=true;
        }
        
        return b;
    }

    pointResiduUnwPha mPoint;
    float mDistance;
};

void CUonlinear::UonlinearPro(string pscFileIn,string pscFileInH,string inteFileIn,string inteFileInH,string imageTime,
        string unwResidueFileIn,string unwResidueFileInH,string interGroup,string actmosOut,string velFileOut,string deformationOut)
{
    //根据集成线性形变头文件读取相关参数
    FILE *fp;
    char buf[100];
    float resolutionSize,actmosDistance,wl;
    int numdiff,numsar;
    fp=fopen(inteFileInH.c_str(),"r");
    while(!feof(fp))
    {
        fscanf(fp,"%s",buf);
        if (feof(fp)) break;
        if(!strcmp(buf,"[resolution]"))
        {
            fscanf(fp,"%s",buf);
            resolutionSize=atof(buf);
        }
        if(!strcmp(buf,"[actmosDistance]"))
        {
            fscanf(fp,"%s",buf);
            actmosDistance=atof(buf);
        }
        if(!strcmp(buf,"[wavelength]"))
        {
            fscanf(fp,"%s",buf);
            wl=atof(buf);
        }
        if(!strcmp(buf,"[numdiff]"))
        {
            fscanf(fp,"%s",buf);
            numdiff=atoi(buf);
        }
        if(!strcmp(buf,"[numsar]"))
        {
            fscanf(fp,"%s",buf);
            numsar=atoi(buf);
        }
    }
    fclose(fp);

    int flag=0;
    int PSCcount=0;
    pscSet PscSet;
    PSC pscTemp;

    //1、读取PSC点
    ifstream PscFile (pscFileIn, ios::in);
    if (!PscFile.is_open()) {
        cout << "error open file\n";
        return;
    }
    while ((flag = PscFile.read((char *)(&pscTemp), sizeof(PSC)).gcount()) > 1)
    {
        PscSet.insert(pscTemp);
        PSCcount++;
    }
    PscFile.close();

    //2、读取线性速度文件     pointResult结构体中包含线性形变速度
    int IntegrationCount=0;
    pointResult intePointTemp;
    prSet intePointSet;
    ifstream InteFile (inteFileIn, ios::in);
    if (!InteFile.is_open()) {
        cout << "error open file\n";
        return;
    }
    flag=0;
    while ((flag = InteFile.read((char *)(&intePointTemp), sizeof(pointResult)).gcount()) > 1)
    {
        intePointSet.insert(intePointTemp);
        IntegrationCount++;
    }
    InteFile.close();

    //3、读取解缠后的残余相位
    flag=0;
    pointResiduUnwPha residuePointTemp;
    prupSet residuePointSet;
    ifstream residueFile (unwResidueFileIn, ios::in);
    if (!residueFile.is_open()) {
        cout << "error open file\n";
        return;
    }
    while ((flag = residueFile.read((char *)(&residuePointTemp), sizeof(pointResult)).gcount()) > 1)
    {
        residuePointSet.insert(residuePointTemp);
    }
    residueFile.close();

    //对残余相位进行空间低通滤波--->空间平均
    //对每个离散点  其离散点的残余相位等于周围（大气相关距离内）点的平均
    prupSet newResiduePointSet;
    int size=int(actmosDistance/resolutionSize);
    prupIterator prupi1;
    prupIterator prupi2;
    int tempCount;
    float *residuetemp=new float[numdiff];
    memset(residuetemp,0,numdiff*sizeof(float));

    for(prupIterator prupi=residuePointSet.begin();prupi!=residuePointSet.end();prupi++)
    {
        tempCount=0;
        //矩形窗坐上角点
        prupi1=find_if(residuePointSet.begin(),residuePointSet.end(),pointOutCircle(*prupi,-size/2)); 
        //矩形窗右下角点
        prupi2=find_if(residuePointSet.begin(),residuePointSet.end(),pointOutCircle(*prupi,size/2));

        residuePointTemp.X=prupi->X;
        residuePointTemp.Y=prupi->Y;
        residuePointTemp.index=prupi->index;

        //清零
        for(int i=0;i<numdiff;i++)
           residuetemp[i]=0;

        for(prupi1;prupi1!=prupi2;prupi1++)
        {
            if(distance(PointF(prupi1->X,prupi1->Y),PointF(prupi->X,prupi->Y))<size/2)
            {
                tempCount++;
                //对所有残余相位进行平均
                for(int i=0;i<numdiff;i++)
                {
                    residuetemp[i]+=prupi1->residuePha[i];
                }
            }
        }

        for(int i=0;i<numdiff;i++)
        {
            residuePointTemp.residuePha[i]=residuetemp[i]/tempCount;
        }

        //添加平均后的点；
        newResiduePointSet.insert(residuePointTemp);
    }
    residuePointSet.clear();

    //最小二乘求解单幅SAR图像残余相位
    //读取干涉组和信息
    //定义干涉组合矩阵
    float **A=new float *[numdiff];
    for(int i=0;i<numdiff;i++)
    {
        A[i]=new float[numsar];
        memset(A[i],0,numsar*sizeof(float));
    }

    //这里使用的是自定义的干涉组合
    fp=fopen(interGroup.c_str(),"r");
    string strTemp;
    int countTemp=0;
    int index0,index1;
    while(fgets(buf,100,fp) != NULL)
    {
        
        strTemp=buf;
        int commaIndex = strTemp.find_first_of(',');
        //index0=atoi(&buf[2]);
        //index1=atoi(&buf[4]);
        index0 = atoi(strTemp.substr(0, commaIndex).c_str());
        index1 = atoi(strTemp.substr(commaIndex+1, strTemp.length()-commaIndex-1).c_str());
        A[countTemp][index0-1]=-1;
        A[countTemp][index1-1]=1;

        countTemp++;
    }
    if(countTemp!=numdiff)
    {
        cout << "wrong interGroup file, please check again!\n";
        return;
    }
    
    //然后对每个点来计算每幅SAR的残余相位，然后实现非形变相位和大气相位分离
    CLsqr mlsqr(numdiff,numsar);
    float *B=new float[numdiff];
    memset(B,0,numdiff*sizeof(float));

    float *X=new float[numsar];
    memset(X,0,numsar*sizeof(float));

    
    for(prupIterator prupi=newResiduePointSet.begin();prupi!=newResiduePointSet.end();prupi++)
    {
        for(int i=0;i<numdiff;i++)
            B[i]=prupi->residuePha[i];

        mlsqr.method(A,B,X);
        pointResiduUnwPha SarResiduePoint(prupi->X,prupi->Y);
        SarResiduePoint.index=prupi->index;
        for(int i=0;i<numsar;i++)
            SarResiduePoint.residuePha[i]=X[i];

        //添加
        residuePointSet.insert(SarResiduePoint);
    }
    newResiduePointSet.clear();

    //通过时间低通，获得非形变相位   时间序列上用个平均滑动窗
    prupSet uonlinearPointSet;
    prupSet actmosPointSet;
    pointResiduUnwPha uonlinearPoint,actmosPoint;
    for(prupIterator prupi=residuePointSet.begin();prupi!=residuePointSet.end();prupi++)
    {
        uonlinearPoint.X=prupi->X;
        uonlinearPoint.Y=prupi->Y;
        uonlinearPoint.index=prupi->index;

        actmosPoint.X=prupi->X;
        actmosPoint.Y=prupi->Y;
        actmosPoint.index=prupi->index;

         for(int i=0;i<numsar;i++)
         {

             //非线性形变相位
             //对于第一幅SAR图像
             if(i==0)
                 uonlinearPoint.residuePha[i]=(prupi->residuePha[i]+prupi->residuePha[i+1])/2;
             //对于最后一幅SAR图像
             else if(i==numsar-1)
                 uonlinearPoint.residuePha[i]=(prupi->residuePha[i-1]+prupi->residuePha[i])/2;
             else
                 uonlinearPoint.residuePha[i]=(prupi->residuePha[i-1]+prupi->residuePha[i]+prupi->residuePha[i+1])/3;

             //大气相位
             actmosPoint.residuePha[i]=prupi->residuePha[i]-uonlinearPoint.residuePha[i];

         }

         //添加
         uonlinearPointSet.insert(uonlinearPoint);
         actmosPointSet.insert(actmosPoint);
    }

    //输出线性形变速率，保存在TXT中
    fp=fopen(velFileOut.c_str(),"w");
    if(fp==NULL) return;
    fprintf(fp,"X\t\tY\t\tdeformagion rate(mm/year)");

    for(prIterator intepi=intePointSet.begin();intepi!=intePointSet.end();intepi++)
    {
        fprintf(fp,"\n%f",intepi->X);
        fprintf(fp,"\t\t%f",intepi->Y);
        fprintf(fp,"\t\t%f",intepi->vel);
    }
    fclose(fp);

    //输出每幅图像的形变量   线性形变+非线性形变
    //1、读取SAR图像相对于第一幅图像的成像时间
    float *SarTime=new float[numsar];
    memset(SarTime,0,numsar*sizeof(float));
    countTemp=0;
    fp=fopen(imageTime.c_str(),"r");
    while(fgets(buf,100,fp) !=  NULL)
    {
        SarTime[countTemp]=atof(buf)/365;    //转化到年
        countTemp++;
    }
    if(countTemp!=numsar)
    {
        cout << "wrong imageTime file, please check again!\n";
        return;
    }
    fclose(fp);

    prupIterator uonPi;
    float fDeformation=0.0;
    string strPath="";
    string strindex;
    for(int i=1;i<numsar;i++)
    {
        strindex = to_string(i+1);
        strPath=deformationOut+"/"+strindex+".txt";
        //打开
        fp=fopen(strPath.c_str(),"w");
        if(fp==NULL) return;
        fprintf(fp,"X\t\tY\t\tcumulative deformation(mm)");
        for(prIterator intepi=intePointSet.begin();intepi!=intePointSet.end();intepi++)
        {
            uonlinearPoint.X=intepi->X;
            uonlinearPoint.Y=intepi->Y;
            uonPi=uonlinearPointSet.find(uonlinearPoint);
            //测试，输出非线性形变
            fDeformation=intepi->vel*SarTime[i]+uonPi->residuePha[i]*wl/(4*const_Pi)*1000;   //非线性部分转化成毫米
            fprintf(fp,"\n%f",intepi->X);
            fprintf(fp,"\t\t%f",intepi->Y);
            fprintf(fp,"\t\t%f",fDeformation);
        }
        //关闭文件
        fclose(fp);
    }

    delete []residuetemp;
    residuetemp=NULL;

    for(int i=0;i<numdiff;i++)
    {
        delete A[i];
        A[i]=NULL;
    }
    delete []A;
    A=NULL;
    delete []B;
    B=NULL;
    delete []X;
    X=NULL;
    delete []SarTime;
    SarTime=NULL;



}
float CUonlinear::distance(PointF p1,PointF p2)
{
    return sqrt((p1.X-p2.X)*(p1.X-p2.X)+(p1.Y-p2.Y)*(p1.Y-p2.Y));
}

CUonlinear::~CUonlinear(void)
{

}
