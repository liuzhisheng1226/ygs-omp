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

    //根据已知点在边集合中寻找相应的边
    edgeReSet FindEdgeByPoint( edgeReSet& eRSet,pointResult startPoint,pscSet mpsSet);

    //根据点的Index取点
    pointResult FindPointByIndex(int pointIndex, pscSet& mprSet);

    ~CIntegration(void);
};

