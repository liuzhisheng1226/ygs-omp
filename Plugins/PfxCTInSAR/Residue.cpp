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
    //��ȡ��ز���
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
    PSCcount=0;       //��ɵ�
    IntegrationCount=0;    //���ɵ���
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
    //��ȡ���ɵ�
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

    //����resultPointSet���еĲ�����λ���������
    //ֱ�Ӹ���ԭ���ļ��ɵ�
    InteFile.open(integrationFileIn, ios::out);
    if (!InteFile.is_open()) {
        cout << "error open file\n";
        return;
    }

    //����ÿ�����ɵ�Ĳ�����λ
    float modelPha;  //ģ����λ
    float residuePha;
    float Vtemp;
    pscIterator psci;  //�ҵ�PSC�ĵ�����
    //prSet NewIntePointSet;  //���ڸ��¼��ɵ�����Ĳ�����λ
    for(prIterator pri=intePointSet.begin();pri!=intePointSet.end();pri++)
    {
        //��PSC���ҵ���Ӧ�ĵ㣬��ȡ��Ӧ����
        pscTemp.m_pos.X=pri->X;
        pscTemp.m_pos.Y=pri->Y;
        psci=PscSet.find(pscTemp);
        Vtemp=pri->vel/365000;   //����λת����m/d  ��Ϊ deltaD����ĵ�λ��d���죩
        for(int i=0;i<numdiff;i++)
        {
            //ģ����λ
            modelPha=0;
            modelPha=4*const_Pi/wl*((psci->deltaD[i])*Vtemp-(psci->bv[i]*pri->hei)/(sin(psci->iAngle[i])*psci->sRange[i]));

            //�Ȳ���
            modelPha= modelPha-int(modelPha/(2*const_Pi))*2*const_Pi;

            //�����λ��ȥģ����λ
            residuePha=psci->data[i]-modelPha;

            if(residuePha>const_Pi)
            {
                residuePha=residuePha-2*const_Pi;
            }
            if(residuePha<-const_Pi)
            {
                residuePha=residuePha+2*const_Pi;
            }

            //���������λ
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
