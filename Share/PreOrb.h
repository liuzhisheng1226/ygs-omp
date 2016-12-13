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

    //���ݼ������ԭ���������ж�ȡ��ͼ��ʱ���Ӧ�ļ�¼����Ϣ
    void LoadOrifile(string preOrbFile,float fTgap);

    //��ת����ľ��������ļ���ȡ������Ϣ
    void LoadOrbfile();

public:
    string m_fileName;         //��ֵ�󾫹����ݵ��ļ���
    double  m_startT;           //�������ݵ�һ�������ʼʱ��
    float   m_fIntervalT;       //��ֵ�󾫹����ݵ�ʱ����
    int     m_nCount;           //  
};

