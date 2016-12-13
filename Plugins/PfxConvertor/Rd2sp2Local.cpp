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
    lpNodeValue=xmlReader.GetValue("//NS:ellipsoidName");header.Ellipse.name=lpNodeValue;                                               //�ο���������
    lpNodeValue=xmlReader.GetValue("//NS:semiMajorAxis");header.Ellipse.major=atof(lpNodeValue);                                        //�ο����򳤰���
    lpNodeValue=xmlReader.GetValue("//NS:semiMinorAxis");header.Ellipse.minor=atof(lpNodeValue);                                        //�ο�����̰���
    lpNodeValue=xmlReader.GetValue("//NS:pulseRepetitionFrequency");header.PulseRepetitionFrequency=atof(lpNodeValue);                  //�ο�����̰���
    lpNodeValue=xmlReader.GetValue("//NS:sampledPixelSpacing");header.SampledPixelSpacing=atof(lpNodeValue);                            //�ο�����̰���
    lpNodeValue=xmlReader.GetValue("//NS:sampledLineSpacing");header.SampledLineSpacing=atof(lpNodeValue);                              //�ο�����̰���
    
    lpNodeValue=xmlReader.GetValue("//NS:numberOfRangeLooks");header.Range.numberOfLooks=atoi(lpNodeValue);                         //����������
    lpNodeValue=xmlReader.GetValue("//NS:rangeLookBandwidth");header.Range.looksBandWidth=atof(lpNodeValue);                            //��������������
    lpNodeValue=xmlReader.GetValue("//NS:totalProcessedRangeBandwidth");header.Range.totalProcessedBandWidth=atof(lpNodeValue);     //���������
    lpNodeValue=xmlReader.GetValue("//NS:rangeWindow/NS:windowName");header.Range.windowName=lpNodeValue;                                   //�����򴰿�����
    lpNodeValut=xmlReader.GetValue("//NS:rangeWindow/NS:windowCoefficient");header.Range.windowCoefficient=atof(lpNodeValue);               //�����򴰿�ϵ��


    lpNodeValue=xmlReader.GetValue("//NS:numberOfAzimuthLooks");header.Azimuth.numberOfLooks=atoi(lpNodeValue);                     //��λ������
    lpNodeValue=xmlReader.GetValue("//NS:azimuthLookBandwidth");header.Azimuth.looksBandWidth=atof(lpNodeValue);                        //��λ����������
    lpNodeValue=xmlReader.GetValue("//NS:totalProcessedAzimuthBandwidth");header.Azimuth.totalProcessedBandWidth=atof(lpNodeValue); //��λ�����
    lpNodeValue=xmlReader.GetValue("//NS:azimuthWindow/NS:windowName");header.Azimuth.windowName=lpNodeValue;                               //��λ�򴰿�����
    lpNodeValue=xmlReader.GetValue("//NS:azimuthWindow/NS:windowCoefficient");header.Azimuth.windowCoefficient=atof(lpNodeValue);           //��λ�򴰿�ϵ��
    
    lpNodeValue=xmlReader.GetValue("//NS:incidenceAngleNearRange");header.IncidenceAngleNearRange=atof(lpNodeValue);                    //���������
    lpNodeValue=xmlReader.GetValue("//NS:incidenceAngleFarRange");header.IncidenceAngleFarRange=atof(lpNodeValue);                      //Զ�������
    lpNodeValue=xmlReader.GetValue("//NS:satelliteHeight");header.SatelliteHeight=atof(lpNodeValue);                                    //���Ǹ߶�

    //lpNodeValue=xmlReader.GetValue("//NS:geodeticCoordinate[1]/NS:latitude");header.GeodeticCoordinateFirst.latitude=atof(lpNodeValue);   //���Ƶ�γ��
    //lpNodeValue=xmlReader.GetValue("//NS:geodeticCoordinate[1]/NS:longitude");header.GeodeticCoordinateFirst.longitude=atof(lpNodeValue); //���Ƶ㾭��
    //lpNodeValue=xmlReader.GetValue("//NS:geodeticCoordinate[1]/NS:height");header.GeodeticCoordinateFirst.height=atof(lpNodeValue);       //���Ƶ�߶�
    
    nodeCount=xmlReader.GetCount("//NS:geodeticCoordinate");
    lpNodeValue=xmlReader.GetValue("//NS:imageTiePoint[1]/NS:geodeticCoordinate/NS:latitude");header.GeodeticCoordinateFirst.latitude=atof(lpNodeValue);    //��һ�е�һ�п��Ƶ�γ��
    lpNodeValue=xmlReader.GetValue("//NS:imageTiePoint[1]/NS:geodeticCoordinate/NS:longitude");header.GeodeticCoordinateFirst.longitude=atof(lpNodeValue);  //��һ�е�һ�п��Ƶ㾭��
    lpNodeValue=xmlReader.GetValue("//NS:imageTiePoint[1]/NS:geodeticCoordinate/NS:height");header.GeodeticCoordinateFirst.height=atof(lpNodeValue);        //��һ�е�һ�п��Ƶ�߶�


    lpNodeTag.Format("//NS:imageTiePoint[%d]/NS:geodeticCoordinate/NS:latitude",(nodeCount-1)/2+8); 
    lpNodeValue=xmlReader.GetValue(lpNodeTag);header.GeodeticCoordinateCenter.latitude=atof(lpNodeValue);                           //�������Ŀ��Ƶ�γ��
    lpNodeTag.Format("//NS:imageTiePoint[%d]/NS:geodeticCoordinate/NS:longitude",(nodeCount-1)/2+8);
    lpNodeValue=xmlReader.GetValue(lpNodeTag);header.GeodeticCoordinateCenter.longitude=atof(lpNodeValue);                          //�������Ŀ��Ƶ�γ��
    lpNodeTag.Format("//NS:imageTiePoint[%d]/NS:geodeticCoordinate/NS:height",(nodeCount-1)/2+8);
    lpNodeValue=xmlReader.GetValue(lpNodeTag);header.GeodeticCoordinateCenter.height=atof(lpNodeValue);                         //�������Ŀ��Ƶ�γ��

    lpNodeValue=xmlReader.GetValue("//NS:imageTiePoint[last()]/NS:geodeticCoordinate/NS:latitude");header.GeodeticCoordinateLast.latitude=atof(lpNodeValue);    //���������п��Ƶ�γ��
    lpNodeValue=xmlReader.GetValue("//NS:imageTiePoint[last()]/NS:geodeticCoordinate/NS:longitude");header.GeodeticCoordinateLast.longitude=atof(lpNodeValue);  //���������п��Ƶ㾭��
    lpNodeValue=xmlReader.GetValue("//NS:imageTiePoint[last()]/NS:geodeticCoordinate/NS:height");header.GeodeticCoordinateLast.height=atof(lpNodeValue);        //���������п��Ƶ�߶�

    lpNodeValue=xmlReader.GetValue("//NS:rawDataStartTime");header.ImagingTime=CRMGHeader::StructTimePoint(lpNodeValue);                                                //����ʱ��
    lpNodeValue=xmlReader.GetValue("//NS:radarCenterFrequency");header.RadarCenterFrequency=atof(lpNodeValue);                                                  //�״�Ƶ��
    
    lpNodeValue=xmlReader.GetValue("//NS:passDirection");
    header.PassDirection=lpNodeValue=="Ascending"?ENUM_DIRECTION_TYPE::ASCEND:ENUM_DIRECTION_TYPE::DESCEND;                                                     //������

    lpNodeValue=xmlReader.GetValue("//NS:zeroDopplerTimeFirstLine");header.ZeroDopplerTimeFirstLine=CRMGHeader::StructTimePoint(lpNodeValue);                       //��һ����λ��Ԫ���������շ�λʱ��
    lpNodeValue=xmlReader.GetValue("//NS:zeroDopplerTimeLastLine");header.ZeroDopplerTimeLastLine=CRMGHeader::StructTimePoint(lpNodeValue);                         //���λ��Ԫ���������շ�λʱ��
    lpNodeValue=xmlReader.GetValue("//NS:dopplerRateReferenceTime");header.DopplerRateReferenceTime=atof(lpNodeValue);                                          //�����������վ���ʱ��

    lpNodeValue=xmlReader.GetValue("//NS:adcSamplingRate");header.AdcSamplingRate=atof(lpNodeValue);                                                            //�������
    
    lpNodeValue=xmlReader.GetValue("//NS:slantRangeTimeToFirstRangeSample");header.SlantRangeTimeToFirstRangeSample=atof(lpNodeValue);                                  //�������
    for(int i=1;i<=5;i++)
    {   
        CRMGHeader::StructStateVector stateVector;
        lpNodeTag.Format("//NS:stateVector[%d]/NS:timeStamp",i);lpNodeValue=xmlReader.GetValue(lpNodeTag);stateVector.timePoint=CRMGHeader::StructTimePoint(lpNodeValue);   //��¼ʱ��
        lpNodeTag.Format("//NS:stateVector[%d]/NS:xPosition",i);lpNodeValue=xmlReader.GetValue(lpNodeTag);stateVector.xPosition=atof(lpNodeValue);                      //xƽ̨λ��
        lpNodeTag.Format("//NS:stateVector[%d]/NS:yPosition",i);lpNodeValue=xmlReader.GetValue(lpNodeTag);stateVector.yPosition=atof(lpNodeValue);                      //yƽ̨λ��
        lpNodeTag.Format("//NS:stateVector[%d]/NS:zPosition",i);lpNodeValue=xmlReader.GetValue(lpNodeTag);stateVector.zPosition=atof(lpNodeValue);                      //zƽ̨λ��
        lpNodeTag.Format("//NS:stateVector[%d]/NS:xVelocity",i);lpNodeValue=xmlReader.GetValue(lpNodeTag);stateVector.xVelocity=atof(lpNodeValue);                      //xƽ̨�ٶ�
        lpNodeTag.Format("//NS:stateVector[%d]/NS:yVelocity",i);lpNodeValue=xmlReader.GetValue(lpNodeTag);stateVector.yVelocity=atof(lpNodeValue);                      //yƽ̨�ٶ�
        lpNodeTag.Format("//NS:stateVector[%d]/NS:zVelocity",i);lpNodeValue=xmlReader.GetValue(lpNodeTag);stateVector.zVelocity=atof(lpNodeValue);                      //zƽ̨�ٶ�
        header.StateVector.push_back(stateVector);
    }
    return header;
}

void CRd2sp2Local::Rd2spData2Rmg(CString lpDataImport,CString lpHdrImport,CString lpDataExport,CString lpHdrExport)
{
    
    GDALAllRegister();
    //�޸��ַ�����������Ӧ����·��
    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8","NO");   
    GDALDataset *rd2Dataset = (GDALDataset *) GDALOpen(lpDataImport, GA_ReadOnly);
    if(rd2Dataset==NULL)return ;    
    GDALDriver *gdalDriver=rd2Dataset->GetDriver();
    /************************************************************************/
    /* xsize ����                                                                   
     * ysize ����
     * lsize ����
     * bsize ������
    /************************************************************************/
    int xsize,ysize,lsize,bsize;
    xsize=rd2Dataset->GetRasterXSize();
    ysize=rd2Dataset->GetRasterYSize();
    bsize=rd2Dataset->GetRasterCount();
    CFile rmgFile;
    if(!rmgFile.Open(lpDataExport,CFile::modeWrite | CFile::shareExclusive | CFile::modeNoTruncate | CFile::modeCreate))
    {
        MessageBox(NULL,"�����ļ�ʧ�ܣ�������ѡ�񱣴�·��!","��ʾ",MB_OK|MB_ICONINFORMATION);
        return;     
    }
    GDALRasterBand *dataBand=NULL;
    GDALDataType dataType=GDALDataType::GDT_Byte;
    CProgressBar pgrsBar(bsize*ysize,"���ڽ�������ת��...");    
    void *pfScanLine=NULL;
    int typeSize=0;

    //����ÿ��������
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
