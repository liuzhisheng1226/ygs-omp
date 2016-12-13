#include "InterferDepict.h"
#include <cstdio>

using namespace std;

CInterferDepict::CInterferDepict(void)
{
}


CInterferDepict::~CInterferDepict(void)
{
}

void CInterferDepict::Process()
{
}

void CInterferDepict::Batch()
{
}

void CInterferDepict::Depict()
{
    
    CRMGImage image1("","");
    CRMGImage image2("","");

    
    const int blockHeight=min(1000,image1.m_oHeader.Line);                  //块高度
    int blockCount=image1.m_oHeader.Line/blockHeight;                   //块数量
    if(image1.m_oHeader.Line % blockHeight >0) blockCount++;

    complex<float> *block1=new complex<float>[image1.m_oHeader.Sample*blockHeight];
    complex<float> *block2=new complex<float>[image1.m_oHeader.Sample*blockHeight];
    complex<float> *data=new complex<float>[image1.m_oHeader.Sample*blockHeight];
    float *phaData=new float[image1.m_oHeader.Sample*blockHeight];

    FILE *fp1=fopen("","rb+");
    FILE *fp2=fopen("","rb+");
    FILE *fp3=fopen("","wb+");
    FILE *fp4=fopen("","wb+");
    complex<float> tmpValue;
    int index=0,_blockHeight=blockHeight;
    
    //遍历所有块
    for(int i=0;i<blockCount;i++)
    {
        //最后一块数据高度
        if(i==blockCount-1&&blockCount>1) _blockHeight=image1.m_oHeader.Line-i*blockHeight;

        fread(block1,sizeof(complex<float>),image1.m_oHeader.Sample*_blockHeight,fp1);
        fread(block2,sizeof(complex<float>),image2.m_oHeader.Sample*_blockHeight,fp2);
        for(int j=i*blockHeight;i<(i+1)*blockHeight && i<image1.m_oHeader.Line;i++)
        {
            for(int k=0;k<image1.m_oHeader.Sample;k++)
            {
                index=(j-i*_blockHeight)*image1.m_oHeader.Sample+k;
                tmpValue=block1[index];
                data[index]=tmpValue*conj(block2[index]);
                phaData[index]=arg(data[index]);
            }//for k
        }//for j
        fwrite(data,sizeof(complex<float>),image1.m_oHeader.Sample*_blockHeight,fp3);
        fwrite(data,sizeof(complex<float>),image1.m_oHeader.Sample*_blockHeight,fp4);
    }//for i

}//Depict
