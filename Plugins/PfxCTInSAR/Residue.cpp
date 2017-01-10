#include "Residue.h"
#include "Delaunay.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
#include <omp.h>

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
    
    int PSCcount=0;       //��ɵ�
    pscSet PscSet;
    PSC pscTemp;
    int flag=0;
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

    int IntegrationCount=0;    //���ɵ���
    prSet intePointSet;
    pointResult intePointTmp;
    flag=0;
    //��ȡ���ɵ�
    fstream InteFile (integrationFileIn, ios::in);
    if (!InteFile.is_open()) {
        cout << "error open file\n";
        return;
    }
    while ((flag = InteFile.read((char *)(&intePointTmp), sizeof(pointResult)).gcount()) > 1)
    {
        intePointSet.insert(intePointTmp);
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

    vector<pointResult> vprs;
    for(prIterator pri=intePointSet.begin();pri!=intePointSet.end();pri++) {
        vprs.push_back(*pri);
    }

    //����ÿ�����ɵ�Ĳ�����λ
    //for(prIterator pri=intePointSet.begin();pri!=intePointSet.end();pri++)
    
    omp_lock_t w_lock;
    omp_init_lock(&w_lock);
#pragma omp parallel for  
    for (int i = 0; i < vprs.size(); ++i)
    {
        pointResult intePointTemp = intePointTmp;
        pointResult *pri = &(vprs[i]);
        //��PSC���ҵ���Ӧ�ĵ㣬��ȡ��Ӧ����
        //PSC pscTemp;
        //pscTemp.m_pos.X=pri->X;
        //pscTemp.m_pos.Y=pri->Y;
        pscIterator psci=PscSet.find(PSC(pri->X, pri->Y));
        float Vtemp=pri->vel/365000;   //����λת����m/d  ��Ϊ deltaD����ĵ�λ��d���죩
        for(int i=0;i<numdiff;i++)
        {
            //ģ����λ
            //modelPha=0;
            float modelPha=4*const_Pi/wl*((psci->deltaD[i])*Vtemp-(psci->bv[i]*pri->hei)/(sin(psci->iAngle[i])*psci->sRange[i]));

            //�Ȳ���
            modelPha= modelPha-int(modelPha/(2*const_Pi))*2*const_Pi;

            //�����λ��ȥģ����λ
            float residuePha=psci->data[i]-modelPha;

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

            omp_set_lock(&w_lock);
            InteFile.seekg((streampos)i*sizeof(pointResult), ios::beg);
            InteFile.write((char *)(&intePointTemp), sizeof(pointResult));
            omp_unset_lock(&w_lock);
    }
    omp_destroy_lock(&w_lock);
    InteFile.close();
    intePointSet.clear();
    PscSet.clear();

}
CResidue::~CResidue(void)
{
}
