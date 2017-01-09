#include "Delaunay.h"
#include "DenuanayNetwork.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include <omp.h>

#define const_Pi 3.1415926

CDenuanayNetwork::CDenuanayNetwork(void)
{
}

void CDenuanayNetwork::Batch()
{
}

void CDenuanayNetwork::Process()
{
	/*CDlgDenuanayNetwork dlg;*/
	//if(dlg.DoModal()!=IDOK) return;

	//CString strPscFile=dlg.m_pscFileIn;
	//CString strPscFileH=dlg.m_pscFileInH;
	//CString strEdgeFileOut=dlg.m_edgeFileOut;
	//CString strEdgeFileOutH=dlg.m_edgeFileOutH;

    /*CTInSAR_incre(strPscFile,strPscFileH,strEdgeFileOut,strEdgeFileOutH);*/
}

void CDenuanayNetwork::CTInSAR_incre(string strPscFile,string strPscFileH,string strEdgeFileOut,string strEdgeFileOutH)
{
    //¶ÁÈ¡Ïà¹Ø²ÎÊý
    //ÎÄ¼þÖ¸Õë
    FILE *fp;
    char buf[500];
    int numdiff,height,width,roi_x,roi_y,roi_h,roi_w,ctNum;
    float wl;
    fp=fopen(strPscFileH.c_str(),"r");
    while(!feof(fp))
    {
        fscanf(fp,"%s",buf);
        if (feof(fp)) break;
        if(!strcmp(buf,"[width]"))
        {
            fscanf(fp,"%s",buf);
            width=atoi(buf);
        }
        if(!strcmp(buf,"[height]"))
        {
            fscanf(fp,"%s",buf);
            height=atoi(buf);
        }
        if(!strcmp(buf,"[roi_y]"))
        {
            fscanf(fp,"%s",buf);
            roi_y=atoi(buf);
        }
        if(!strcmp(buf,"[roi_x]"))
        {
            fscanf(fp,"%s",buf);
            roi_x=atoi(buf);
        }
        if(!strcmp(buf,"[roi_w]"))
        {
            fscanf(fp,"%s",buf);
            roi_w=atoi(buf);
        }
        if(!strcmp(buf,"[roi_h]"))
        {
            fscanf(fp,"%s",buf);
            roi_h=atoi(buf);
        }
        if(!strcmp(buf,"[numPoint]"))
        {
            fscanf(fp,"%s",buf);
            ctNum=atoi(buf);
        }
        if(!strcmp(buf,"[numdiff]"))
        {
            fscanf(fp,"%s",buf);
            numdiff=atoi(buf);
        }
        if(!strcmp(buf,"[wavelength]"))
        {
            fscanf(fp,"%s",buf);
            wl=atof(buf);
        }
    }
    fclose(fp);
    //¾Ö²¿±äÁ¿
    //int i,j;

    ifstream pscFile (strPscFile, ios::in);
    if (!pscFile.is_open()) {
        cout << "error open file\n";
        return;
    }

    //Èý½ÇÍø
    Delaunay d;
    vertexSet m_Vertices;    //Èý½ÇÍø¶¥µã¼¯ºÏ
    triangleSet m_Triangles; //Èý½ÇÐÎ¼¯ºÏ
    pscSet PSpoints;  //CTµã¼¯ºÏ
    edgeSet edges;    //±ß¼¯ºÏ
    PSC  tempPSC;

    int flag=0;
    int nPointCount=0;
    while ((flag = pscFile.read((char *)(&tempPSC), sizeof(PSC)).gcount()) > 1)
    {
        PSpoints.insert(tempPSC);
        m_Vertices.insert(vertex(tempPSC.m_pos.X,tempPSC.m_pos.Y));
        nPointCount++;
    }
    pscFile.close();

    //1¡¢½¨Á¢Èý½ÇÍø
    d.Triangulate(m_Vertices, m_Triangles);
    d.TrianglesToEdges(m_Triangles,edges);

    int Count_edge=d.Count_edge(edges);
    if (!Count_edge) {
        printf("no edge!\n");
        exit(-1);
    }

    //2¡¢¼ÆËãÃ¿Ìõ±ßµÄÔöÁ¿
    ofstream file_edge (strEdgeFileOut, ios::out);
    if (!file_edge.is_open()) {
        cout << "error open file\n";
        return;
    }

    //---------------½ø¶ÈÌõ----------
    //---------------½ø¶ÈÌõ--------------

    vector<edge> vedges;
    for(edgeIterator ei=edges.begin();ei!=edges.end();ei++) {
        vedges.push_back(*ei);
    }

    omp_lock_t elock;
    omp_init_lock(&elock);
#pragma omp parallel for 
    for (int i = 0; i < Count_edge; ++i)
    {
        edge *ei = &(vedges[i]);

        float *diffpha= new float[numdiff];
        float *upbn= new float[numdiff];
        float *upinc= new float[numdiff];
        float *upslt= new float[numdiff];
        float *time_c= new float[numdiff];
        memset(diffpha,0,sizeof(float)*numdiff);
        memset(upbn,0,sizeof(float)*numdiff);
        memset(upinc,0,sizeof(float)*numdiff);
        memset(upslt,0,sizeof(float)*numdiff);
        memset(time_c,0,sizeof(float)*numdiff);

        float **result,*temp;
        temp=new float[1];
        result=new float*[1];
        result[0]=new float[3];
        edge_Re edgeTemp;

        //±ßÉÏµÄÁ½¸ö¶¥µã
        PSC ps0,ps1;
        ps0.m_pos=ei->m_Pv0->GetPoint();
        ps1.m_pos=ei->m_Pv1->GetPoint();
        //PSC¼¯ºÏ±éÀúµü´úÆ÷
        pscIterator ipscT=PSpoints.find(ps0);
        for(int i=0;i<numdiff;i++)
        {
            ps0.data[i]=ipscT->data[i];
            ps0.bv[i]=ipscT->bv[i];
            ps0.iAngle[i]=ipscT->iAngle[i];
            ps0.sRange[i]=ipscT->sRange[i];
            ps0.deltaD[i]=ipscT->deltaD[i];
        }
        ps0.index=ipscT->index;

        ipscT=PSpoints.find(ps1);
        for(int i=0;i<numdiff;i++)
        {
            ps1.data[i]=ipscT->data[i];
            ps1.bv[i]=ipscT->bv[i];
            ps1.iAngle[i]=ipscT->iAngle[i];
            ps1.sRange[i]=ipscT->sRange[i];
            ps1.deltaD[i]=ipscT->deltaD[i];
        }
        ps1.index=ipscT->index;

        for(int i=0;i<numdiff;i++)
        {
            diffpha[i]=ps1.data[i]-ps0.data[i];
            upbn[i]=ps0.bv[i];
            upinc[i]=ps0.iAngle[i];
            upslt[i]=ps0.sRange[i];
            time_c[i]=float(ps0.deltaD[i])/356000;   //Ê±¼ä»ùÏß  °Ñµ¥Î»m/d  ×ª»¯³Émm/yr
        }

        float optins[2];
        optins[0]=0;    optins[1]=0;
        //×îÓÅ»¯¹ý³Ì
        CTInSAR_Findopt(optins,numdiff,time_c,diffpha,upbn,upinc,upslt,wl,result);

        //Èç¹ûÄ£ÐÍÓëÊµ¼Ê½Ç¶È²î³¬¹ýÁË90¶È£¬ÄÇÃ´ÈÏÎªÊÇÎó²î£¬Òª¼ÓÒÔÅÅ³ý
        float *phasTemp= new float[numdiff];   //Ä£ÐÍÏàÎ»±äÁ¿
        for(int k=0;k<numdiff;k++)
        {
            phasTemp[k]=4*const_Pi/wl*time_c[k]*result[0][0]-4*const_Pi/wl*(upbn[k]*result[0][1])/(sin(upinc[k])*upslt[k]);
            phasTemp[k]=phasTemp[k]-diffpha[k];   //Ä£ÐÍÏàÎ»ÓëÕæÊµÏàÎ»Ö®²î

            phasTemp[k]=phasTemp[k]-(phasTemp[k]/(2*const_Pi))*2*const_Pi;

            if(phasTemp[k]>const_Pi)
            {
                phasTemp[k]=phasTemp[k]-2*const_Pi;
            }
            if((phasTemp[k]<-const_Pi/2)||(phasTemp[k]>const_Pi/2))
            {
                upbn[k]=0;
            }
        }
        delete []phasTemp;
        //4¡¢½øÐÐµÚ¶þ´Î×îÓÅ»¯ËÑË÷
        optins[0]=result[0][0];optins[1]=result[0][1];
        CTInSAR_Findopt(optins,numdiff,time_c,diffpha,upbn,upinc,upslt,wl,result);

        edgeTemp.dV=result[0][0];
        edgeTemp.dH=result[0][1];
        edgeTemp.edgeCoh=-result[0][2];  //±ßÉÏµÄÊ±¼äÏà¸ÉÐÔ
        edgeTemp.m_pos0=ps0.m_pos;
        edgeTemp.m_pos1=ps1.m_pos;
        edgeTemp.index0=ps0.index;
        edgeTemp.index1=ps1.index;
        //edgeTemp.m_Pv0=ei->m_Pv0;
        //edgeTemp.m_Pv1=ei->m_Pv0;
        
        //Êä³ö±ß
        omp_set_lock(&elock);
        file_edge.seekp((streampos)i*sizeof(edge_Re), ios::beg);
        file_edge.write((char *)(&edgeTemp), sizeof(edge_Re));
        omp_unset_lock(&elock);

        //Çå³þÄÚ´æ
        delete []time_c;
        delete []diffpha;
        delete []upbn;
        delete []upinc;
        delete []upslt;

        delete []temp;
        delete []result[0];
        delete []result;
    }
    omp_destroy_lock(&elock);
    //¹Ø±ÕÎÄ¼þ£»
    file_edge.close();

    //Í·ÎÄ¼þ
    fp=fopen(strEdgeFileOutH.c_str(),"w");
    if(fp==NULL) return;

    fprintf(fp,"[width]");
    fprintf(fp,"\n\t\t%d",width);
    fprintf(fp,"\n[height]");
    fprintf(fp,"\n\t\t%d",height);
    fprintf(fp,"\n[roi_y]");
    fprintf(fp,"\n\t\t%d",roi_y);
    fprintf(fp,"\n[roi_x]");
    fprintf(fp,"\n\t\t%d",roi_x);
    fprintf(fp,"\n[roi_w]");
    fprintf(fp,"\n\t\t%d",roi_w);
    fprintf(fp,"\n[roi_h]");
    fprintf(fp,"\n\t\t%d",roi_h);
    fprintf(fp,"\n[count_edge]");
    fprintf(fp,"\n\t\t%d",Count_edge);
    fprintf(fp,"\n[numdiff]");
    fprintf(fp,"\n\t\t%d",numdiff);
    fclose(fp);
}

void CDenuanayNetwork::CTInSAR_incre(string filecoh,string filetime,vector<string> filepha,
                                     vector<string> filebn,vector<string> fileinc, vector<string> fileslt,
                                     int height,int width,int numsar,int numdiff)
{
    int dot = filetime.find_last_of('\\');
    string fileout_edge = filetime.substr(0, dot)+"\\output\\file1_edge.mat";
    string fileout_dvel = filetime.substr(0, dot)+"\\output\\file1_dvel.mat";
    string fileout_dhei = filetime.substr(0, dot)+"\\output\\file1_dhei.mat";
    string fileout_point= filetime.substr(0, dot)+"\\output\\file1_point.mat";
    string fileout_x    = filetime.substr(0, dot)+"\\output\\file1_x.mat";
    string fileout_y    = filetime.substr(0, dot)+"\\output\\file1_y.mat";

    //È·¶¨²¨³¤
    double wl=0.236;

    //¾Ö²¿±äÁ¿
    int i,j;

    //Èý½ÇÍø
    Delaunay d;
    vertexSet m_Vertices;    //Èý½ÇÍø¶¥µã¼¯ºÏ
    triangleSet m_Triangles; //Èý½ÇÐÎ¼¯ºÏ
    edgeSet edges;    //±ß¼¯ºÏ
    pscSet PSpoints;  //CTµã¼¯ºÏ

    //µÚÒ»²½ ¶ÁÈ¡Ïà¸Éµã
    ifstream file1 (filecoh, ios::in);
    if (!file1.is_open()) {
        cout << "error open file\n";
        return;
    }


    char *coh=new char[width];
    int numPoint=0;
    //¶ÁÈ¡Ïà¸ÉµãÎÄ¼þ
    PSC pscPoint;
    for( i=0;i<height;i++)
    {

        file1.read((char *)coh, sizeof(char)*width);

        for( j=0;j<width;j++)
        {
            if(coh[j]==1)
            {
                m_Vertices.insert(vertex(j+1,i+1));        //  X-->Width  Y-->height
                PSpoints.insert(PSC(j+1,i+1,numPoint)); //µãµÄindex´Ó0¿ªÊ¼
                numPoint++;
            }
        }

    }
    //1¡¢½¨Á¢Èý½ÇÍø
    d.Triangulate(m_Vertices, m_Triangles);
    d.TrianglesToEdges(m_Triangles,edges);

    int Count_edge=d.Count_edge(edges);

    //ÒÀ´Î¶Á²î·Ö¸ÉÉæÍ¼
    for(int nFileIndex=0;nFileIndex<numdiff;nFileIndex++ )
    {
        //2¡¢¶ÁÈ¡PSµãÏàÎ»
        CTInSAR_ReadPha(filepha,nFileIndex,height,width,PSpoints);

        //3¡¢¶ÁÈ¡PSµã´¹Ö±»ùÏß£¬ÈëÉä½Ç£¬Ð±¾àµÈ²ÎÊý
        CTInSAR_ReadBIS(filebn,fileinc,fileslt,nFileIndex,height,width,PSpoints);

    }

    

    //  4¡¢¶ÁÈ¡Ê±¼ä»ùÏß
    float *time_c=new float[numdiff];
    int temp_num=0;
    FILE *fp;
    char buf[50];
    fp=fopen(filetime.c_str(),"r");
    while(!feof(fp))
    {
        fscanf(fp,"%s",buf);
        if (feof(fp)) break;
        time_c[temp_num]=atof(buf)/365000;      //°ÑÊ±¼ä»»³ÉÄê£¬ÐÎ±äÁ¿»»³ÉºÁÃ×
        temp_num++;
    }
    fclose(fp); 


    float *diffpha= new float[numdiff];
    float *upbn= new float[numdiff];
    float *upinc= new float[numdiff];
    float *upslt= new float[numdiff];

    //5¡¢¼ÆËãÃ¿Ìõ±ßÉÏµÄÔöÁ¿

    ofstream file_edge (fileout_edge, ios::out);
    if (!file_edge.is_open()) {
        cout << "error open file\n";
        return;
    }
    ofstream file_point (fileout_point, ios::out);
    if (!file_point.is_open()) {
        cout << "error open file\n";
        return;
    }
    ofstream file_vel (fileout_dvel, ios::out);
    if (!file_vel.is_open()) {
        cout << "error open file\n";
        return;
    }
    ofstream file_hei(fileout_dhei, ios::out);
    if (!file_hei.is_open()) {
        cout << "error open file\n";
        return;
    }

    float **result,*temp;
    temp=new float[1];
    result=new float*[1];
    result[0]=new float[3];
    edge_Re edgeTemp;


    PSC ps0,ps1;
    pscIterator ipscT;      //PSC¼¯ºÏ±éÀúµü´úÆ÷

    //---------------½ø¶ÈÌõ----------
    //---------------½ø¶ÈÌõ--------------


    for(edgeIterator ei=edges.begin();ei!=edges.end();ei++)
    {
        //±ßÉÏµÄÁ½¸ö¶¥µã
        ps0.m_pos=ei->m_Pv0->GetPoint();
        ps1.m_pos=ei->m_Pv1->GetPoint();
        ipscT=PSpoints.find(ps0);

        for(i=0;i<numdiff;i++)
        {
            ps0.data[i]=ipscT->data[i];
            ps0.bv[i]=ipscT->bv[i];
            ps0.iAngle[i]=ipscT->iAngle[i];
            ps0.sRange[i]=ipscT->sRange[i];
        }
        ps0.index=ipscT->index;

        ipscT=PSpoints.find(ps1);
        for(i=0;i<numdiff;i++)
        {
            ps1.data[i]=ipscT->data[i];
            ps1.bv[i]=ipscT->bv[i];
            ps1.iAngle[i]=ipscT->iAngle[i];
            ps1.sRange[i]=ipscT->sRange[i];
        }
        ps1.index=ipscT->index;

        for(int i=0;i<numdiff;i++)
        {
            diffpha[i]=ps1.data[i]-ps0.data[i];
            upbn[i]=ps0.bv[i];
            upinc[i]=ps0.iAngle[i];
            upslt[i]=ps0.sRange[i];
        }

        float optins[2];
        optins[0]=0;    optins[1]=0;
        //×îÓÅ»¯¹ý³Ì
        CTInSAR_Findopt(optins,numdiff,time_c,diffpha,upbn,upinc,upslt,wl,result);

        //Èç¹ûÄ£ÐÍÓëÊµ¼Ê½Ç¶È²î³¬¹ýÁË90¶È£¬ÄÇÃ´ÈÏÎªÊÇÎó²î£¬Òª¼ÓÒÔÅÅ³ý
        float *phasTemp= new float[numdiff];   //Ä£ÐÍÏàÎ»±äÁ¿
        for(int k=0;k<numdiff;k++)
        {
            phasTemp[k]=4*const_Pi/wl*time_c[k]*result[0][0]-4*const_Pi/wl*(upbn[k]*result[0][1])/(sin(upinc[k])*upslt[k]);
            phasTemp[k]=phasTemp[k]-diffpha[k];   //Ä£ÐÍÏàÎ»ÓëÕæÊµÏàÎ»Ö®²î

            phasTemp[k]=phasTemp[k]-(phasTemp[k]/(2*const_Pi))*2*const_Pi;

            if(phasTemp[k]>const_Pi)
            {
                phasTemp[k]=phasTemp[k]-2*const_Pi;
            }
            if((phasTemp[k]<-const_Pi/2)||(phasTemp[k]>const_Pi/2))
            {
                upbn[k]=0;
            }
        }
        delete []phasTemp;
        //4¡¢½øÐÐµÚ¶þ´Î×îÓÅ»¯ËÑË÷
        optins[0]=result[0][0];optins[1]=result[0][1];
        CTInSAR_Findopt(optins,numdiff,time_c,diffpha,upbn,upinc,upslt,wl,result);


        edgeTemp.dV=result[0][0];
        edgeTemp.dH=result[0][1];
        edgeTemp.edgeCoh=-result[0][2];  //±ßÉÏµÄÊ±¼äÏà¸ÉÐÔ
        edgeTemp.m_pos0=ps0.m_pos;
        edgeTemp.m_pos1=ps1.m_pos;
        edgeTemp.index0=ps0.index;
        edgeTemp.index1=ps1.index;
        //edgeTemp.m_Pv0=ei->m_Pv0;
        //edgeTemp.m_Pv1=ei->m_Pv0;

        
        //Êä³ö±ß
        file_edge.write((char *)(&edgeTemp), sizeof(edge_Re));

        temp[0]=result[0][0];
        file_vel.write((char *)temp, sizeof(float));

        temp[0]=result[0][1];
        file_hei.write((char *)temp, sizeof(float));

    }
    //¹Ø±ÕÎÄ¼þ£»
    file_edge.close();
    file_vel.close();
    file_hei.close();

    //Êä³öµãÄ¿±ê
    //PSC  *pstemp;
    pointResult *points;
    points =new pointResult();
    for(pscIterator Pi=PSpoints.begin();Pi!=PSpoints.end();Pi++)
    {
        points->X=Pi->m_pos.X;
        points->Y=Pi->m_pos.Y;
        points->index=Pi->index;
        file_point.write((char *)points, sizeof(pointResult));
    }
    file_point.close();




    delete []time_c;
    delete []coh;
    delete []diffpha;
    delete []upbn;
    delete []upinc;
    delete []upslt;
    delete []temp;

    delete points;
    delete []result[0];
    delete []result;
}


void CDenuanayNetwork::CTInSAR_incre(string filecoh,string filetime,vector<string> filepha,
                                     vector<string> filebn,vector<string> fileinc, vector<string> fileslt,
                                     int height,int width,int numsar,int numdiff,int roi_x0,int roi_y0,int roi_h0,int roi_w0)
{
    int dot = filetime.find_last_of('\\');
    string fileout_edge = filetime.substr(0, dot)+"\\roi_file1_edge.mat";
    string fileout_dvel = filetime.substr(0, dot)+"\\roi_file1_dvel.mat";
    string fileout_dhei = filetime.substr(0, dot)+"\\roi_file1_dhei.mat";
    string fileout_point= filetime.substr(0, dot)+"\\roi_file1_point.mat";
    string fileout_x    = filetime.substr(0, dot)+"\\roi_file1_x.mat";
    string fileout_y    = filetime.substr(0, dot)+"\\roi_file1_y.mat";

    //È·¶¨²¨³¤
    double wl= 0.056;               //0.236;
    //¾Ö²¿±äÁ¿
    int i,j;

    //Èý½ÇÍø
    Delaunay d;
    vertexSet m_Vertices;    //Èý½ÇÍø¶¥µã¼¯ºÏ
    triangleSet m_Triangles; //Èý½ÇÐÎ¼¯ºÏ
    edgeSet edges;    //±ß¼¯ºÏ
    pscSet PSpoints;  //CTµã¼¯ºÏ

    //µÚÒ»²½ ¶ÁÈ¡Ïà¸Éµã
    ifstream file1 (filecoh, ios::in);
    if (!file1.is_open()) {
        cout << "error open file\n";
        return;
    }

    int offset;
    offset=sizeof(char)*((roi_y0-1)*width+roi_x0-1);
    file1.seekg(offset, ios::beg);

    char *coh=new char[roi_w0];
    int numPoint=0;
    //¶ÁÈ¡Ïà¸ÉµãÎÄ¼þ
    for(int i=0;i<roi_h0;i++)
    {

        file1.read((char *)coh, sizeof(char)*roi_w0);

        int numEdge=0;
        for(int j=0;j<roi_w0;j++)
        {
            if(coh[j]==1)
            {
                m_Vertices.insert(vertex(j+1,i+1));  //  X-->Width  Y-->height
                PSpoints.insert(PSC(j+1,i+1,numPoint));  //µãµÄindex´Ó0¿ªÊ¼
                numPoint++;
            }
        }

        file1.seekg(sizeof(char)*(width-roi_w0), ios::cur);
    }
    file1.close();

    //1¡¢½¨Á¢Èý½ÇÍø
    d.Triangulate(m_Vertices, m_Triangles);
    d.TrianglesToEdges(m_Triangles,edges);

    //±ßµÄ¸öÊý
    int Count_edge=d.Count_edge(edges);

    //ÒÀ´Î¶Á²î·Ö¸ÉÉæÍ¼¡¢²ÎÊý
    for(int nFileIndex=0;nFileIndex<numdiff;nFileIndex++ )
    {
        //2¡¢¶ÁÈ¡PSµãÏàÎ»
        CTInSAR_ReadPha(filepha,nFileIndex,height,width,PSpoints,roi_x0,roi_y0,roi_h0,roi_w0);      //PSpoints= 

        //3¡¢¶ÁÈ¡PSµã´¹Ö±»ùÏß£¬ÈëÉä½Ç£¬Ð±¾àµÈ²ÎÊý
        CTInSAR_ReadBIS(filebn,fileinc,fileslt,nFileIndex,height,width,PSpoints,roi_x0,roi_y0,roi_h0,roi_w0);   //PSpoints=
    }

    //  4¡¢¶ÁÈ¡Ê±¼ä»ùÏß
    float *time_c=new float[numdiff];
    int temp_num=0;
    FILE *fp;
    char buf[50];
    fp=fopen(filetime.c_str(),"r");
    while(!feof(fp))
    {
        fscanf(fp,"%s",buf);
        if (feof(fp)) break;
        time_c[temp_num]=atof(buf)/356000;
        temp_num++;
    }
    fclose(fp); 


    float *diffpha= new float[numdiff];
    float *upbn= new float[numdiff];
    float *upinc= new float[numdiff];
    float *upslt= new float[numdiff];

    //5¡¢¼ÆËãÃ¿Ìõ±ßÉÏµÄÔöÁ¿
    ofstream file_edge (fileout_edge, ios::out);
    if (!file_edge.is_open()) {
        cout << "error open file\n";
        return;
    }
    ofstream file_point (fileout_point, ios::out);
    if (!file_point.is_open()) {
        cout << "error open file\n";
        return;
    }
    ofstream file_vel (fileout_dvel, ios::out);
    if (!file_vel.is_open()) {
        cout << "error open file\n";
        return;
    }
    ofstream file_hei(fileout_dhei, ios::out);
    if (!file_hei.is_open()) {
        cout << "error open file\n";
        return;
    }


    float **result,*temp;
    temp=new float[1];
    result=new float*[1];
    result[0]=new float[3];
    edge_Re edgeTemp;

    PSC ps0,ps1;
    pscIterator ipscT;      //PSC¼¯ºÏ±éÀúµü´úÆ÷

    //---------------½ø¶ÈÌõ----------
    //---------------½ø¶ÈÌõ--------------

    for(edgeIterator ei=edges.begin();ei!=edges.end();ei++)
    {
        //±ßÉÏµÄÁ½¸ö¶¥µã
        ps0.m_pos=ei->m_Pv0->GetPoint();
        ps1.m_pos=ei->m_Pv1->GetPoint();
        ipscT=PSpoints.find(ps0);

        for(i=0;i<numdiff;i++)
        {
            ps0.data[i]=ipscT->data[i];
            ps0.bv[i]=ipscT->bv[i];
            ps0.iAngle[i]=ipscT->iAngle[i];
            ps0.sRange[i]=ipscT->sRange[i];
        }
        ps0.index=ipscT->index;

        ipscT=PSpoints.find(ps1);
        for(i=0;i<numdiff;i++)
        {
            ps1.data[i]=ipscT->data[i];
            ps1.bv[i]=ipscT->bv[i];
            ps1.iAngle[i]=ipscT->iAngle[i];
            ps1.sRange[i]=ipscT->sRange[i];
        }
        ps1.index=ipscT->index;

        for(int i=0;i<numdiff;i++)
        {
            diffpha[i]=ps1.data[i]-ps0.data[i];
            upbn[i]=ps0.bv[i];
            upinc[i]=ps0.iAngle[i];
            upslt[i]=ps0.sRange[i];
        }

        float optins[2];
        optins[0]=0;    optins[1]=0;
        //×îÓÅ»¯¹ý³Ì
        CTInSAR_Findopt(optins,numdiff,time_c,diffpha,upbn,upinc,upslt,wl,result);

        //Èç¹ûÄ£ÐÍÓëÊµ¼Ê½Ç¶È²î³¬¹ýÁË90¶È£¬ÄÇÃ´ÈÏÎªÊÇÎó²î£¬Òª¼ÓÒÔÅÅ³ý
        float *phasTemp= new float[numdiff];   //Ä£ÐÍÏàÎ»±äÁ¿
        for(int k=0;k<numdiff;k++)
        {
            phasTemp[k]=4*const_Pi/wl*time_c[k]*result[0][0]-4*const_Pi/wl*(upbn[k]*result[0][1])/(sin(upinc[k])*upslt[k]);
            phasTemp[k]=phasTemp[k]-diffpha[k];   //Ä£ÐÍÏàÎ»ÓëÕæÊµÏàÎ»Ö®²î

            phasTemp[k]=phasTemp[k]-(phasTemp[k]/(2*const_Pi))*2*const_Pi;

            if(phasTemp[k]>const_Pi)
            {
                phasTemp[k]=phasTemp[k]-2*const_Pi;
            }
            if((phasTemp[k]<-const_Pi/2)||(phasTemp[k]>const_Pi/2))
            {
                upbn[k]=0;
            }
        }
        delete []phasTemp;

        //4¡¢½øÐÐµÚ¶þ´Î×îÓÅ»¯ËÑË÷
        optins[0]=result[0][0];optins[1]=result[0][1];
        CTInSAR_Findopt(optins,numdiff,time_c,diffpha,upbn,upinc,upslt,wl,result);

        edgeTemp.dV=result[0][0];
        edgeTemp.dH=result[0][1];
        edgeTemp.edgeCoh=-result[0][2];  //±ßÉÏµÄÊ±¼äÏà¸ÉÐÔ
        edgeTemp.m_pos0=ps0.m_pos;
        edgeTemp.m_pos1=ps1.m_pos;
        edgeTemp.index0=ps0.index;
        edgeTemp.index1=ps1.index;
        //edgeTemp.m_Pv0=ei->m_Pv0;
        //edgeTemp.m_Pv1=ei->m_Pv1;

        file_edge.write((char *)(&edgeTemp), sizeof(edge_Re));
        
        temp[0]=result[0][0];
        file_vel.write((char *)temp, sizeof(float));

        temp[0]=result[0][1];
        file_hei.write((char *)temp, sizeof(float));

    }
    //¹Ø±ÕÎÄ¼þ£»
    file_edge.close();
    file_vel.close();
    file_hei.close();

    //PSC  *pstemp;
    pointResult *points;
    points =new pointResult();
    for(pscIterator Pi=PSpoints.begin();Pi!=PSpoints.end();Pi++)
    {

        points->X=Pi->m_pos.X;
        points->Y=Pi->m_pos.Y;
        points->index=Pi->index;
        file_point.write((char *)points, sizeof(pointResult));
    }
    file_point.close();



    //ÊÍ·ÅÄÚ´æ
    delete []coh;
    delete []time_c;
    delete []diffpha;
    delete []upbn;
    delete []upinc;
    delete []upslt;

    delete []temp;
    delete points;
    delete []result[0];
    delete []result;
}

//²î·ÖÏàÎ»¶ÁÈ¡
void CDenuanayNetwork::CTInSAR_ReadPha(vector<string> filepha,int nFileIndex,int height,int width, pscSet& PSpoints)
{
    pscSet PssetTemp=PSpoints;

    string temp=filepha[nFileIndex];
    ifstream file1 (temp, ios::in);
    if (!file1.is_open()) {
        cout << "error open file\n";
        return;
    }

    float **pha=new float*[height];
    for(int i=0;i<height;i++)
    {
        pha[i]=new float[width];
    }
    for(int i=0;i<height;i++)
    {
        file1.read((char *)pha[i], sizeof(float)*width);
    }

    PSC  pstemp;  
    for(pscIterator Pi=PSpoints.begin();Pi!=PSpoints.end();Pi++)
    {
        pstemp =(*Pi);
        pstemp.data[nFileIndex]=pha[(int)pstemp.m_pos.Y-1][(int)pstemp.m_pos.X-1];   //ÐÐÁÐ
        pscIterator found =PssetTemp.find(*Pi);
        PssetTemp.erase(found);
        PssetTemp.insert(pstemp);
        
    }

    PSpoints=PssetTemp;
    PssetTemp.clear();
    for(int i=0;i<height;i++)
    {
        delete[]pha[i];

    }
    delete pha;

    return ;
}

void CDenuanayNetwork::CTInSAR_ReadPha(vector<string> filepha,int nFileIndex,int height,int width, pscSet& PSpoints,
                                       int roi_x0,int roi_y0,int roi_h0,int roi_w0)
{
    pscSet PssetTemp=PSpoints;


    string temp=filepha[nFileIndex];

    ifstream file1 (temp, ios::in);
    if (!file1.is_open()) {
        cout << "error open file\n";
        return;
    }

    float **pha=new float*[roi_h0];
    for(int i=0;i<roi_h0;i++)
    {
        pha[i]=new float[roi_w0];
    }
    int offset;
    offset=sizeof(float)*((roi_y0-1)*width+roi_x0-1);
    file1.seekg(offset, ios::beg);

    for(int i=0;i<roi_h0;i++)
    {
        file1.read((char *)pha[i], sizeof(float)*roi_w0);
        file1.seekg(sizeof(float)*(width-roi_w0), ios::cur);
    }
    file1.close();

    PSC  pstemp;  
    for(pscIterator Pi=PSpoints.begin();Pi!=PSpoints.end();Pi++)
    {
        pstemp =(*Pi);
        pstemp.data[nFileIndex]=pha[(int)pstemp.m_pos.Y-1][(int)pstemp.m_pos.X-1];   //ÐÐÁÐ
        pscIterator found =PssetTemp.find(*Pi);
        PssetTemp.erase(found);
        PssetTemp.insert(pstemp);
        
    }

    PSpoints=PssetTemp;
    PssetTemp.clear();

    for(int i=0;i<roi_h0;i++)
    {
        delete[]pha[i];

    }
    delete pha;

    return;
}

//¶ÁÈ¡»ùÏß£¬ÈëÉä½Ç£¬Ð±¾à
void CDenuanayNetwork::CTInSAR_ReadBIS(vector<string> filebn,vector<string> fileinc, vector<string> fileslt,int nFileIndex,
                                       int height,int width, pscSet& PSpoints)
{
    pscSet PssetTemp=PSpoints;
    ifstream file1 (filebn[nFileIndex], ios::in);
    if (!file1.is_open()) {
        cout << "error open file\n";
        return;
    }
    ifstream file2 (fileinc[nFileIndex], ios::in);
    if (!file2.is_open()) {
        cout << "error open file\n";
        return;
    }
    ifstream file3 (fileslt[nFileIndex], ios::in);
    if (!file3.is_open()) {
        cout << "error open file\n";
        return;
    }

    float **bn=new float*[height];
    float **inc=new float*[height];
    double **slc=new double*[height];

    for(int i=0;i<height;i++)
    {

        bn[i]=new float[width];
        inc[i]=new float[width];
        slc[i]=new double[width];

    }

    for(int i=0;i<height;i++)
    {

        file1.read((char *)bn[i], sizeof(float)*width);
        file2.read((char *)inc[i], sizeof(float)*width);
        file3.read((char *)slc[i], sizeof(double)*width);
    }

    PSC pstemp; 
    for(pscIterator Pi=PSpoints.begin();Pi!=PSpoints.end();Pi++)
    {
 
        pstemp=(*Pi);
        
        pstemp.bv[nFileIndex]=bn[(int)pstemp.m_pos.Y-1][(int)pstemp.m_pos.X-1];
        pstemp.iAngle[nFileIndex]=inc[(int)pstemp.m_pos.Y-1][(int)pstemp.m_pos.X-1];
        pstemp.sRange[nFileIndex]=slc[(int)pstemp.m_pos.Y-1][(int)pstemp.m_pos.X-1];
        pscIterator found =PssetTemp.find(*Pi);
        PssetTemp.erase(found);
        PssetTemp.insert(pstemp);
    }
    PSpoints=PssetTemp;
    PssetTemp.clear();

    for(int i=0;i<height;i++)
    {
        delete[]bn[i];
        delete[]inc[i];
        delete[]slc[i];


    }
    delete bn;
    delete inc;
    delete slc;

    return ;
}

void CDenuanayNetwork::CTInSAR_ReadBIS(vector<string> filebn,vector<string> fileinc, vector<string> fileslt,int nFileIndex,
                                       int height,int width, pscSet& PSpoints,int roi_x0,int roi_y0,int roi_h0,int roi_w0)
{
    pscSet PssetTemp=PSpoints;
    ifstream file1 (filebn[nFileIndex], ios::in);
    if (!file1.is_open()) {
        cout << "error open file\n";
        return;
    }
    ifstream file2 (fileinc[nFileIndex], ios::in);
    if (!file2.is_open()) {
        cout << "error open file\n";
        return;
    }
    ifstream file3 (fileslt[nFileIndex], ios::in);
    if (!file3.is_open()) {
        cout << "error open file\n";
        return;
    }


    float **bn=new float*[roi_h0];
    float **inc=new float*[roi_h0];
    double **slc=new double*[roi_h0];

    for(int i=0;i<roi_h0;i++)
    {

        bn[i]=new float[roi_w0];
        inc[i]=new float[roi_w0];
        slc[i]=new double[roi_w0];

    }
    int offset,offset1;
    offset=sizeof(float)*((roi_y0-1)*width+roi_x0-1);
    offset1=sizeof(double)*((roi_y0-1)*width+roi_x0-1);
    file1.seekg(offset, ios::beg);
    file2.seekg(offset, ios::beg);
    file3.seekg(offset1, ios::beg);
    for(int i=0;i<roi_h0;i++)
    {

        file1.read((char *)bn[i], sizeof(float)*roi_w0);
        file2.read((char *)inc[i], sizeof(float)*roi_w0);
        file3.read((char *)slc[i], sizeof(float)*roi_w0);

        file1.seekg(sizeof(float)*(width-roi_w0), ios::cur);
        file2.seekg(sizeof(float)*(width-roi_w0), ios::cur);
        file3.seekg(sizeof(double)*(width-roi_w0), ios::cur);
    }
    file1.close();
    file2.close();
    file3.close();


    PSC pstemp; 
    for(pscIterator Pi=PSpoints.begin();Pi!=PSpoints.end();Pi++)
    {
 
        pstemp=(*Pi);
        
        pstemp.bv[nFileIndex]=bn[(int)pstemp.m_pos.Y-1][(int)pstemp.m_pos.X-1];
        pstemp.iAngle[nFileIndex]=inc[(int)pstemp.m_pos.Y-1][(int)pstemp.m_pos.X-1];
        pstemp.sRange[nFileIndex]=slc[(int)pstemp.m_pos.Y-1][(int)pstemp.m_pos.X-1];
        pscIterator found =PssetTemp.find(*Pi);
        PssetTemp.erase(found);
        PssetTemp.insert(pstemp);
    }
    PSpoints=PssetTemp;
    PssetTemp.clear();

    for(int i=0;i<roi_h0;i++)
    {
        delete[]bn[i];
        delete[]inc[i];
        delete[]slc[i];


    }
    delete bn;
    delete inc;
    delete slc;

    return ;
}

//×îÓÅ»¯¹ý³Ì
void CDenuanayNetwork::CTInSAR_Findopt(float options[],int numdiff,float time_c[],float diff[],float upbn[],float upinc[],float upslt[],float wl,float **result)
{
    float start_V[3];
    float start_H[3];
    float Fun_mean;
    float **V_H,**V_H_temp;
    V_H_temp=new float*[1];
    V_H=new float*[3];
    for(int i=0;i<3;i++)
    {
        V_H[i]=new float[2];
    }
    V_H_temp[0]=new float[2];

    //¸ø³õÊ¼Öµ
    V_H[0][0]=options[0]-1;V_H[0][1]=options[1]+1;
    V_H[1][0]=options[0]+1;V_H[1][1]=options[1]+1;
    V_H[2][0]=options[0];V_H[2][1]=options[1]-1;

    float inc_V;
    float inc_H;

    float alpha=1.0;
    float beta=0.5;
    float gamma=2.0;

    int max,min,mild;
    max=min=mild=0;
    float V_mean,H_mean;
    V_mean=0;H_mean=0;

    do{

        float Funcs[3];
        for(int i=0;i<3;i++)
        {
            V_H_temp[0][0]=V_H[i][0];
            V_H_temp[0][1]=V_H[i][1];
            Funcs[i]=Condition_v_h(V_H_temp,wl,numdiff,time_c,diff,upbn,upinc,upslt);
        }

        for(int i=0;i<3;i++)
        {
            if(Funcs[i]>Funcs[max])
            {
                max=i;
            }
            if(Funcs[i]<Funcs[min])
            {
                min=i;
            }

        }
        mild=3-max-min;

        //¼ÆËãÆ½¾ùÖµ
        V_mean=(V_H[min][0]+V_H[mild][0])/2;
        H_mean=(V_H[min][1]+V_H[mild][1])/2;


        //¼ÆËã·´Éäµã
        float  V_reflect=(1+alpha)*V_mean-alpha*V_H[max][0];
        float  H_reflect=(1+alpha)*H_mean-alpha*V_H[max][1];
   
        V_H_temp[0][0]=V_reflect;
        V_H_temp[0][1]=H_reflect;

        //¼ÆËã·´ÉäµãµÄº¯ÊýÖµ
        float Fun_reflect;
        Fun_reflect=0;
        Fun_reflect=Condition_v_h(V_H_temp,wl,numdiff,time_c,diff,upbn,upinc,upslt);

        //·´ÉäºóµÄÈýÖÖ¿ÉÄÜ
        //1¡¢Fun_reflect<Funcs[min]
        if(Fun_reflect<Funcs[min])
        {
            //À©Õ¹µã
            float V_extend=(1+alpha*gamma)*V_mean-alpha*gamma*V_H[max][0];
            float H_extend=(1+alpha*gamma)*H_mean-alpha*gamma*V_H[max][1];

            V_H_temp[0][0]=V_extend;
            V_H_temp[0][1]=H_extend;
            float Fun_extend;
            Fun_extend=Condition_v_h(V_H_temp,wl,numdiff,time_c,diff,upbn,upinc,upslt);

            if(Fun_extend<Fun_reflect)
            {
                V_H[max][0]=V_extend;
                V_H[max][1]=H_extend;
            }
            else
            {
                //À©Õ¹Ê§°Ü
                V_H[max][0]=V_reflect;
                V_H[max][1]=H_reflect;
            }
        }
        else if((Fun_reflect>=Funcs[min])&&(Fun_reflect<=Funcs[mild]))
        {
            //2¡¢Fun_reflectÎ»ÓÚ×îÐ¡ÖµºÍ´ÎÐ¡ÖµÖ®¼ä
            V_H[max][0]=V_reflect;
            V_H[max][1]=H_reflect;
        }
        else
        {
            //3¡¢Fun_reflect´óÓÚ´Î¸ßµãÖµ  Ôò½øÐÐÑ¹Ëõ²½Öè

            float Fun_h,V_h,H_h,V_contract,H_contract,Fun_contract;
            if(Fun_reflect>Funcs[max])
            {
                Fun_h=Funcs[max];
                V_h=V_H[max][0];
                H_h=V_H[max][1];
            }
            else
            {
                Fun_h=Fun_reflect;
                V_h=V_reflect;
                H_h=H_reflect;
            }
            V_contract=V_mean+beta*(V_h-V_mean);
            H_contract=H_mean+beta*(H_h-H_mean);

            V_H_temp[0][0]=V_contract;
            V_H_temp[0][1]=H_contract;
            Fun_contract=Condition_v_h(V_H_temp,wl,numdiff,time_c,diff,upbn,upinc,upslt);

            if(Fun_contract<=Fun_h)
            {
                V_H[max][0]=V_contract;
                V_H[max][1]=H_contract;
            }
            else
            {
                float V_2=V_H[mild][0]+beta*(V_H[min][0]-V_H[mild][0]);
                float H_2=V_H[mild][1]+beta*(V_H[min][1]-V_H[mild][1]);

                float V_3=V_H[max][0]+beta*(V_H[min][0]-V_H[max][0]);
                float H_3=V_H[max][1]+beta*(V_H[min][1]-V_H[max][1]);

                V_H[mild][0]=V_2;V_H[mild][1]=H_2;
                V_H[max][0]=V_3;V_H[max][1]=H_3;
            }

        }

        //ÅÐ¶ÏÌõ¼þ
        float breakflag=0;
        V_H_temp[0][0]=V_mean;
        V_H_temp[0][1]=H_mean;
        Fun_mean=Condition_v_h(V_H_temp,wl,numdiff,time_c,diff,upbn,upinc,upslt);
        for(int i=0;i<3;i++)
        {
            breakflag+=abs(Funcs[i]-Fun_mean);
        }
        if(breakflag<0.0001)
        {
            break;
        }

    }while(1);

    for(int i=0;i<3;i++)
    {
        delete []V_H[i];
    }
    delete V_H;
    delete []V_H_temp[0];
    delete V_H_temp;

    result[0][0]=V_mean;
    result[0][1]=H_mean;
    result[0][2]=Fun_mean;
}


float CDenuanayNetwork::Condition_v_h(float **V_H,float wl,int numdiff,float time_c[],float diff[],float upbn[],float upinc[],float upslt[])
{
    float phaseModel,phasetemp,real,imag;
    complex<float> Fun_reflect;
    phaseModel=0;phasetemp=0;
    Fun_reflect=complex<float>(0.0,0.0);

    //Ò»°ãVµÄµ¥Î»ÊÇmm/a ,¶ø³õÊ¼ÖµµÄµ¥Î»ÊÇm/t,ËùÒÔV/356000´úÌæ
    float  inc_V=V_H[0][0];
    float inc_H=V_H[0][1];
    for(int k=0;k<numdiff;k++)
    {
        phaseModel=4*const_Pi/wl*time_c[k]*inc_V-4*const_Pi/wl*(upbn[k]*inc_H)/(sin(upinc[k])*upslt[k]);
        phasetemp=(diff[k]-phaseModel);
        //Sbas_wrapp(phasetemp);   //¶ÔÃ¿Ò»·ùÍ¼Ïñ¶¼Òª½øÐÐ²øÈÆ
        real=cos(phasetemp);imag=sin(phasetemp);
        Fun_reflect+=complex<float>(real,imag);
    }

    Fun_reflect=complex<float>(Fun_reflect.real()/numdiff,Fun_reflect.imag()/numdiff);
    return -abs(Fun_reflect);  //  ½«¿ÉÄÜµÄ×î´óÖµ×ª»¯µ½×îÐ¡ÖµÉÏÈ¥

}

CDenuanayNetwork::~CDenuanayNetwork(void)
{

}
