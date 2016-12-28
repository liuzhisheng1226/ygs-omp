#include "RMGHeader.h"
#include <fstream>
#include "RdWrIni.h"
#include <sstream>
#include <cstdio>


CRMGHeader::CRMGHeader(void):CONST_SECTION_HEADER("HEADER"),CONST_SECTION_EXTENTION("EXTENTION")
{
}

CRMGHeader::~CRMGHeader(void)
{
}

void CRMGHeader::del_char(char *str,char c)
{
    //printf("in del \n");
    int i=0,j=0;
    while(str[i]!='\0'){
        if(str[i] != c){

            str[j++] = str[i++];

        }
        else{

            i++;

        }

    }
    str[j] = '\0';

    //printf("out del \n");
}


bool CRMGHeader::Save(string lpFilename)
{   
    cout << "CRMGHeader::Save()\n";
    //格式化临时变量
    char lpValue[1000]; 

// #pragma region Basic Header Infomation
    //记录最初始图像的大小
    sprintf(lpValue, "%d", this->SampleOri);
    WritePrivateProfileString(CONST_SECTION_HEADER,"oriSample",lpValue, lpFilename.c_str());
     sprintf(lpValue, "%d", this->LineOri);
    WritePrivateProfileString(CONST_SECTION_HEADER,"oriLine",lpValue, lpFilename.c_str());
    sprintf(lpValue, "%d", this->Sample);
    WritePrivateProfileString(CONST_SECTION_HEADER,"Sample",lpValue, lpFilename.c_str());
    sprintf(lpValue, "%d", this->Line);
    WritePrivateProfileString(CONST_SECTION_HEADER,"Line",lpValue, lpFilename.c_str());
    sprintf(lpValue, "%d", this->Band);
    WritePrivateProfileString(CONST_SECTION_HEADER,"Band",lpValue, lpFilename.c_str());
    sprintf(lpValue, "%d", this->DataType);
    WritePrivateProfileString(CONST_SECTION_HEADER,"DataType",lpValue, lpFilename.c_str());
    sprintf(lpValue, "%.15e", this->Wavelength);
    WritePrivateProfileString(CONST_SECTION_HEADER,"Wavelength",lpValue, lpFilename.c_str());
    sprintf(lpValue, "%d", this->Sensor);
    WritePrivateProfileString(CONST_SECTION_HEADER,"Sensor",lpValue, lpFilename.c_str());    
    sprintf(lpValue, "%.15e", this->PulseRepetitionFrequency);
    WritePrivateProfileString(CONST_SECTION_HEADER,"PulseRepetitionFrequency",lpValue, lpFilename.c_str());  
    sprintf(lpValue, "%.15e", this->Rsamplrate2x);
    WritePrivateProfileString(CONST_SECTION_HEADER,"Rsamplrate2x",lpValue, lpFilename.c_str());
    sprintf(lpValue, "%s %.15e %.15e", Ellipse.name.c_str(), Ellipse.major, Ellipse.minor);
    WritePrivateProfileString(CONST_SECTION_HEADER,"Ellipse",lpValue, lpFilename.c_str());
    sprintf(lpValue, "%.15e", this->SampledPixelSpacing);
    WritePrivateProfileString(CONST_SECTION_HEADER,"SampledPixelSpacing",lpValue, lpFilename.c_str());
    sprintf(lpValue, "%.15e", this->SampledLineSpacing);
    WritePrivateProfileString(CONST_SECTION_HEADER,"SampledLineSpacing",lpValue, lpFilename.c_str());    
// #pragma endregion Basic Header Infomation
// #pragma region Orientation

    sprintf(lpValue, "%s %.15e %d %.15e %.15e", Azimuth.windowName.c_str(), Azimuth.windowCoefficient, Azimuth.numberOfLooks, Azimuth.looksBandWidth, Azimuth.totalProcessedBandWidth);
    WritePrivateProfileString(CONST_SECTION_HEADER,"Azimuth",lpValue, lpFilename.c_str());

    sprintf(lpValue, "%s %.15e %d %.15e %.15e", Range.windowName.c_str(), Range.windowCoefficient, Range.numberOfLooks, Range.looksBandWidth, Range.totalProcessedBandWidth);
    WritePrivateProfileString(CONST_SECTION_HEADER,"Range",lpValue, lpFilename.c_str());
    
// #pragma endregion Orientation

// #pragma region StateVector
    string lpBuffer;
    for(int i=0;i<StateVector.size();i++)
    {
         sprintf(lpValue, "%s%s %.15e %.15e %.15e %.15e %.15e %.15e|",lpBuffer.c_str(),StateVector[i].timePoint.timeString.c_str(),
             StateVector[i].xPosition,StateVector[i].yPosition,StateVector[i].zPosition,
             StateVector[i].xVelocity,StateVector[i].yVelocity,StateVector[i].zVelocity);
         
        lpBuffer = lpValue; 
    }   
    WritePrivateProfileString(CONST_SECTION_HEADER,"StateVector",lpValue,lpFilename.c_str());
// #pragma endregion StateVector

    sprintf(lpValue, "%.15e", this->IncidenceAngleNearRange);
    WritePrivateProfileString(CONST_SECTION_HEADER,"IncidenceAngleNearRange",lpValue,lpFilename.c_str());
    sprintf(lpValue, "%.15e", this->IncidenceAngleFarRange);
    WritePrivateProfileString(CONST_SECTION_HEADER,"IncidenceAngleFarRange",lpValue, lpFilename.c_str());
    sprintf(lpValue, "%.15e", this->SatelliteHeight);
    WritePrivateProfileString(CONST_SECTION_HEADER,"SatelliteHeight",lpValue, lpFilename.c_str());
    
// #pragma region GeodeticCoordinate
    sprintf(lpValue, "%.15e %.15e %.15e", GeodeticCoordinateFirst.longitude, GeodeticCoordinateFirst.latitude, GeodeticCoordinateFirst.height);
    WritePrivateProfileString(CONST_SECTION_HEADER,"GeodeticCoordinate.first",lpValue,lpFilename.c_str());
    sprintf(lpValue, "%.15e %.15e %.15e", GeodeticCoordinateCenter.longitude, GeodeticCoordinateCenter.latitude, GeodeticCoordinateCenter.height);
    WritePrivateProfileString(CONST_SECTION_HEADER,"GeodeticCoordinate.center",lpValue, lpFilename.c_str());
    sprintf(lpValue, "%.15e %.15e %.15e", GeodeticCoordinateLast.longitude, GeodeticCoordinateLast.latitude, GeodeticCoordinateLast.height);
    WritePrivateProfileString(CONST_SECTION_HEADER,"GeodeticCoordinate.last",lpValue,lpFilename.c_str());

// #pragma endregion GeodeticCoordinate        
    WritePrivateProfileString(CONST_SECTION_HEADER,"ImagingTime",this->ImagingTime.timeString.c_str(),lpFilename.c_str());
    sprintf(lpValue, "%.15e", this->RadarCenterFrequency);
    WritePrivateProfileString(CONST_SECTION_HEADER,"RadarCenterFrequency",lpValue,lpFilename.c_str());
    sprintf(lpValue, "%d", (int)this->PassDirection);
    WritePrivateProfileString(CONST_SECTION_HEADER,"PassDirection",lpValue,lpFilename.c_str());
    WritePrivateProfileString(CONST_SECTION_HEADER,"ZeroDopplerTimeFirstLine",ZeroDopplerTimeFirstLine.timeString.c_str(),lpFilename.c_str());
    WritePrivateProfileString(CONST_SECTION_HEADER,"ZeroDopplerTimeLastLine",ZeroDopplerTimeLastLine.timeString.c_str(),lpFilename.c_str());
    sprintf(lpValue, "%.15e", this->DopplerRateReferenceTime);
    WritePrivateProfileString(CONST_SECTION_HEADER,"DopplerRateReferenceTime",lpValue,lpFilename.c_str());

    sprintf(lpValue, "%.15e", this->AdcSamplingRate);
    WritePrivateProfileString(CONST_SECTION_HEADER,"AdcSamplingRate",lpValue,lpFilename.c_str());   
    
    sprintf(lpValue, "%.15e", this->SlantRangeTimeToFirstRangeSample);
    WritePrivateProfileString(CONST_SECTION_HEADER,"SlantRangeTimeToFirstRangeSample",lpValue,lpFilename.c_str());  

    //sprintf(lpValue, "%d", this->aLooks);
    //WritePrivateProfileString(CONST_SECTION_HEADER,"aLooks",lpValue,lpFilename.c_str());

    //sprintf(lpValue, "%d", this->rLooks);
    //WritePrivateProfileString(CONST_SECTION_HEADER,"rLooks",lpValue,lpFilename.c_str());

// #pragma region updateInfo
    sprintf(lpValue, "%d", Registration.registrType);
    WritePrivateProfileString(CONST_SECTION_EXTENTION,"RegistrationType",lpValue,lpFilename.c_str());
    sprintf(lpValue, "%.4f", Registration.azimuthOffst);
    WritePrivateProfileString(CONST_SECTION_EXTENTION,"RegistrationAziOffset",lpValue,lpFilename.c_str());
    sprintf(lpValue, "%.4f", Registration.rangeOffset);
    WritePrivateProfileString(CONST_SECTION_EXTENTION,"RegistrationRngOffset",lpValue,lpFilename.c_str());

    sprintf(lpValue, "%d", leftUpC);
    WritePrivateProfileString(CONST_SECTION_EXTENTION,"LeftUpCornCol",lpValue,lpFilename.c_str());
    sprintf(lpValue, "%d", leftUpL);
    WritePrivateProfileString(CONST_SECTION_EXTENTION,"leftUpCornLine",lpValue,lpFilename.c_str());


    //sprintf(lpValue, "%.15e %.15e %.15e %.15e %.15e %.15e",this->sampleToLong.a[0],this->sampleToLong.a[1],this->sampleToLong.a[2],
        //this->sampleToLong.a[3],this->sampleToLong.a[4],this->sampleToLong.a[5]);
    //WritePrivateProfileString(CONST_SECTION_EXTENTION,"longCoef",lpValue,lpFilename.c_str());

    //sprintf(lpValue, "%.15e %.15e %.15e %.15e %.15e %.15e",this->LineToLat.b[0],this->LineToLat.b[1],this->LineToLat.b[2],
        //this->LineToLat.b[3],this->LineToLat.b[4],this->LineToLat.b[5]);
    //WritePrivateProfileString(CONST_SECTION_EXTENTION,"latiCoef",lpValue,lpFilename.c_str());

// #pragma endregion updateInfo

    return true;
}

bool CRMGHeader::Load(string lpFilename)
{
    cout << "CRMGHeader::Load()\n";
    //窗口结构体
    /*this->WindowOriginal.pixlo=0;
    this->WindowOriginal.linelo=0;

    this->WindowCurrent.pixlo=0;
    this->WindowCurrent.linelo=0;*/
    
    const int buffer_size=1024*3;
    char buffer[buffer_size];
    string lpValue;

// #pragma region Basic Header Infomation
    GetPrivateProfileString(CONST_SECTION_HEADER,"oriSample",0,buffer,buffer_size,lpFilename.c_str());this->SampleOri=atoi(buffer);
    cout << "buffer is " << buffer << endl;
    GetPrivateProfileString(CONST_SECTION_HEADER,"oriLine",0,buffer,buffer_size,lpFilename.c_str());this->LineOri=atoi(buffer);
    GetPrivateProfileString(CONST_SECTION_HEADER,"Sample",0,buffer,buffer_size,lpFilename.c_str());this->Sample=atoi(buffer);
    GetPrivateProfileString(CONST_SECTION_HEADER,"Line",0,buffer,buffer_size,lpFilename.c_str());this->Line=atoi(buffer);
    GetPrivateProfileString(CONST_SECTION_HEADER,"Band",0,buffer,buffer_size,lpFilename.c_str());this->Band=atoi(buffer);
    GetPrivateProfileString(CONST_SECTION_HEADER,"DataType",0,buffer,buffer_size,lpFilename.c_str());this->DataType=(ENUM_DATA_TYPE)atoi(buffer);
    GetPrivateProfileString(CONST_SECTION_HEADER,"Wavelength",0,buffer,buffer_size,lpFilename.c_str());this->Wavelength=atof(buffer);
    GetPrivateProfileString(CONST_SECTION_HEADER,"Sensor",0,buffer,buffer_size,lpFilename.c_str()); this->Sensor=(ENUM_SENSOR_TYPE)atoi(buffer);    
    GetPrivateProfileString(CONST_SECTION_HEADER,"PulseRepetitionFrequency",0,buffer,buffer_size,lpFilename.c_str());this->PulseRepetitionFrequency=atof(buffer);
    GetPrivateProfileString(CONST_SECTION_HEADER,"Rsamplrate2x",0,buffer,buffer_size,lpFilename.c_str());this->Rsamplrate2x=atof(buffer);
    GetPrivateProfileString(CONST_SECTION_HEADER,"Ellipse",0,buffer,buffer_size,lpFilename.c_str());this->Ellipse=StructEllipse(buffer);

    GetPrivateProfileString(CONST_SECTION_HEADER,"SampledPixelSpacing",0,buffer,buffer_size,lpFilename.c_str());this->SampledPixelSpacing=atof(buffer);
    GetPrivateProfileString(CONST_SECTION_HEADER,"SampledLineSpacing",0,buffer,buffer_size,lpFilename.c_str());this->SampledLineSpacing=atof(buffer);
// #pragma endregion Basic Header Infomation
// #pragma region Orientation
    
    GetPrivateProfileString(CONST_SECTION_HEADER,"Azimuth",0,buffer,buffer_size,lpFilename.c_str());this->Azimuth=StructOrientation(buffer);
    GetPrivateProfileString(CONST_SECTION_HEADER,"Range",0,buffer,buffer_size,lpFilename.c_str());this->Range=StructOrientation(buffer);

// #pragma endregion Orientation   
// #pragma region StateVector
    GetPrivateProfileString(CONST_SECTION_HEADER,"StateVector",0,buffer,buffer_size,lpFilename.c_str());
    lpValue=buffer;
    
    int start=0,end=lpValue.find('|');
    while(end>0)
    {       
        StructStateVector stateVector=StructStateVector(lpValue.substr(start,end-start));
        StateVector.push_back(stateVector);
        start=end+1;
        end=lpValue.find('|',start);
    }
// #pragma endregion StateVector
    GetPrivateProfileString(CONST_SECTION_HEADER,"IncidenceAngleNearRange",0,buffer,buffer_size,lpFilename.c_str());this->IncidenceAngleNearRange=atof(buffer);
    GetPrivateProfileString(CONST_SECTION_HEADER,"IncidenceAngleFarRange",0,buffer,buffer_size,lpFilename.c_str());this->IncidenceAngleFarRange=atof(buffer);
    GetPrivateProfileString(CONST_SECTION_HEADER,"SatelliteHeight",0,buffer,buffer_size,lpFilename.c_str());this->SatelliteHeight=atof(buffer);
// #pragma region GeodeticCoordinate
    GetPrivateProfileString(CONST_SECTION_HEADER,"GeodeticCoordinate.first",0,buffer,buffer_size,lpFilename.c_str());this->GeodeticCoordinateFirst=StructGeodeticCoordinate(buffer);
    GetPrivateProfileString(CONST_SECTION_HEADER,"GeodeticCoordinate.center",0,buffer,buffer_size,lpFilename.c_str());this->GeodeticCoordinateCenter=StructGeodeticCoordinate(buffer);
    GetPrivateProfileString(CONST_SECTION_HEADER,"GeodeticCoordinate.last",0,buffer,buffer_size,lpFilename.c_str());this->GeodeticCoordinateLast=StructGeodeticCoordinate(buffer);
// #pragma endregion GeodeticCoordinate
    GetPrivateProfileString(CONST_SECTION_HEADER,"ImagingTime",0,buffer,buffer_size,lpFilename.c_str());this->ImagingTime=StructTimePoint(buffer);
    GetPrivateProfileString(CONST_SECTION_HEADER,"PassDirection",0,buffer,buffer_size,lpFilename.c_str());this->PassDirection=(ENUM_DIRECTION_TYPE)atoi(buffer);
    GetPrivateProfileString(CONST_SECTION_HEADER,"ZeroDopplerTimeFirstLine",0,buffer,buffer_size,lpFilename.c_str());this->ZeroDopplerTimeFirstLine=StructTimePoint(buffer);
    GetPrivateProfileString(CONST_SECTION_HEADER,"ZeroDopplerTimeLastLine",0,buffer,buffer_size,lpFilename.c_str());this->ZeroDopplerTimeLastLine=StructTimePoint(buffer);
    GetPrivateProfileString(CONST_SECTION_HEADER,"DopplerRateReferenceTime",0,buffer,buffer_size,lpFilename.c_str());this->DopplerRateReferenceTime=atof(buffer);
    
    GetPrivateProfileString(CONST_SECTION_HEADER,"AdcSamplingRate",0,buffer,buffer_size,lpFilename.c_str());this->AdcSamplingRate=atof(buffer); 
    GetPrivateProfileString(CONST_SECTION_HEADER,"SlantRangeTimeToFirstRangeSample",0,buffer,buffer_size,lpFilename.c_str());this->SlantRangeTimeToFirstRangeSample=atof(buffer);   
    GetPrivateProfileString(CONST_SECTION_HEADER,"RadarCenterFrequency",0,buffer,buffer_size,lpFilename.c_str());this->RadarCenterFrequency=atof(buffer);   

    double senseTime = abs(this->ZeroDopplerTimeLastLine.precision - this->ZeroDopplerTimeFirstLine.precision)/(this->LineOri-1);

    this->PulseRepetitionFrequency = 1.0/senseTime;
    //GetPrivateProfileString(CONST_SECTION_HEADER,"aLooks",0,buffer,buffer_size,lpFilename.c_str());this->aLooks=atoi(buffer);
    //GetPrivateProfileString(CONST_SECTION_HEADER,"rLooks",0,buffer,buffer_size,lpFilename.c_str());this->rLooks=atoi(buffer);

// #pragma region updateInfo
    GetPrivateProfileString(CONST_SECTION_EXTENTION,"RegistrationType",0,buffer,buffer_size,lpFilename.c_str());this->Registration.registrType =atoi(buffer);
    GetPrivateProfileString(CONST_SECTION_EXTENTION,"RegistrationAziOffset",0,buffer,buffer_size,lpFilename.c_str());this->Registration.azimuthOffst =atof(buffer);
    GetPrivateProfileString(CONST_SECTION_EXTENTION,"RegistrationRngOffset",0,buffer,buffer_size,lpFilename.c_str());this->Registration.rangeOffset =atof(buffer);

    GetPrivateProfileString(CONST_SECTION_EXTENTION,"LeftUpCornCol",0,buffer,buffer_size,lpFilename.c_str());this->leftUpC =atoi(buffer);
    GetPrivateProfileString(CONST_SECTION_EXTENTION,"LeftUpCornLine",0,buffer,buffer_size,lpFilename.c_str());this->leftUpL =atoi(buffer);

    //GetPrivateProfileString(CONST_SECTION_EXTENTION,"longCoef",0,buffer,buffer_size,lpFilename.c_str());this->sampleToLong =structSampleToLong(buffer);
    //GetPrivateProfileString(CONST_SECTION_EXTENTION,"latiCoef",0,buffer,buffer_size,lpFilename.c_str());this->LineToLat=structLineToLat(buffer);

// #pragma  endregion updateInfo
    
    return true;
}


ENUM_DATA_TYPE CRMGHeader::DataTypeToggle()
{
    ENUM_DATA_TYPE datatype=ENUM_DATA_TYPE::eBYTE;
    switch (this->DataType)
    {
    case ENUM_DATA_TYPE::eINT16:    datatype=ENUM_DATA_TYPE::eCINT16;break;
    case ENUM_DATA_TYPE::eINT32:    datatype=ENUM_DATA_TYPE::eCINT32;break;
    case ENUM_DATA_TYPE::eFLOAT32:  datatype=ENUM_DATA_TYPE::eCFLOAT32;break;
    case ENUM_DATA_TYPE::eFLOAT64:  datatype=ENUM_DATA_TYPE::eCFLOAT64;break;

    case ENUM_DATA_TYPE::eCINT16:   datatype=ENUM_DATA_TYPE::eINT16;break;
    case ENUM_DATA_TYPE::eCINT32:   datatype=ENUM_DATA_TYPE::eINT32;break;
    case ENUM_DATA_TYPE::eCFLOAT32: datatype=ENUM_DATA_TYPE::eFLOAT32;break;
    case ENUM_DATA_TYPE::eCFLOAT64: datatype=ENUM_DATA_TYPE::eFLOAT64;break;
    }
    return datatype;
}


string CRMGHeader::DataType2String()
{   
    string lpDataType;
    char buff[50];
    switch (this->DataType)
    {
    case ENUM_DATA_TYPE::eBYTE:     sprintf(buff,"Byte(%d)",sizeof(unsigned char));break;   
    case ENUM_DATA_TYPE::eUINT16:   sprintf(buff,"Unsigned Int16(%d)",sizeof(unsigned short));break;   
    case ENUM_DATA_TYPE::eINT16:    sprintf(buff,"Int16(%d)",sizeof(short));break;
    case ENUM_DATA_TYPE::eUINT32:   sprintf(buff,"Unsigned Int32(%d)",sizeof(unsigned int));break;       
    case ENUM_DATA_TYPE::eINT32:    sprintf(buff,"Int32(%d)",sizeof(int));break;
    case ENUM_DATA_TYPE::eFLOAT32:  sprintf(buff,"Float32(%d)",sizeof(float));break;
    case ENUM_DATA_TYPE::eFLOAT64:  sprintf(buff,"Float64(%d)",sizeof(double));break;
    case ENUM_DATA_TYPE::eCINT16:   sprintf(buff,"Complex Int16(%d)",sizeof(complex<short>));break;
    case ENUM_DATA_TYPE::eCINT32:   sprintf(buff,"Complex Int32(%d)",sizeof(complex<int>));break;
    case ENUM_DATA_TYPE::eCFLOAT32: sprintf(buff,"Complex Float32(%d)",sizeof(complex<float>));break;
    case ENUM_DATA_TYPE::eCFLOAT64: sprintf(buff,"Complex Float64(%d)",sizeof(complex<double>));break;
    }
    lpDataType = buff;
    return lpDataType;
}
