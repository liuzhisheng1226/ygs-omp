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
/// @brief RMGHeader.h文件 定义了RMG图像相关参数


/// 数据类型接口体
enum ENUM_DATA_TYPE
{
    eUNKNOW,            ///<未知类型
    eBYTE,              ///<BYTE类型[1字节]
    eUINT16,            ///<unsigned short类型[2字节]
    eINT16,             ///<signed short类型[2字节]
    eUINT32,            ///<unsigned int类型[4字节]
    eINT32,             ///<signed int类型[4字节]
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
/// RMG图像头文件类
/// </summary>
/// <remarks>定义了RMG图像处理过程中需要用到的所有卫星参数、和头文件读取方法</remarks>
/// @ingroup Core
class CRMGHeader
{



public:
    CRMGHeader(void);
    ~CRMGHeader(void);
    
    /// <summary>
    /// 存储头文件
    /// </summary>
    /// <param name="lpFilename">头文件名称</param>
    /// <returns>函数执行结果</returns>
    bool Save(string lpFilename);

    
    /// <summary>
    /// 加载头文件
    /// </summary>
    /// <param name="lpFilename">头文件名称</param>
    /// <returns>函数执行结果</returns>
    bool Load(string lpFilename);
    void del_char(char *str,char c);

private:    
    const char *CONST_SECTION_HEADER;               ///<头文件配置节名称
    const char *CONST_SECTION_EXTENTION;            ///<头文件配置扩展名称
    

public:
    
    ///时间点结构体
    struct StructTimePoint                  
    {
        string timeString;                          /*<日期时间字符串*/
        short year;                                 /*<年*/
        short month;                                /*<月*/
        short day;                                  //<日
        short hour;                                 //<小时
        short minute;                               //<分钟
        short second;                               //<秒
        double decimal;                             //<小数部分
        double precision;                           //<格式化的时间（hour*3600+minute*60+second+decimal）
        
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
        ///构造函数
        StructTimePoint(string lpValue)                 
        {                                           
            //          0123456789ABCDEFGHIJKLMNOPQ         
            this->timeString=lpValue;
            // string timeText=lpValue.GetBuffer();
            string timeText=lpValue;
            //根据时间字符串长度来判断时间类型(不严密，最好改成正则表达式)
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
                //计算 月、日 信息需要考虑闰年和每月的天数 这里忽略计算
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
        ///重载加号运算符
        const double operator+(const StructTimePoint& time)
        {
            return this->precision+time.precision;
        }
        ///重载减号运算符
        const double operator-(const StructTimePoint& time)
        {
            return this->precision-time.precision;
        }
        ///重载大于符号运算
        bool operator>(const StructTimePoint& time)
        {
            return this->precision>time.precision;
        }
        ///重载大于等于符号运算
        bool operator>=(const StructTimePoint& time)
        {
            return this->precision>=time.precision;
        }
        ///重载小于符号运算
        bool operator<(const StructTimePoint& time)
        {
            return this->precision<time.precision;
        }
        ///重载小于等于符号运算
        bool operator<=(const StructTimePoint& time)
        {
            return this->precision<=time.precision;
        }
    };

    //处理窗口结构体
    //struct StructWindow                           // window file={1,N,1,N}
    //{
    //  unsigned int    linelo,                 // min. line coord.
    //                  linehi,                 // max. line coord.
    //                  pixlo,                  // min. pix coord.
    //                  pixhi;                  // max. pix coord.333

    
    
    ///椭球长短轴结构体
    struct StructEllipse                        
    {
        string name;                            ///<参考椭球名称
        double major;                           ///<长轴
        double minor;                           ///<短轴        
        StructEllipse(){}
        ///构造函数
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

    ///卫星方位信息结构体
    struct StructOrientation                    
    {
        string windowName;                      ///<窗口名称
        double windowCoefficient;               ///<窗口系数
        int numberOfLooks;                      ///<视数
        double looksBandWidth;                  ///<视数带宽
        double totalProcessedBandWidth;         ///<带宽        

        StructOrientation(){}
        ///默认构造函数
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

    ///卫星状态结构体
    struct StructStateVector                    
    {
    
        StructTimePoint timePoint;              ///<时间        
        double xPosition;                       ///<x坐标
        double yPosition;                       ///<y坐标
        double zPosition;                       ///<z坐标

        double xVelocity;                       ///<x方向速度
        double yVelocity;                       ///<y方向速度
        double zVelocity;                       ///<z方向速度

        StructStateVector(){};
        ///构造函数
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

    ///控制点结构体
    struct StructGeodeticCoordinate             
    {
        double longitude;                       ///<经度
        double latitude;                        ///<纬度        
        double height;                          ///<高度
        StructGeodeticCoordinate(){}
        ///构造函数
        StructGeodeticCoordinate(string lpValue)
        {
            int start=0,end=lpValue.find(' ',start);            
            longitude=atof(lpValue.substr(start,end).c_str());
            start=end+1;end=lpValue.find(' ',start);latitude=atof(lpValue.substr(start,end).c_str());
            start=end+1;height=atof(lpValue.substr(start).c_str());
        }
    };
    
    ///配准信息
    struct StructRegistration                   
    {
        bool isRegistr;                         ///<标记图像是否已经配准过
        int registrType;                        ///<配准类型 0 粗匹配  1精匹配
        double rangeOffset;                     ///<距离向便偏移
        double azimuthOffst;                    ///<方位向偏移
    };

    //struct StructLog
    //{
    //  string datetime;                        //日期时间  
    //  string handle;                          //操作名称
    //};
// 图像中imageTiePoint的信息
    struct structImageTiePoint
    {
        float Row;                               //行号
        float Clm;                               //列号
        float longitude;                      //
        float latitude;
    };

    // 行列号与经纬度之间的关系


    struct structLineToLat
    {

        // lat=b1*X^2+b2*X+b3*X*Y+b4*Y+b5*Y^2+b6;
        float b[6];
        // 构造函数
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
    
    int Sample;                                             ///<图像列数
    int Line;                                               ///<图像行数
    int SampleOri;                                          ///卫星图像初始列数
    int LineOri;                                            ///卫星图像初始行数
    int Band;                                               ///<图像波段数
    ENUM_DATA_TYPE DataType;                                ///<图像数据类型  与GDT枚举对应
    double Wavelength;                                      ///<波长
    
    ENUM_SENSOR_TYPE Sensor;                                ///<传感器类型      
    double PulseRepetitionFrequency;                        ///<方位向采样率 （图像重复采样频率(Hz)）           
    double Rsamplrate2x;                                    ///<双程距离向采样频率(Hz)  

    StructEllipse Ellipse;                                  ///<椭球长短轴参数

    double SampledPixelSpacing;                             ///<距离分辨率
    double SampledLineSpacing;                              ///<方位分辨率

    StructOrientation Azimuth;                              ///<方位向参数
    StructOrientation Range;                                ///<距离向参数
    vector<StructStateVector> StateVector;                  ///<卫星状态矢量列表  按照      
    
    double IncidenceAngleNearRange;                         ///<近距入射角;
    double IncidenceAngleFarRange;                          ///<远距入射角;
    double SatelliteHeight;                                 ///<卫星高度

    StructGeodeticCoordinate GeodeticCoordinateFirst;       ///<第一行控制点信息
    StructGeodeticCoordinate GeodeticCoordinateCenter;      ///<中心点控制点信息
    StructGeodeticCoordinate GeodeticCoordinateLast;        ///<最后一行控制点信息

    StructTimePoint ImagingTime;                            ///<成像时间
    double RadarCenterFrequency;                            ///<雷达频率
    ENUM_DIRECTION_TYPE PassDirection;                      ///<升降轨

    StructTimePoint ZeroDopplerTimeFirstLine;               ///<第一方位向零多普勒时间  
    StructTimePoint ZeroDopplerTimeLastLine;                ///<最后方位向零多普勒时间  
    double DopplerRateReferenceTime;                        ///<近距点零多普勒距离时间
    
    double AdcSamplingRate;                                 ///<采样间隔
    
    double SlantRangeTimeToFirstRangeSample;                ///<斜距第一距离向时间
    /*
    *   以下为处理过程中产生变量
    */

    StructRegistration Registration;                        ///<配准信息

    int aLooks;
    int rLooks;

    int leftUpC;                                            ///裁剪图像时左上角坐标，列，从1开始
    int leftUpL;                                            ///

// 行列号到经纬度转换公式
    structLineToLat  LineToLat;
    structSampleToLong sampleToLong;

    /***************************************************************************************************
     *      以下为计算量
     */
    //StructWindow WindowOriginal;                          ///原始窗口大小
    //StructWindow WindowCurrent;                           ///处理窗口大小


    /// <summary>
    /// 单、复数据类型之间自动转换
    /// </summary>
    ENUM_DATA_TYPE DataTypeToggle();
    

    /// <summary>
    /// 提供数据类型到字符串转换
    /// </summary>
    string DataType2String();
};

