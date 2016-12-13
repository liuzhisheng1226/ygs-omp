#pragma once
#include "../../Share/Core.h"
#include "../../Share/Common.h"
#include "Delaunay.h"

using namespace std;

class CIntegration : public CIProcessable
{
public:
    CIntegration(void);


    void Process();
    void Batch();

    void IntegrationPro(string m_Fileedge,string m_FileedgeH,string m_Filepoint,string m_FilepointH,
        float m_edgeThreshold,float m_resolution,float m_atmosDistance,int m_startX,int m_startY);

    //������֪���ڱ߼�����Ѱ����Ӧ�ı�
    edgeReSet FindEdgeByPoint( edgeReSet& eRSet,pointResult startPoint,pscSet mpsSet);

    //���ݵ��Indexȡ��
    pointResult FindPointByIndex(int pointIndex, pscSet& mprSet);

    ~CIntegration(void);
};

