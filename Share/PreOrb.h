#pragma once
#include "RMGHeader.h"
#include <boost/regex.hpp>


class CPreOrb :public CRMGHeader
{
public:
    CPreOrb(void);
    CPreOrb(string oriPreOrbFile);
    ~CPreOrb(void);

    vector<StructStateVector> preOrbitPoint;
    
    //vector<StructStateVector> interPreOrbitPoint;

    //根据间隔，从原精轨数据中读取与图像时间对应的记录点信息
    void LoadOrifile(string preOrbFile,float fTgap);

    //从转换后的精轨数据文件获取精轨信息
    void LoadOrbfile();

public:
    string m_fileName;         //插值后精轨数据的文件名
    double  m_startT;           //精轨数据第一个点的起始时间
    float   m_fIntervalT;       //插值后精轨数据的时间间隔
    int     m_nCount;           //  
};

