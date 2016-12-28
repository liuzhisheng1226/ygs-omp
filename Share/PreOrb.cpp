// #include "StdAfx.h"
#include "PreOrb.h"
#include <string.h>

CPreOrb::CPreOrb(void)
{
}

CPreOrb::CPreOrb(string oriPreOrbFile)
{

}

CPreOrb::~CPreOrb(void)
{
}


void CPreOrb::LoadOrifile(string preOrbFile,float fInternalT)
{
    double startT = StateVector[0].timePoint.precision;
    double endT = StateVector[StateVector.size()-1].timePoint.precision;
    
//#pragma region ¾«¹ìÊý¾Ý¶ÁÈ¡
    //ÆðÊ¼Ê±¼ä²ÎÊý
    
    FILE* fp=fopen(preOrbFile.c_str(),"r");
    int nCount=0;
    const int bufferSize=512;
    char buffer[bufferSize];
    //vector<CRMGHeader::StructStateVector> stateVector;
    //Ê±¼ä¸ñÊ½ÕýÔò±í´ïÊ½ 2012-268-10:03:10.297266
    boost::regex pattern("\\d{4}-\\d{3}-\\d{2}:\\d{2}:\\d{2}.+");
    
    //¾«¹ìÊý¾ÝÎÄ¼þ
    while(!feof(fp))                    //&&nCount<CONST_ORBIT_SAPMLING_POINT_COUNT)
    {
        //»ñÈ¡Ò»ÐÐÊý¾Ý
        fgets(buffer,bufferSize,fp);
        //½«»»ÐÐ·û±êÊ¶Î´½áÎ²        
        buffer[strlen(buffer)-1]='\0';
        string text(buffer);                        
        //ÕýÔò±í´ïÊ½Æ¥Åä ÈÕÆÚ¸ñÊ½
        if(boost::regex_match(text,pattern))
        {
            CRMGHeader::StructTimePoint timePoint(text.data());
            CRMGHeader::StructStateVector nVector;
            if(startT-8<timePoint.precision && timePoint.precision <endT+8)
            {
                nVector.timePoint=timePoint;        //.precision;
                fgets(buffer,bufferSize,fp);
                string str(buffer);
                nVector.xPosition=atof(str.substr(0,12).data());
                nVector.yPosition=atof(str.substr(13,12).data());
                nVector.zPosition=atof(str.substr(25,12).data());
                fgets(buffer,bufferSize,fp);
                str=buffer;
                nVector.xVelocity=atof(str.substr(0,12).data());
                nVector.yVelocity=atof(str.substr(13,12).data());
                nVector.zVelocity=atof(str.substr(25,12).data());
                //stateVector.push_back(nVector);
                preOrbitPoint.push_back(nVector);
                nCount++;
            }//if
        }//if boost::regex
    }//while 

    m_startT = preOrbitPoint[0].timePoint.precision;
    m_fIntervalT = fInternalT;
    m_nCount = (int)(preOrbitPoint[preOrbitPoint.size()-1].timePoint.precision-preOrbitPoint[0].timePoint.precision)/m_fIntervalT+1;
    


//#pragma endregion ´ÓÎÄ¼þÖÐ¶ÁÈ¡¾«¹ìÊý¾Ý

//#pragma region Êý¾ÝÈÚºÏ
//  for(vector<CRMGHeader::StructStateVector>::iterator itr=stateVector.begin();itr!=stateVector.end();itr++)
//  {
//      if(itr->timePoint<hdrStaVector[0].timePoint)                
//          hdrStaVector.insert(hdrStaVector.begin(),*itr);
//      else if (itr->timePoint>hdrStaVector[hdrStaVector.size()-1].timePoint)
//          hdrStaVector.push_back(*itr);
//      else 
//          for(int i=0;i<hdrStaVector.size();i++)          
//              if(itr->timePoint>hdrStaVector[i].timePoint && itr->timePoint<hdrStaVector[i+1].timePoint) 
//                  hdrStaVector.insert(hdrStaVector.begin()+i+1,*itr);
//  }//for ±éÀú¾«¹ìÊý¾Ý
//#pragma endregion ½«¾«¹ìÊý¾ÝºÍÍ·ÎÄ¼þÊý¾ÝÈÚºÏ
}

void CPreOrb::LoadOrbfile()
{
    char buffer[200];
    FILE *fp;
    if((fp = fopen(m_fileName.c_str(),"rt"))==NULL)
        return;
    while(!feof(fp))
    {
        fscanf(fp,"%s",buffer);
        if (feof(fp)) break;
        if(!strcmp(buffer,"[起始时间]"))
        {
            fscanf(fp, "%s",buffer);
            m_startT=atof(buffer);
        }
        if(!strcmp(buffer,"[时间间隔]"))
        {
            fscanf(fp, "%s",buffer);
            m_fIntervalT=atof(buffer);
        }
        if(!strcmp(buffer,"[点数]"))
        {
            fscanf(fp, "%s",buffer);
            m_nCount=atoi(buffer);
        }
        
        if(!strcmp(buffer,"[卫星位置]"))
        {
            CRMGHeader::StructStateVector nVector;
            for(int i=0;i<m_nCount;i++)
            {
                fscanf(fp,"%s",buffer);
                nVector.xPosition = atof(buffer);
                fscanf(fp,"%s",buffer);
                nVector.yPosition = atof(buffer);
                fscanf(fp,"%s",buffer);
                nVector.zPosition = atof(buffer);
                preOrbitPoint.push_back(nVector);
            }
        }
    }
    fclose(fp);
    preOrbitPoint[0].timePoint.precision = m_startT;
    return;
}
