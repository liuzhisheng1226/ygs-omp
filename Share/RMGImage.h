#pragma once
#include "RMGBase.h"
#include "SpMatrix.h"
#include <string.h>

/// @file RMGImage.h 
/// @brief RMGͼ�����ļ�
/// @author Zhaolong


/// <summary>
//// RMGͼ���ඨ��
/// </summary>
/// <remarks>����RMGͼ���࣬�ṩRMGͼ�񹫹��������ֶ�</remarks>
/// @ingroup Core
class CRMGImage : public CRMGBase
{
public:
    /// <summary>
    /// ���캯��
    /// </summary>
    /// <param name="lpDataFullname">�����ļ�·��</param>
    CRMGImage(string lpDataFullname);
    /// <summary>
    /// ���캯��
    /// </summary>
    /// <param name="lpDataFullname">�����ļ�·��</param>
    /// <param name="lpHdrFullname">ͷ�ļ�·��</param>
    CRMGImage(string lpDataFullname,string lpHdrFullname);
    /// <summary>
    /// ��������
    /// </summary>  
    ~CRMGImage(void);
    /// <summary>
    /// ��γ�����굽�ѿ�������ת��
    /// </summary>
    /// <param name="x">x��������</param>
    /// <param name="y">y��������</param>
    /// <param name="z">z��������</param>
    void LonLat2Coordinate(double &x,double &y,double &z);

    //���ݵ�������꣬��Ըõ����ʱ���ǵ�ʱ��
    double  getSatposT(double x, double y,double z,CSpMatrix<double> &xcoef,CSpMatrix<double> &ycoef,CSpMatrix<double> &zcoef,bool isPreorb=false);

    //ԭRMGInSAR�࣬�ַŵ�rmgImage��,��������״̬ʸ��stateVector�����ǹ��(x,y,z)�Ķ���ʽ���ϵ��
    void OrbitCoef(vector<CRMGHeader::StructStateVector> stateVector,CSpMatrix<double> &xcoef,CSpMatrix<double> &ycoef,CSpMatrix<double> &zcoef);

    //���ݲ�ֵ���������״̬ʸ�������ǹ���Ķ���ʽ���ϵ��
    void PreorbCoef(vector<CRMGHeader::StructStateVector> stateVector,CSpMatrix<double> &xcoef,CSpMatrix<double> &ycoef,CSpMatrix<double> &zcoef);

    
    //ʵ�ֶ���ʽ�����㣨satX-x��*satVx+��satY-y��*satVy+(satZ-z)*satVz,�ڳ���ʱ����ֵ=0
    double Polynomial(CSpMatrix<double> &aef,CSpMatrix<double> &bef,CSpMatrix<double> &cef,double t,double x,double y,double z);
    
    //����ʽ��ϣ��ù�ʽ��Ҫʵ��������д���º궨�壬����order��pos�����ٶ�v
    double Polyfit(CSpMatrix<double> &mat,double t,int order);


   //�����������������ϵ�����
    void Newton(double t,double semia, double semib,CSpMatrix<double> xPolyCoef,CSpMatrix<double> yPolyCoef,CSpMatrix<double> zPolyCoef,
            double R,double &x, double &y, double &z,int num);


};

