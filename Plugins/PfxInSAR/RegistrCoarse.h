#pragma once
#include "../../Share/Core.h"
#include "../../Share/Common.h"
#include <complex>
#include <string>

class CRegistrCoarse : public CIProcessable
{
public:
    CRegistrCoarse(void);
    ~CRegistrCoarse(void);


    /*
    *   �̳нӿڷ���������ʵ��
    */
    void Process();
    void Batch();

    

    void Coarse(string lpDataFullname1,string lpHdrFullname1,string lpDataFullname2,string lpHdrFullname2,string lpDataExport,string lpHdrExport);

    /*
     * �������׼ƫ��
     */
    void CoarseOffset(CRMGImage &image1,CRMGImage &image2,int &colOffset,int &rowOffset);
    /*
     * ��ȡ����׼ָ����С��������
     */
    float* CoarseBlock(CRMGImage &image,int blockWidth,int blockHeight,Rect window);
    /*
     * ������Ӧͼ���Ӧ�����к�
     */
    //DOUBLE LocCoordinate(CRMGImage &image,int &row,int &col,double x3d,double y3d,double z3d);
    /*
     *ʵ�ֶ���ʽ
     */
    //DOUBLE Polynomial(CSpMatrix<double> &aef,CSpMatrix<double> &bef,CSpMatrix<double> &cef,double t,double x,double y,double z);
    /*
     *����ʽ��ϣ��ù�ʽ��Ҫʵ��������д���º궨��
    */
    //DOUBLE Polyfit(CSpMatrix<double> &mat,double t,int order);
    
    /*
     *�������ʽorder��ϵ��
     *@para: n      ����ʽָ��
     *@para: order  ��ǰ����
     */
    //LONG PolyCoef(int n,int order);

};

