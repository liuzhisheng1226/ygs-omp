#include "Integration.h"
#include "Delaunay.h"
#include "Lsqr.h"
#include "umfpack.h"
#include "sparse.h"
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>
#include <fstream>

CIntegration::CIntegration(void)
{
}

void CIntegration::Process()
{	
   /* CDlgIntegration dlg;*/
	//if(dlg.DoModal()!=IDOK) return;
	
	//CString strPscFileH=dlg.m_FilepointH;
	////集成相关参数
	//CString file_edge=dlg.m_Fileedge;
	//CString file_edgeH=dlg.m_FileedgeH;
	//CString file_point=dlg.m_Filepoint;
	//CString file_vel=dlg.m_Filevel;
	//CString file_hei=dlg.m_Filehei;
	//CString outputFile_vel=dlg.m_OutputFile_vel;

	////起始点  第一个点sample=150，line=100
	//int startX=dlg.m_startX;
	//int startY=dlg.m_startY;



	////边权阈值
	//float EdgeThreshold=dlg.m_edgeThreshold;
	////大气相关距离
	//float atmosDistance=dlg.m_atmosDistance;

	////分辨单元大小
	//float resolution=dlg.m_resolution;

	//IntegrationPro(file_edge,file_edgeH,file_point,strPscFileH,EdgeThreshold,resolution,atmosDistance,
		/*startX,startY);*/
}

void CIntegration::Batch()
{
}

void CIntegration::IntegrationPro(string file_edge,string file_edgeH,string file_point,string strPscFileH,
                                  float EdgeThreshold,float resolution,float atmosDistance,int startX,int startY)
{
    
    // 读取相关参数
    FILE *fp;
    char buf[500];
    int numdiff,numsar,height,width,roi_x,roi_y,roi_h,roi_w,ctNum;
    float wl;
    fp=fopen(strPscFileH.c_str(),"r");
    while(!feof(fp))
    {
        fscanf(fp,"%s",buf);
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
        if(!strcmp(buf,"[numsar]"))
        {
            fscanf(fp,"%s",buf);
            numsar=atoi(buf);
        }
        if(!strcmp(buf,"[wavelength]"))
        {
            fscanf(fp,"%s",buf);
            wl=atof(buf);
        }
    }
    fclose(fp);
    ifstream file1 (file_edge, ios::in);
    if (!file1.is_open()) {
        cout << "error open file\n";
        return;
    }
    ifstream file2 (file_point, ios::in);
    if (!file2.is_open()) {
        cout << "error open file\n";
        return;
    }

    int flage=0;
    //读取相干点
    pscSet pRSet;
    PSC pointR;
    int nPointCount=0;  //计算点个数；
    while ((flage = file2.read((char *)(&pointR), sizeof(PSC)).gcount()) > 1)
    {
        pRSet.insert(pointR);
        nPointCount++;
    }
    file2.close();

    PSC startPoint;
    startPoint.m_pos.X=startX;
    startPoint.m_pos.Y=startY;
    pscIterator psci=pRSet.find(startPoint);
    if(psci==pRSet.end())
    {
        cout << "wrong start point, please input it again\n";
        return;
    }

    int startIndex=psci->index;

    //读取边
    flage=0;
    edgeReSet InitialEdgeSet;
    edge_Re    tempEdge;
    int nEdgeCount=0; 
    while ((flage = file1.read((char *)(&tempEdge), sizeof(edge_Re)).gcount()) > 1)
    {
        InitialEdgeSet.insert(tempEdge);
        nEdgeCount++;
    }
    file1.close();

    edgeReSet NewEdgeSet=InitialEdgeSet;
    //根据边权阈值删除一些点
    int deleteEdge=0;
    int nUnZeros=0;   //记录非零系数的个数
    for(edgeReIterator eRi=InitialEdgeSet.begin();eRi!=InitialEdgeSet.end();eRi++)
    {
        float distance=sqrt((eRi->m_pos0.X-eRi->m_pos1.X)*(eRi->m_pos0.X-eRi->m_pos1.X)+(eRi->m_pos0.Y-eRi->m_pos1.Y)*(eRi->m_pos0.Y-eRi->m_pos1.Y));
        distance=distance*resolution; 
        if((eRi->edgeCoh<EdgeThreshold)&&(distance<atmosDistance))
        {
            edgeReIterator founder=NewEdgeSet.find(*eRi);
            NewEdgeSet.erase(founder);
            deleteEdge++;
        }

        if(eRi->index0!=startIndex)
            nUnZeros++;

        if(eRi->index1!=startIndex)
            nUnZeros++;

    }
    nEdgeCount=nEdgeCount-deleteEdge ;  //更新边的数量
    InitialEdgeSet.clear();
    InitialEdgeSet=NewEdgeSet;
    NewEdgeSet.clear();  //  后面用于记录新的边


    //根据d_vel d_hei 的mean 和std 进一步限制增量误差，设置可信区间

    //------------直接遍历边，建立方程---------------------

    // 确定所有边中的点的个数

    int *point;
    point=new int[nPointCount];
    int count=0;
    
    pointResult newpoint;  // 存放新编号的边
    prSet  NewpointSet;  //记录新的点集合   并且对新的边从新标记，因为新的边中，点的数量减少了

    newpoint.X=startPoint.m_pos.X;
    newpoint.Y=startPoint.m_pos.Y;
    newpoint.index=0;
    NewpointSet.insert(newpoint);  //添加点
    count=1;
    point[startIndex]=1;

    //修改点的标记
    prIterator tempri;
    for(edgeReIterator eRi=InitialEdgeSet.begin();eRi!=InitialEdgeSet.end();eRi++)
    {
        if(point[eRi->index0]!=1)
        {
            newpoint.index=count;
            newpoint.X=eRi->m_pos0.X;
            newpoint.Y=eRi->m_pos0.Y;
            NewpointSet.insert(newpoint);
            point[eRi->index0]=1;
            count++;
        }
        if(point[eRi->index1]!=1)
        {
            newpoint.index=count;
            newpoint.X=eRi->m_pos1.X;
            newpoint.Y=eRi->m_pos1.Y;
            NewpointSet.insert(newpoint);
            point[eRi->index1]=1;
            count++;
        }

        //修改对于的边中的点的表示
        //先判断这条边是否添加
        edgeReIterator isAddeRi=NewEdgeSet.find(*eRi);
        if(isAddeRi==NewEdgeSet.end())
        {
            
            //对边的两个点进行 index进行修改
            tempEdge=*eRi;
            pointResult temp0(eRi->m_pos0);
            tempri=NewpointSet.find(temp0);
            tempEdge.index0=tempri->index;

            pointResult temp1(eRi->m_pos1);
            tempri=NewpointSet.find(temp1);
            tempEdge.index1=tempri->index;
            NewEdgeSet.insert(tempEdge);
        }
    }


    double *Xv,*Xh,*H,*V;

    V=new double[nEdgeCount];
    H=new double[nEdgeCount];
    Xv=new double[count];
    Xh=new double[count];


    // 保存A’V
    double *Atb=new double[count];
    memset(Atb,0,count*sizeof(double));
    for(int i=0;i<count;i++)
        Atb[i]=0;

    NRsparseMat aT(count,nEdgeCount,nUnZeros);
    int edgecount=0;
    nUnZeros=0;

    //  进度条
    for(edgeReIterator eRi=NewEdgeSet.begin();eRi!=NewEdgeSet.end();eRi++)
    {
        if(eRi->index0!=0)
        {
            aT.val[nUnZeros]=-1;
            aT.row_ind[nUnZeros]=eRi->index0;
            nUnZeros++;
        }

        if(eRi->index1!=0)
        {
            aT.val[nUnZeros]=1;
            aT.row_ind[nUnZeros]=eRi->index1;
            nUnZeros++;
        }

        aT.col_ptr[edgecount+1]=nUnZeros;

        V[edgecount]=eRi->dV;
        H[edgecount]=eRi->dH;
        edgecount++;
    }

    //// 定义a;
    NRsparseMat a=aT.transpose();

    for(int j=0;j<aT.ncols;j++)
    {
        for(int i=aT.col_ptr[j];i<aT.col_ptr[j+1];i++)
        {
            Atb[aT.row_ind[i]]+=aT.val[i]*V[j];
        }
    }


    double *d=new double[edgecount];

    for(int i=0;i<edgecount;i++)
        d[i]=1;
    VecDoub D(edgecount,d);

    ADAT adaT(aT,a);
    adaT.updateD(D);

    nUnZeros=0;
    int L;
    for(int i=0;i<count;i++)
    {
        for(int j=adaT.adat->col_ptr[i];j<adaT.adat->col_ptr[i+1];j++)
        {
            if(adaT.adat->val[j]!=0)
            {
                nUnZeros++;
            }
        }
    }

    //  对adat进行处理
    // 定义每列中的非零元素个数、以及他们的值和位置
    int *Ai=new int[nUnZeros];   //记录每列非零元素的位置
    int *Ap=new int[count+1];    //记录各列非零元素累积个数
    double *Ax=new double[nUnZeros];  //非零元素的值，与Ai对应

    //进行赋值
    Ap[0]=0;   //第一个值为零
    nUnZeros=0;
    for(int i=0;i<count;i++)
    {
        for(int j=adaT.adat->col_ptr[i];j<adaT.adat->col_ptr[i+1];j++)
        {
            if(adaT.adat->val[j]!=0)
            {
                Ax[nUnZeros]=adaT.adat->val[j];
                Ai[nUnZeros]=adaT.adat->row_ind[j];
                nUnZeros++;
            }
        }
        Ap[i+1]=nUnZeros;

        Xv[i]=0;
        Xh[i]=0;
    }

    double *null = (double *) NULL ;
    void *Symbolic, *Numeric ;

    (void) umfpack_di_symbolic(count,count,Ap,Ai,Ax,&Symbolic, null, null);
    (void) umfpack_di_numeric (Ap, Ai, Ax, Symbolic, &Numeric, null, null) ;
    umfpack_di_free_symbolic (&Symbolic) ;
    (void) umfpack_di_solve (UMFPACK_A, Ap, Ai, Ax, Xv, Atb, Numeric, null, null) ;
    umfpack_di_free_numeric (&Numeric) ;


    //------------


    //int count=1;       //
    //int numofsys=0;   //numofsys  用于计算方程个数 ，同时用于矩阵的下表，numofsys从0开始

    //int *point,*findp;
    //point=new int[nPointCount];
    //findp=new int[nPointCount];

    //findp[0]=startIndex;
    //point[findp[0]]=1;   //对于已经存储的点 做标记

    //int t1,t2;
    //t2=1;
    //t1=1;



    //edgeReSet mTempeRSet;     //临时存找到的相邻的边
    //pointResult flagPoint;    //用来找边的点
    //pointResult newpoint;  // 存放新编号的边
    //prSet  NewpointSet;  //记录新的点集合

    ////添加第一个点
    //newpoint.X=startPoint.m_pos.X;
    //newpoint.Y=startPoint.m_pos.Y;
    //newpoint.index=0;
    //NewpointSet.insert(newpoint);  //添加点


    //CProgressBar wndProgress(NULL, "程序进程 -- ", TRUE);
    //wndProgress.SetText("建立方程，请稍候...\n\n"
    //  "按 <取消> 中断进程.");
    //wndProgress.SetRange(1,nEdgeCount);
    //wndProgress.SetStep(1);


    //-------------最开始的根据已知点搜索建立系数矩阵的方法-------
    //prIterator tempri;
    ////根据初始点建立方程
    //while(1)
    //{
    //  t2=count;
    //  if(t1>t2)
    //      break;

    //  //对于新增加的点，在边集合中重新找相邻的点
    //  for(int i=t1;i<=t2;i++)
    //  {
    //      //根据index找点
    //      flagPoint=FindPointByIndex(findp[i-1],pRSet);

    //      //根据已标记的点查找边
    //      mTempeRSet=FindEdgeByPoint(InitialEdgeSet,flagPoint,pRSet);

    //      //对新找到的每一条边
    //      for(edgeReIterator eRi=mTempeRSet.begin();eRi!=mTempeRSet.end();eRi++)
    //      {
    //          //看边的存在情况
    //          float distance=sqrt((eRi->m_pos0.X-eRi->m_pos1.X)*(eRi->m_pos0.X-eRi->m_pos1.X)+(eRi->m_pos0.Y-eRi->m_pos1.Y)*(eRi->m_pos0.Y-eRi->m_pos1.Y));
    //          distance=distance*resolution; 
    //          if((eRi->edgeCoh>EdgeThreshold)&&(distance<atmosDistance))
    //          {

    //              if(point[eRi->index1]!=1)   
    //              {

    //                  //根据旧的index判断是有已经存储了边
    //                  count++;   //找到点的总数
    //                  findp[count-1]=eRi->index1;
    //                  point[eRi->index1]=1;   //标记已存储的点

    //                  //添加新找到的点
    //                  newpoint.X=eRi->m_pos1.X;
    //                  newpoint.Y=eRi->m_pos1.Y;
    //                  newpoint.index=count-1;
    //                  NewpointSet.insert(newpoint);
    //              }
    //              if(point[eRi->index0]!=1)
    //              {
    //                  count++;   //找到点的总数
    //                  findp[count-1]=eRi->index0;
    //                  point[eRi->index0]=1;   //标记已存储的点

    //                  //添加新找到的点
    //                  newpoint.X=eRi->m_pos0.X;
    //                  newpoint.Y=eRi->m_pos0.Y;
    //                  newpoint.index=count-1;
    //                  NewpointSet.insert(newpoint);
    //              }


    //                  //先判断这条边是否添加
    //              edgeReIterator isAddeRi=NewEdgeSet.find(*eRi);
    //              if(isAddeRi==NewEdgeSet.end())
    //              {
    //                  numofsys++;   //边的个数，也是方程的个数

    //                  //对边的两个点进行 index进行修改
    //                  tempEdge=*eRi;
    //                  pointResult temp0(eRi->m_pos0);
    //                  tempri=NewpointSet.find(temp0);
    //                  tempEdge.index0=tempri->index;

    //                  pointResult temp1(eRi->m_pos1);
    //                  tempri=NewpointSet.find(temp1);
    //                  tempEdge.index1=tempri->index;

    //                  NewEdgeSet.insert(tempEdge);
    //                  wndProgress.StepIt();
    //                  wndProgress.PeekAndPump();
    //              }

    //          }

    //      }

    //  }

    //  t1=t2+1;  //更新t1的值，进行下一步的循环
    //}
    //wndProgress.DestroyWindow();


    ////销毁初始的点和边
    // pRSet.clear();
    //InitialEdgeSet.clear();
    //mTempeRSet.clear();    //临时变量
    //delete []findp;
    //findp=NULL;
    //delete []point;
    //point=NULL;
    //------------------------------------------------------------


    //-------------使用压缩矩阵的方法进行求解------------------
    //double *Xv,*Xh,*H,*V;

    //V=new double[numofsys];
    //H=new double[numofsys];
    //Xv=new double[count];
    //Xh=new double[count];
    //int nUnZeros=0;

    //// 保存A’V
    //double *Atb=new double[count];
    //memset(Atb,0,count*sizeof(double));
    //for(int i=0;i<count;i++)
    //  Atb[i]=0;

    ////  计算非零系数的个数
    //for(edgeReIterator eRi=NewEdgeSet.begin();eRi!=NewEdgeSet.end();eRi++)
    //{
    //  if(eRi->index0!=0)
    //      nUnZeros++;

    //  if(eRi->index1!=0)
    //      nUnZeros++;
    //}

    //NRsparseMat aT(count,numofsys,nUnZeros);
    //nUnZeros=0;
    //int edgecount=0;
    //for(edgeReIterator eRi=NewEdgeSet.begin();eRi!=NewEdgeSet.end();eRi++)
    //{
    //  //直接将系数存入aT中
    //  if(eRi->index0!=0)
    //  {
    //      aT.val[nUnZeros]=-1;
    //      aT.row_ind[nUnZeros]=eRi->index0;
    //      nUnZeros++;

    //  }

    //  if(eRi->index1!=0)
    //  {
    //      aT.val[nUnZeros]=1;
    //      aT.row_ind[nUnZeros]=eRi->index1;
    //      nUnZeros++;

    //  }
    //  aT.col_ptr[edgecount+1]=nUnZeros;

    //  V[edgecount]=eRi->dV;
    //  H[edgecount]=eRi->dH;
    //  edgecount++;
    //}
    ////// 定义a;
    //NRsparseMat a=aT.transpose();

    //  需要将A转化成方阵 A'*A,  对于的A'*b

    //for(int j=0;j<aT.ncols;j++)
    //{
    //  for(int i=aT.col_ptr[j];i<aT.col_ptr[j+1];i++)
    //  {
    //      Atb[aT.row_ind[i]]+=aT.val[i]*V[j];
    //  }
    //}


    //double *d=new double[numofsys];

    //for(int i=0;i<numofsys;i++)
    //  d[i]=1;
    //VecDoub D(numofsys,d);

    // ADAT adaT(aT,a);
    // adaT.updateD(D);

    //  nUnZeros=0;
    //  int L;
    //     for(int i=0;i<count;i++)
    //{
    //  for(int j=adaT.adat->col_ptr[i];j<adaT.adat->col_ptr[i+1];j++)
    //  {
    //      if(adaT.adat->val[j]!=0)
    //      {
    //          nUnZeros++;
    //      }
    //  }
    //}

    ////  对adat进行处理
    //// 定义每列中的非零元素个数、以及他们的值和位置
    // int *Ai=new int[nUnZeros];   //记录每列非零元素的位置
    //int *Ap=new int[count+1];    //记录各列非零元素累积个数
    //double *Ax=new double[nUnZeros];  //非零元素的值，与Ai对应

    ////进行赋值
    //Ap[0]=0;   //第一个值为零
    //nUnZeros=0;
    //for(int i=0;i<count;i++)
    //{
    //  for(int j=adaT.adat->col_ptr[i];j<adaT.adat->col_ptr[i+1];j++)
    //  {
    //      if(adaT.adat->val[j]!=0)
    //      {
    //          Ax[nUnZeros]=adaT.adat->val[j];
    //          Ai[nUnZeros]=adaT.adat->row_ind[j];
    //          nUnZeros++;
    //      }
    //  }
    //  Ap[i+1]=nUnZeros;

    //  Xv[i]=0;
    //  Xh[i]=0;
    //}

    //double *null = (double *) NULL ;
    //   void *Symbolic, *Numeric ;

    //(void) umfpack_di_symbolic(count,count,Ap,Ai,Ax,&Symbolic, null, null);
    //   (void) umfpack_di_numeric (Ap, Ai, Ax, Symbolic, &Numeric, null, null) ;
    //   umfpack_di_free_symbolic (&Symbolic) ;
    //   (void) umfpack_di_solve (UMFPACK_A, Ap, Ai, Ax, Xv, Atb, Numeric, null, null) ;
    //   umfpack_di_free_numeric (&Numeric) ;

    //------------------------使用压缩矩阵的方法求解--------------------

    int dot = file_point.find_last_of('/');
    string fileout_point = file_point.substr(0, dot)+"/integratePoint.rmg";
    string fileout_pointH=file_point.substr(0, dot)+"/integratePoint.ldr";
    string fileout_vel=file_point.substr(0, dot)+"/vel.txt";
    //输出点
    ofstream Out_point (fileout_point, ios::out);
    if (!Out_point.is_open()) {
        cout << "error open file\n";
        return;
    }
    //  以点结构体的形式输出集成后的点，同时输出成txt的格式
    fp=fopen(fileout_vel.c_str(),"w");
    if(fp==NULL) return;
    fprintf(fp,"X\tY\tdeformation rate(mm/year)");
    for(prIterator temppi= NewpointSet.begin();temppi!=NewpointSet.end();temppi++)
    {
        newpoint.X=temppi->X;
        newpoint.Y=temppi->Y;
        newpoint.index=temppi->index;
        newpoint.hei=Xh[temppi->index];
        newpoint.vel=Xv[temppi->index];
        Out_point.write((char *)(&newpoint), sizeof(pointResult));


        fprintf(fp,"\n%f",temppi->X);
        fprintf(fp,"\t%f",temppi->Y);
        fprintf(fp,"\t%f",Xv[temppi->index]);
    }
    Out_point.close();
    fclose(fp);
    //清除内存
    NewpointSet.clear();
    NewEdgeSet.clear();

    //输出集成点的头文件
    fp=fopen(fileout_pointH.c_str(),"w");
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
    fprintf(fp,"\n[nPointCount]");
    fprintf(fp,"\n\t\t%d",nPointCount);
    fprintf(fp,"\n[count]");
    fprintf(fp,"\n\t\t%d",count);
    fprintf(fp,"\n[numdiff]");
    fprintf(fp,"\n\t\t%d",numdiff);
    fprintf(fp,"\n[numsar]");
    fprintf(fp,"\n\t\t%d",numsar);
    fprintf(fp,"\n[startY]");
    fprintf(fp,"\n\t\t%d",startY);
    fprintf(fp,"\n[startX]");
    fprintf(fp,"\n\t\t%d",startX);
    fprintf(fp,"\n[resolution]");
    fprintf(fp,"\n\t\t%f",resolution);
    fprintf(fp,"\n[atmosDistance]");
    fprintf(fp,"\n\t\t%f",atmosDistance);
    fprintf(fp,"\n[wavelength]");
    fprintf(fp,"\n\t\t%f",wl);
    fprintf(fp,"\n[EdgeThreshold]");
    fprintf(fp,"\n\t\t%f",EdgeThreshold);

    fclose(fp);

    delete[]V;V=NULL;
    delete[]H;H=NULL;
    delete[]Xv;Xv=NULL;
    delete[]Xh;Xh=NULL;

    delete []Ai;
    delete []Ap;
    delete []Ax;

    delete []Atb;Atb=NULL;
    delete []d;
    delete []point;
    point=NULL;
}


//  根据点  查找包含这个点的所有边
//  mpsSet  还没有标记的点集
//  eRSet   还没有找到的边集
edgeReSet CIntegration::FindEdgeByPoint(edgeReSet& eRSet,pointResult startPoint,pscSet mpsSet)
{
    edgeReSet temperSet;
    edgeReIterator temperi;
    PointF p0;
    PointF p1;
    for(pscIterator pi=mpsSet.begin();pi!=mpsSet.end();pi++)
    {
        p0=PointF(startPoint.X,startPoint.Y);
        p1=PointF(pi->m_pos.X,pi->m_pos.Y);
        edge_Re tempEdge(p0,p1);
        temperi=eRSet.find(tempEdge);
        if(temperi!=eRSet.end())
        {
            temperSet.insert(*temperi);
            eRSet.erase(temperi);
        }
        else
        {
            //交换边的顶点次序
            tempEdge.m_pos0=p1;
            tempEdge.m_pos1=p0;
            temperi=eRSet.find(tempEdge);
            if(temperi!=eRSet.end())
            {
                temperSet.insert(*temperi);
                eRSet.erase(temperi);
            }
        }
    }
    return temperSet;
}

//  根据点的index 查找点
pointResult CIntegration::FindPointByIndex(int pointIndex, pscSet& mprSet)
{
    pscIterator pri;
    pointResult result;
    for(pri=mprSet.begin();pri!=mprSet.end();pri++)
    {
        if(pri->index==pointIndex)
        {
            //  找到后，将这个点给删除
            result.X=pri->m_pos.X;
            result.Y=pri->m_pos.Y;
            mprSet.erase(pri);
            break;
        }
    }
    return result;
}

CIntegration::~CIntegration(void)
{
}
