#include "Residue.h"
#include "Delaunay.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>



#define const_Pi 3.141592658978

CResidue::CResidue(void)
{
}

void CResidue::Batch()
{

}

void CResidue::Process()
{
   /* CDlgResidue dlg;*/
	//if(dlg.DoModal()!=IDOK) return;
	/*ResiduePro(dlg.m_PscFileIn,dlg.m_PscFileInH,dlg.m_IntegrationFileIn,dlg.m_IntegrationFileInH);*/
}

void CResidue::ResiduePro(string pscFileIn,string pscFileInH,string integrationFileIn,string integrationFileInH)
{
    //读取相关参数
    int numdiff;
    float wl;
    FILE *fp;
    char buf[200];
    fp=fopen(pscFileInH.c_str(),"r");

    while(!feof(fp))
    {
        fscanf(fp,"%s",buf);
        if (feof(fp)) break;
        if(!strcmp(buf,"[numdiff]"))
        {
            fscanf(fp,"%s",buf);
            numdiff=atoi(buf);
        }
        if(!strcmp(buf,"[wavelength]"))
        {
            fscanf(fp,"%s",buf);
            wl=atof(buf);
        }
    }
    
    int PSCcount,IntegrationCount;
    PSCcount=0;       //相干点
    IntegrationCount=0;    //集成点数
    int flag=0;
    prSet intePointSet;
    pscSet PscSet;

    PSC pscTemp;
    pointResult intePointTemp;

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

    flag=0;
    //读取集成点
    fstream InteFile (integrationFileIn, ios::in);
    if (!InteFile.is_open()) {
        cout << "error open file\n";
        return;
    }
    while ((flag = InteFile.read((char *)(&intePointTemp), sizeof(pointResult)).gcount()) > 1)
    {
        intePointSet.insert(intePointTemp);
        IntegrationCount++;
    }
    InteFile.close();

    //更新resultPointSet点中的残余相位后，依次输出
    //直接覆盖原来的集成点
    InteFile.open(integrationFileIn, ios::out);
    if (!InteFile.is_open()) {
        cout << "error open file\n";
        return;
    }

    //计算每个集成点的残余相位
    float modelPha;  //模型相位
    float residuePha;
    float Vtemp;
    pscIterator psci;  //找到PSC的迭代器
    //prSet NewIntePointSet;  //用于更新集成点里面的残余相位
    for(prIterator pri=intePointSet.begin();pri!=intePointSet.end();pri++)
    {
        //在PSC中找到对应的点，读取相应参数
        pscTemp.m_pos.X=pri->X;
        pscTemp.m_pos.Y=pri->Y;
        psci=PscSet.find(pscTemp);
        Vtemp=pri->vel/365000;   //将单位转换成m/d  因为 deltaD里面的单位是d（天）
        for(int i=0;i<numdiff;i++)
        {
            //模型相位
            modelPha=0;
            modelPha=4*const_Pi/wl*((psci->deltaD[i])*Vtemp-(psci->bv[i]*pri->hei)/(sin(psci->iAngle[i])*psci->sRange[i]));

            //先缠绕
            modelPha= modelPha-int(modelPha/(2*const_Pi))*2*const_Pi;

            //差分相位减去模型相位
            residuePha=psci->data[i]-modelPha;

            if(residuePha>const_Pi)
            {
                residuePha=residuePha-2*const_Pi;
            }
            if(residuePha<-const_Pi)
            {
                residuePha=residuePha+2*const_Pi;
            }

            //保存残余相位
            intePointTemp.residuePha[i]=residuePha;

        }
            intePointTemp.hei=pri->hei;
            intePointTemp.index=pri->index;
            intePointTemp.vel=pri->vel;
            intePointTemp.X=pri->X;
            intePointTemp.Y=pri->Y;
            //NewIntePointSet.insert(intePointTemp);

            InteFile.write((char *)(&intePointTemp), sizeof(pointResult));
    }

    InteFile.close();

    intePointSet.clear();
    PscSet.clear();

}
CResidue::~CResidue(void)
{
}
