#include "StdAfx.h"
#include "Rd2sp2Local.h"


CRd2sp2Local::CRd2sp2Local(void)
{
}


CRd2sp2Local::~CRd2sp2Local(void)
{
}


void CRd2sp2Local::Process()
{
    CDlgRd2sp2Rmg rd2Dlg;
    if(rd2Dlg.DoModal()==IDCANCEL) return;  
    Rd2spData2Rmg(
        rd2Dlg.m_lpDataImport,
        rd2Dlg.m_lpHeaderImport,
        rd2Dlg.m_lpDataExport,
        rd2Dlg.m_lpHeaderExport);

}
void CRd2sp2Local::Batch()
{
    Rd2spData2Rmg(
        m_aryStrs.GetAt(0),
        m_aryStrs.GetAt(1),
        m_aryStrs.GetAt(2),
        m_aryStrs.GetAt(3));    

}



CRMGHeader CRd2sp2Local::Rd2spHdr2Ldr(CString lpHeaderImport)
{   
    CXmlReader xmlReader(lpHeaderImport);
    CRMGHeader header;
    CString lpNodeValue,lpNodeTag;
    long nodeCount=0;

    lpNodeValue=xmlReader.GetValue(lpNodeTag);
    lpNodeValue=xmlReader.GetValue("//NS:ellipsoidName");header.Ellipse.name=lpNodeValue;                                               //参考椭球名称
    lpNodeValue=xmlReader.GetValue("//NS:semiMajorAxis");header.Ellipse.major=atof(lpNodeValue);                                        //参考椭球长半轴
    lpNodeValue=xmlReader.GetValue("//NS:semiMinorAxis");header.Ellipse.minor=atof(lpNodeValue);                                        //参考椭球短半轴
    lpNodeValue=xmlReader.GetValue("//NS:pulseRepetitionFrequency");header.PulseRepetitionFrequency=atof(lpNodeValue);                  //参考椭球短半轴
    lpNodeValue=xmlReader.GetValue("//NS:sampledPixelSpacing");header.SampledPixelSpacing=atof(lpNodeValue);                            //参考椭球短半轴
    lpNodeValue=xmlReader.GetValue("//NS:sampledLineSpacing");header.SampledLineSpacing=atof(lpNodeValue);                              //参考椭球短半轴
    
    lpNodeValue=xmlReader.GetValue("//NS:numberOfRangeLooks");header.Range.numberOfLooks=atoi(lpNodeValue);                         //距离向视数
    lpNodeValue=xmlReader.GetValue("//NS:rangeLookBandwidth");header.Range.looksBandWidth=atof(lpNodeValue);                            //距离向视数带宽
    lpNodeValue=xmlReader.GetValue("//NS:totalProcessedRangeBandwidth");header.Range.totalProcessedBandWidth=atof(lpNodeValue);     //距离向带宽
    lpNodeValue=xmlReader.GetValue("//NS:rangeWindow/NS:windowName");header.Range.windowName=lpNodeValue;                                   //距离向窗口名称
    lpNodeValut=xmlReader.GetValue("//NS:rangeWindow/NS:windowCoefficient");header.Range.windowCoefficient=atof(lpNodeValue);               //距离向窗口系数


    lpNodeValue=xmlReader.GetValue("//NS:numberOfAzimuthLooks");header.Azimuth.numberOfLooks=atoi(lpNodeValue);                     //方位向视数
    lpNodeValue=xmlReader.GetValue("//NS:azimuthLookBandwidth");header.Azimuth.looksBandWidth=atof(lpNodeValue);                        //方位向视数带宽
    lpNodeValue=xmlReader.GetValue("//NS:totalProcessedAzimuthBandwidth");header.Azimuth.totalProcessedBandWidth=atof(lpNodeValue); //方位向带宽
    lpNodeValue=xmlReader.GetValue("//NS:azimuthWindow/NS:windowName");header.Azimuth.windowName=lpNodeValue;                               //方位向窗口名称
    lpNodeValue=xmlReader.GetValue("//NS:azimuthWindow/NS:windowCoefficient");header.Azimuth.windowCoefficient=atof(lpNodeValue);           //方位向窗口系数
    
    lpNodeValue=xmlReader.GetValue("//NS:incidenceAngleNearRange");header.IncidenceAngleNearRange=atof(lpNodeValue);                    //近距入射角
    lpNodeValue=xmlReader.GetValue("//NS:incidenceAngleFarRange");header.IncidenceAngleFarRange=atof(lpNodeValue);                      //远距入射角
    lpNodeValue=xmlReader.GetValue("//NS:satelliteHeight");header.SatelliteHeight=atof(lpNodeValue);                                    //卫星高度

    //lpNodeValue=xmlReader.GetValue("//NS:geodeticCoordinate[1]/NS:latitude");header.GeodeticCoordinateFirst.latitude=atof(lpNodeValue);   //控制点纬度
    //lpNodeValue=xmlReader.GetValue("//NS:geodeticCoordinate[1]/NS:longitude");header.GeodeticCoordinateFirst.longitude=atof(lpNodeValue); //控制点经度
    //lpNodeValue=xmlReader.GetValue("//NS:geodeticCoordinate[1]/NS:height");header.GeodeticCoordinateFirst.height=atof(lpNodeValue);       //控制点高度
    
    nodeCount=xmlReader.GetCount("//NS:geodeticCoordinate");
    lpNodeValue=xmlReader.GetValue("//NS:imageTiePoint[1]/NS:geodeticCoordinate/NS:latitude");header.GeodeticCoordinateFirst.latitude=atof(lpNodeValue);    //第一行第一列控制点纬度
    lpNodeValue=xmlReader.GetValue("//NS:imageTiePoint[1]/NS:geodeticCoordinate/NS:longitude");header.GeodeticCoordinateFirst.longitude=atof(lpNodeValue);  //第一行第一列控制点经度
    lpNodeValue=xmlReader.GetValue("//NS:imageTiePoint[1]/NS:geodeticCoordinate/NS:height");header.GeodeticCoordinateFirst.height=atof(lpNodeValue);        //第一行第一列控制点高度


    lpNodeTag.Format("//NS:imageTiePoint[%d]/NS:geodeticCoordinate/NS:latitude",(nodeCount-1)/2+8); 
    lpNodeValue=xmlReader.GetValue(lpNodeTag);header.GeodeticCoordinateCenter.latitude=atof(lpNodeValue);                           //成像中心控制点纬度
    lpNodeTag.Format("//NS:imageTiePoint[%d]/NS:geodeticCoordinate/NS:longitude",(nodeCount-1)/2+8);
    lpNodeValue=xmlReader.GetValue(lpNodeTag);header.GeodeticCoordinateCenter.longitude=atof(lpNodeValue);                          //成像中心控制点纬度
    lpNodeTag.Format("//NS:imageTiePoint[%d]/NS:geodeticCoordinate/NS:height",(nodeCount-1)/2+8);
    lpNodeValue=xmlReader.GetValue(lpNodeTag);header.GeodeticCoordinateCenter.height=atof(lpNodeValue);                         //成像中心控制点纬度

    lpNodeValue=xmlReader.GetValue("//NS:imageTiePoint[last()]/NS:geodeticCoordinate/NS:latitude");header.GeodeticCoordinateLast.latitude=atof(lpNodeValue);    //最后行最后列控制点纬度
    lpNodeValue=xmlReader.GetValue("//NS:imageTiePoint[last()]/NS:geodeticCoordinate/NS:longitude");header.GeodeticCoordinateLast.longitude=atof(lpNodeValue);  //最后行最后列控制点经度
    lpNodeValue=xmlReader.GetValue("//NS:imageTiePoint[last()]/NS:geodeticCoordinate/NS:height");header.GeodeticCoordinateLast.height=atof(lpNodeValue);        //最后行最后列控制点高度

    lpNodeValue=xmlReader.GetValue("//NS:rawDataStartTime");header.ImagingTime=CRMGHeader::StructTimePoint(lpNodeValue);                                                //成像时间
    lpNodeValue=xmlReader.GetValue("//NS:radarCenterFrequency");header.RadarCenterFrequency=atof(lpNodeValue);                                                  //雷达频率
    
    lpNodeValue=xmlReader.GetValue("//NS:passDirection");
    header.PassDirection=lpNodeValue=="Ascending"?ENUM_DIRECTION_TYPE::ASCEND:ENUM_DIRECTION_TYPE::DESCEND;                                                     //升降轨

    lpNodeValue=xmlReader.GetValue("//NS:zeroDopplerTimeFirstLine");header.ZeroDopplerTimeFirstLine=CRMGHeader::StructTimePoint(lpNodeValue);                       //第一个方位像元点的零多普勒方位时间
    lpNodeValue=xmlReader.GetValue("//NS:zeroDopplerTimeLastLine");header.ZeroDopplerTimeLastLine=CRMGHeader::StructTimePoint(lpNodeValue);                         //最后方位像元点的零多普勒方位时间
    lpNodeValue=xmlReader.GetValue("//NS:dopplerRateReferenceTime");header.DopplerRateReferenceTime=atof(lpNodeValue);                                          //近距点零多普勒距离时间

    lpNodeValue=xmlReader.GetValue("//NS:adcSamplingRate");header.AdcSamplingRate=atof(lpNodeValue);                                                            //采样间隔
    
    lpNodeValue=xmlReader.GetValue("//NS:slantRangeTimeToFirstRangeSample");header.SlantRangeTimeToFirstRangeSample=atof(lpNodeValue);                                  //采样间隔
    for(int i=1;i<=5;i++)
    {   
        CRMGHeader::StructStateVector stateVector;
        lpNodeTag.Format("//NS:stateVector[%d]/NS:timeStamp",i);lpNodeValue=xmlReader.GetValue(lpNodeTag);stateVector.timePoint=CRMGHeader::StructTimePoint(lpNodeValue);   //记录时间
        lpNodeTag.Format("//NS:stateVector[%d]/NS:xPosition",i);lpNodeValue=xmlReader.GetValue(lpNodeTag);stateVector.xPosition=atof(lpNodeValue);                      //x平台位置
        lpNodeTag.Format("//NS:stateVector[%d]/NS:yPosition",i);lpNodeValue=xmlReader.GetValue(lpNodeTag);stateVector.yPosition=atof(lpNodeValue);                      //y平台位置
        lpNodeTag.Format("//NS:stateVector[%d]/NS:zPosition",i);lpNodeValue=xmlReader.GetValue(lpNodeTag);stateVector.zPosition=atof(lpNodeValue);                      //z平台位置
        lpNodeTag.Format("//NS:stateVector[%d]/NS:xVelocity",i);lpNodeValue=xmlReader.GetValue(lpNodeTag);stateVector.xVelocity=atof(lpNodeValue);                      //x平台速度
        lpNodeTag.Format("//NS:stateVector[%d]/NS:yVelocity",i);lpNodeValue=xmlReader.GetValue(lpNodeTag);stateVector.yVelocity=atof(lpNodeValue);                      //y平台速度
        lpNodeTag.Format("//NS:stateVector[%d]/NS:zVelocity",i);lpNodeValue=xmlReader.GetValue(lpNodeTag);stateVector.zVelocity=atof(lpNodeValue);                      //z平台速度
        header.StateVector.push_back(stateVector);
    }
    return header;
}

void CRd2sp2Local::Rd2spData2Rmg(CString lpDataImport,CString lpHdrImport,CString lpDataExport,CString lpHdrExport)
{
    
    GDALAllRegister();
    //修改字符集配置以适应中文路径
    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8","NO");   
    GDALDataset *rd2Dataset = (GDALDataset *) GDALOpen(lpDataImport, GA_ReadOnly);
    if(rd2Dataset==NULL)return ;    
    GDALDriver *gdalDriver=rd2Dataset->GetDriver();
    /************************************************************************/
    /* xsize 列数                                                                   
     * ysize 行数
     * lsize 行数
     * bsize 波段数
    /************************************************************************/
    int xsize,ysize,lsize,bsize;
    xsize=rd2Dataset->GetRasterXSize();
    ysize=rd2Dataset->GetRasterYSize();
    bsize=rd2Dataset->GetRasterCount();
    CFile rmgFile;
    if(!rmgFile.Open(lpDataExport,CFile::modeWrite | CFile::shareExclusive | CFile::modeNoTruncate | CFile::modeCreate))
    {
        MessageBox(NULL,"创建文件失败，请重新选择保存路径!","提示",MB_OK|MB_ICONINFORMATION);
        return;     
    }
    GDALRasterBand *dataBand=NULL;
    GDALDataType dataType=GDALDataType::GDT_Byte;
    CProgressBar pgrsBar(bsize*ysize,"正在进行数据转换...");    
    void *pfScanLine=NULL;
    int typeSize=0;

    //遍历每个波段数
    for(int b=0;b<bsize;b++)
    {
        dataBand=rd2Dataset->GetRasterBand(b+1);
        dataType=dataBand->GetRasterDataType();         
        MACRO_GENERIC_SIMPLE_INVOKE((ENUM_DATA_TYPE)dataType,GenericCPLMalloc,xsize,typeSize,&pfScanLine);
        
        for (int y=0;y<ysize;y++)
        {
            pgrsBar.StepIt();       
            dataBand->RasterIO(GF_Read,0,y,xsize,1,pfScanLine,xsize,1,dataType,0,0);
            rmgFile.Write(pfScanLine,typeSize*xsize);
        }//for y
        CPLFree(pfScanLine);
    }//for b    

    
    CRMGHeader header=Rd2spHdr2Ldr(lpHdrImport);
    header.Sensor=ENUM_SENSOR_TYPE::RD2;
    header.DataType=(ENUM_DATA_TYPE)dataType;
    header.Band=bsize;
    header.Sample=xsize;
    header.Line=ysize;
    header.SampleOri = xsize;
    header.LineOri = ysize;
    header.Wavelength = 299792458.0/header.RadarCenterFrequency;
    header.Registration.isRegistr =0;
    header.Registration.registrType = -1;
    header.Registration.azimuthOffst =0;
    header.Registration.rangeOffset =0;
    header.leftUpC =0;
    header.leftUpL =0;
    header.Save(lpHdrExport);
    
}
