#include "ResidueUnw.h"
#include "Delaunay.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <math.h>
#include "Lsqr.h"
#include <iostream>
#include <fstream>

#define const_Pi 3.1415926

CResidueUnw::CResidueUnw(void)
{
}

void CResidueUnw::Batch()
{

}

void CResidueUnw::Process()
{
   /* CDlgResidueUnw dlg;*/
	//if(dlg.DoModal()!=IDOK) return;
	/*ResidueUnwPro(dlg.m_filein,dlg.m_fileinH,dlg.m_fileout,dlg.m_fileoutH);*/
}

void CResidueUnw::ResidueUnwPro(string fileIn,string fileinH,string fileout,string fileoutH)
{
    //读取相关参数
    int numdiff;
    FILE *fp;
    char buf[200];
    fp=fopen(fileinH.c_str(),"r");
    while(!feof(fp))
    {
        fscanf(fp,"%s",buf);
        if (feof(fp)) break;
        if(!strcmp(buf,"[numdiff]"))
        {
            fscanf(fp,"%s",buf);
            numdiff=atoi(buf);
        }
    }
    ifstream file1 (fileIn, ios::in);
    if (!file1.is_open()) {
        cout << "error open file\n";
        return;
    }
    int flage=0;
    //读取相干点
    prSet pRSet;
    pointResult pointR;
    int nPointCount=0;  //计算点个数；
    while ((flage = file1.read((char *)(&pointR), sizeof(pointResult)).gcount()) > 1)
    {
        pRSet.insert(pointR);
        nPointCount++;
    }
    file1.close();

    ofstream file_out (fileout, ios::out);
    if (!file_out.is_open()) {
        cout << "error open file\n";
        return;
    }

        //输出解缠后相位
    pointResiduUnwPha pointRUPhase;
    for(prIterator tempri=pRSet.begin();tempri!=pRSet.end();tempri++)
    {

        pointRUPhase.index=tempri->index;
        pointRUPhase.X=tempri->X;
        pointRUPhase.Y=tempri->Y;
        for(int i=0;i<numdiff;i++)
        {   
            pointRUPhase.residuePha[i]=tempri->residuePha[i];
        }

        file_out.write((char *)(&pointRUPhase), sizeof(pointResiduUnwPha));
    }

    file_out.close();


}

double CResidueUnw::Round(double r)
{
     return (r > 0.0) ? floor(r + 0.5) : ceil(r - 0.5);
}

void CResidueUnw::WrapPhase(float & phase)
{
    float temp=phase;

    if(temp>const_Pi)
        temp=temp-2*const_Pi;
    if(temp<-const_Pi)
        temp+=2*const_Pi;

    phase=temp;
}

CResidueUnw::~CResidueUnw(void)
{
}
