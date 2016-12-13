#pragma once
#include <vector>
#include <complex>
#include <string>
#include <iostream>
#include <sstream>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
using namespace std;
#define LINUX
#define MAXLINELENGTH 1024

typedef unsigned char BYTE;
// <liutao>
typedef unsigned short USHORT;
typedef short INT16;
typedef unsigned int UINT32;
typedef int INT32;
typedef float FLOAT;
typedef double DOUBLE;

/// @file RMGHeader.h
/// @brief RMGHeader.h�ļ� ������RMGͼ����ز���


/// �������ͽӿ���
enum ENUM_DATA_TYPE
{
    eUNKNOW,            ///<δ֪����
    eBYTE,              ///<BYTE����[1�ֽ�]
    eUINT16,            ///<unsigned short����[2�ֽ�]
    eINT16,             ///<signed short����[2�ֽ�]
    eUINT32,            ///<unsigned int����[4�ֽ�]
    eINT32,             ///<signed int����[4�ֽ�]
    eFLOAT32,           ///
    eFLOAT64,
    eCINT16,
    eCINT32,
    eCFLOAT32,
    eCFLOAT64
};

enum ENUM_SENSOR_TYPE
{
    RD2=0,
    ERS,
    JERS,
    ASAR,
    ENVISAT,
    ALOSPALSAR,
    RSAT,
    TERRESAR
};

enum ENUM_DIRECTION_TYPE
{
    ASCEND,
    DESCEND 
};


/// <summary>
/// RMGͼ��ͷ�ļ���
/// </summary>
/// <remarks>������RMGͼ�����������Ҫ�õ����������ǲ�������ͷ�ļ���ȡ����</remarks>
/// @ingroup Core
class CRMGHeader
{



public:
    CRMGHeader(void);
    ~CRMGHeader(void);
    
    /// <summary>
    /// �洢ͷ�ļ�
    /// </summary>
    /// <param name="lpFilename">ͷ�ļ�����</param>
    /// <returns>����ִ�н��</returns>
    bool Save(string lpFilename);

    
    /// <summary>
    /// ����ͷ�ļ�
    /// </summary>
    /// <param name="lpFilename">ͷ�ļ�����</param>
    /// <returns>����ִ�н��</returns>
    bool Load(string lpFilename);
    void del_char(char *str,char c);

private:    
    const char *CONST_SECTION_HEADER;               ///<ͷ�ļ����ý�����
    const char *CONST_SECTION_EXTENTION;            ///<ͷ�ļ�������չ����
    

public:
    
    ///ʱ���ṹ��
    struct StructTimePoint                  
    {
        string timeString;                          /*<����ʱ���ַ���*/
        short year;                                 /*<��*/
        short month;                                /*<��*/
        short day;                                  //<��
        short hour;                                 //<Сʱ
        short minute;                               //<����
        short second;                               //<��
        double decimal;                             //<С������
        double precision;                           //<��ʽ����ʱ�䣨hour*3600+minute*60+second+decimal��
        
        StructTimePoint()
        {
            year=0;
            month=0;
            day=0;
            hour=0;
            minute=0;
            second=0;
            decimal=0;
            precision=0;
        }
        ///���캯��
        StructTimePoint(string lpValue)                 
        {                                           
            //          0123456789ABCDEFGHIJKLMNOPQ         
            this->timeString=lpValue;
            // string timeText=lpValue.GetBuffer();
            string timeText=lpValue;
            //����ʱ���ַ����������ж�ʱ������(�����ܣ���øĳ�������ʽ)
            boost::regex patnOrbit("\\d{4}-\\d{3}-\\d{2}:\\d{2}:\\d{2}.+");
            boost::regex patnHeader("\\d{4}(-\\d{2}){2}T(\\d{2}:?){3}.+");
            if(boost::regex_match(timeText,patnHeader))//          lpValue = 2012-09-24T10:12:00.085693Z           27
            {
                this->year=atoi(timeString.substr(0,4).c_str());
                this->month=atoi(timeString.substr(5,2).c_str());
                this->day=atoi(timeString.substr(8,2).c_str());

                this->hour=atoi(timeString.substr(11,2).c_str());
                this->minute=atoi(timeString.substr(14,2).c_str());
                this->second=atoi(timeString.substr(17,2).c_str());            
                this->decimal=atof(timeString.substr(19,7).c_str());
            }
            else if (boost::regex_match(timeText,patnOrbit))//         lpValue = 2012-268-10:03:06.297266              24
            {
                this->year=atoi(timeString.substr(0,4).c_str());
                //int totalDay=atoi(timeString.substr(5,3));               
                //���� �¡��� ��Ϣ��Ҫ���������ÿ�µ����� ������Լ���
                this->month=0;
                this->day=0;
                //this->day=atoi(timeString.substr(8,2));

                this->hour=atoi(timeString.substr(9,2).c_str());
                this->minute=atoi(timeString.substr(12,2).c_str());
                this->second=atoi(timeString.substr(15,2).c_str());            
                this->decimal=atof(timeString.substr(17,7).c_str());               
            }           
            this->precision=hour*3600+minute*60+second+decimal;
        }
        ///���ؼӺ������
        const double operator+(const StructTimePoint& time)
        {
            return this->precision+time.precision;
        }
        ///���ؼ��������
        const double operator-(const StructTimePoint& time)
        {
            return this->precision-time.precision;
        }
        ///���ش��ڷ�������
        bool operator>(const StructTimePoint& time)
        {
            return this->precision>time.precision;
        }
        ///���ش��ڵ��ڷ�������
        bool operator>=(const StructTimePoint& time)
        {
            return this->precision>=time.precision;
        }
        ///����С�ڷ�������
        bool operator<(const StructTimePoint& time)
        {
            return this->precision<time.precision;
        }
        ///����С�ڵ��ڷ�������
        bool operator<=(const StructTimePoint& time)
        {
            return this->precision<=time.precision;
        }
    };

    //�����ڽṹ��
    //struct StructWindow                           // window file={1,N,1,N}
    //{
    //  unsigned int    linelo,                 // min. line coord.
    //                  linehi,                 // max. line coord.
    //                  pixlo,                  // min. pix coord.
    //                  pixhi;                  // max. pix coord.333

    
    
    ///���򳤶���ṹ��
    struct StructEllipse                        
    {
        string name;                            ///<�ο���������
        double major;                           ///<����
        double minor;                           ///<����        
        StructEllipse(){}
        ///���캯��
        StructEllipse(string lpValue){          //lpValue = WGS84 6.378137000000000e+06 6.356752314245179e+06
            /*int start=0,end=lpValue.find(' ',start);
            
            name=lpValue.substr(start,end);            
            start=end+1;end=lpValue.find(' ',start);major=atof(lpValue.substr(start,end));
            start=end+1;minor=atof(lpValue.substr(start));*/

            // int end  = lpValue.ReverseFind(' ');
            int end  = lpValue.find_last_of(' ');
            // minor = atof(lpValue.Right(lpValue.GetLength()-end));
            // FIXME: may have problem
            minor = atof(lpValue.substr(end+1, (lpValue.length()-end)).c_str());
            // lpValue = lpValue.Left(end);
            lpValue = lpValue.substr(0, end);
            end = lpValue.find_last_of(' ');
            major = atof(lpValue.substr(end+1, (lpValue.length()-end)).c_str());
            name = lpValue.substr(0, end);
        }
    };

    ///���Ƿ�λ��Ϣ�ṹ��
    struct StructOrientation                    
    {
        string windowName;                      ///<��������
        double windowCoefficient;               ///<����ϵ��
        int numberOfLooks;                      ///<����
        double looksBandWidth;                  ///<��������
        double totalProcessedBandWidth;         ///<����        

        StructOrientation(){}
        ///Ĭ�Ϲ��캯��
        StructOrientation(string lpValue)       //lpValue = Kaiser 2.400000100000000e+000 1.500000000000000e+003 1 1.500000000000000e+003
        {
            int start=0,end=lpValue.find(' ',start);
            windowName=lpValue.substr(start,end);
            start=end+1;end=lpValue.find(' ',start);windowCoefficient=atof(lpValue.substr(start,end).c_str());
            start=end+1;end=lpValue.find(' ',start);numberOfLooks=atoi(lpValue.substr(start,end).c_str());
            start=end+1;end=lpValue.find(' ',start);looksBandWidth=atof(lpValue.substr(start,end).c_str());
            start=end+1;totalProcessedBandWidth=atof(lpValue.substr(start).c_str());
        }
    };

    ///����״̬�ṹ��
    struct StructStateVector                    
    {
    
        StructTimePoint timePoint;              ///<ʱ��        
        double xPosition;                       ///<x����
        double yPosition;                       ///<y����
        double zPosition;                       ///<z����

        double xVelocity;                       ///<x�����ٶ�
        double yVelocity;                       ///<y�����ٶ�
        double zVelocity;                       ///<z�����ٶ�

        StructStateVector(){};
        ///���캯��
        StructStateVector(string lpValue) //lpValue = time 0.000000000000000e+000 0.000000000000000e+000 0.000000000000000e+000 0.000000000000000e+000 0.000000000000000e+000 0.000000000000000e+000|
        {
            int start=0,end=lpValue.find(' ',start);
            // end-1 seems to be wrong
            //timePoint=StructTimePoint(lpValue.substr(start,end-1));
            timePoint=StructTimePoint(lpValue.substr(start,end));
            start=end+1;end=lpValue.find(' ',start);xPosition=atof(lpValue.substr(start,end).c_str());
            start=end+1;end=lpValue.find(' ',start);yPosition=atof(lpValue.substr(start,end).c_str());
            start=end+1;end=lpValue.find(' ',start);zPosition=atof(lpValue.substr(start,end).c_str());
            
            start=end+1;end=lpValue.find(' ',start);xVelocity=atof(lpValue.substr(start,end).c_str());
            start=end+1;end=lpValue.find(' ',start);yVelocity=atof(lpValue.substr(start,end).c_str());
            start=end+1;zVelocity=atof(lpValue.substr(start).c_str());
        }   
    };

    ///���Ƶ�ṹ��
    struct StructGeodeticCoordinate             
    {
        double longitude;                       ///<����
        double latitude;                        ///<γ��        
        double height;                          ///<�߶�
        StructGeodeticCoordinate(){}
        ///���캯��
        StructGeodeticCoordinate(string lpValue)
        {
            int start=0,end=lpValue.find(' ',start);            
            longitude=atof(lpValue.substr(start,end).c_str());
            start=end+1;end=lpValue.find(' ',start);latitude=atof(lpValue.substr(start,end).c_str());
            start=end+1;height=atof(lpValue.substr(start).c_str());
        }
    };
    
    ///��׼��Ϣ
    struct StructRegistration                   
    {
        bool isRegistr;                         ///<���ͼ���Ƿ��Ѿ���׼��
        int registrType;                        ///<��׼���� 0 ��ƥ��  1��ƥ��
        double rangeOffset;                     ///<�������ƫ��
        double azimuthOffst;                    ///<��λ��ƫ��
    };

    //struct StructLog
    //{
    //  string datetime;                        //����ʱ��  
    //  string handle;                          //��������
    //};
// ͼ����imageTiePoint����Ϣ
    struct structImageTiePoint
    {
        float Row;                               //�к�
        float Clm;                               //�к�
        float longitude;                      //
        float latitude;
    };

    // ���к��뾭γ��֮��Ĺ�ϵ


    struct structLineToLat
    {

        // lat=b1*X^2+b2*X+b3*X*Y+b4*Y+b5*Y^2+b6;
        float b[6];
        // ���캯��
        structLineToLat(){}
        structLineToLat(string lpValue)
       {
            int start=0,end=lpValue.find(' ',start);
            b[0]=atof(lpValue.substr(start,end).c_str());
            start=end+1;end=lpValue.find(' ',start);b[1]=atof(lpValue.substr(start,end).c_str());
            start=end+1;end=lpValue.find(' ',start);b[2]=atof(lpValue.substr(start,end).c_str());
            start=end+1;end=lpValue.find(' ',start);b[3]=atof(lpValue.substr(start,end).c_str());
            start=end+1;end=lpValue.find(' ',start);b[4]=atof(lpValue.substr(start,end).c_str());
            start=end+1;b[5]=atof(lpValue.substr(start).c_str());
       }
    };

    struct structSampleToLong
    {
        float a[6];
        structSampleToLong(){}
        structSampleToLong(string lpValue){
            int start=0,end=lpValue.find(' ',start);
            a[0]=atof(lpValue.substr(start,end).c_str());
            start=end+1;end=lpValue.find(' ',start);a[1]=atof(lpValue.substr(start,end).c_str());
            start=end+1;end=lpValue.find(' ',start);a[2]=atof(lpValue.substr(start,end).c_str());
            start=end+1;end=lpValue.find(' ',start);a[3]=atof(lpValue.substr(start,end).c_str());
            start=end+1;end=lpValue.find(' ',start);a[4]=atof(lpValue.substr(start,end).c_str());
            start=end+1;a[5]=atof(lpValue.substr(start).c_str());
        }
    };

public:
    
    int Sample;                                             ///<ͼ������
    int Line;                                               ///<ͼ������
    int SampleOri;                                          ///����ͼ���ʼ����
    int LineOri;                                            ///����ͼ���ʼ����
    int Band;                                               ///<ͼ�񲨶���
    ENUM_DATA_TYPE DataType;                                ///<ͼ����������  ��GDTö�ٶ�Ӧ
    double Wavelength;                                      ///<����
    
    ENUM_SENSOR_TYPE Sensor;                                ///<����������      
    double PulseRepetitionFrequency;                        ///<��λ������� ��ͼ���ظ�����Ƶ��(Hz)��           
    double Rsamplrate2x;                                    ///<˫�̾��������Ƶ��(Hz)  

    StructEllipse Ellipse;                                  ///<���򳤶������

    double SampledPixelSpacing;                             ///<����ֱ���
    double SampledLineSpacing;                              ///<��λ�ֱ���

    StructOrientation Azimuth;                              ///<��λ�����
    StructOrientation Range;                                ///<���������
    vector<StructStateVector> StateVector;                  ///<����״̬ʸ���б�  ����      
    
    double IncidenceAngleNearRange;                         ///<���������;
    double IncidenceAngleFarRange;                          ///<Զ�������;
    double SatelliteHeight;                                 ///<���Ǹ߶�

    StructGeodeticCoordinate GeodeticCoordinateFirst;       ///<��һ�п��Ƶ���Ϣ
    StructGeodeticCoordinate GeodeticCoordinateCenter;      ///<���ĵ���Ƶ���Ϣ
    StructGeodeticCoordinate GeodeticCoordinateLast;        ///<���һ�п��Ƶ���Ϣ

    StructTimePoint ImagingTime;                            ///<����ʱ��
    double RadarCenterFrequency;                            ///<�״�Ƶ��
    ENUM_DIRECTION_TYPE PassDirection;                      ///<������

    StructTimePoint ZeroDopplerTimeFirstLine;               ///<��һ��λ���������ʱ��  
    StructTimePoint ZeroDopplerTimeLastLine;                ///<���λ���������ʱ��  
    double DopplerRateReferenceTime;                        ///<�����������վ���ʱ��
    
    double AdcSamplingRate;                                 ///<�������
    
    double SlantRangeTimeToFirstRangeSample;                ///<б���һ������ʱ��
    /*
    *   ����Ϊ��������в�������
    */

    StructRegistration Registration;                        ///<��׼��Ϣ

    int aLooks;
    int rLooks;

    int leftUpC;                                            ///�ü�ͼ��ʱ���Ͻ����꣬�У���1��ʼ
    int leftUpL;                                            ///

// ���кŵ���γ��ת����ʽ
    structLineToLat  LineToLat;
    structSampleToLong sampleToLong;

    /***************************************************************************************************
     *      ����Ϊ������
     */
    //StructWindow WindowOriginal;                          ///ԭʼ���ڴ�С
    //StructWindow WindowCurrent;                           ///�����ڴ�С


    /// <summary>
    /// ��������������֮���Զ�ת��
    /// </summary>
    ENUM_DATA_TYPE DataTypeToggle();
    

    /// <summary>
    /// �ṩ�������͵��ַ���ת��
    /// </summary>
    string DataType2String();
};

