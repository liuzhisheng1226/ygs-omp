#pragma once
#include <set>
#include <algorithm>
#include <math.h>
#include <iterator>
#include <cassert>
using namespace std;


typedef float REAL;

struct PointF
{
    PointF():X(0),Y(0){}
    PointF(const PointF& p):X(p.X),Y(p.Y){}
    PointF(float x, float y) : X(x), Y(y)   {}
    PointF operator+(const PointF& p) const { return PointF(X + p.X, Y + p.Y); }
    PointF operator-(const PointF& p) const { return PointF(X - p.X, Y - p.Y); }
    bool operator<(const PointF& p)const
    {
        if(X == p.X) return Y<p.Y;
        return X < p.X;     
    }
    bool operator==(const PointF& p)const
    {
        return X == p.X && Y == p.Y;        
    }
    float X;   //height
    float Y;   //width
};

struct pointResult
{
    bool operator<(const pointResult& p)const
    {
        if(X == p.X) return Y<p.Y;
        return X < p.X;     
    }
    bool operator==(const pointResult& p)const
    {
        return X == p.X && Y == p.Y;        
    }
    pointResult(float x,float y): X(x),Y(y){}
    pointResult(const PointF &p): X(p.X),Y(p.Y){}
    pointResult(): X(0),Y(0){}
    float X;
    float Y;
    float vel;   //线性形变
    float hei;   //高程
    int index;   //点的标示
    float residuePha[100];  //残余相位
};
typedef set<pointResult> prSet;
typedef set<pointResult>::iterator prIterator;
typedef set<pointResult>::const_iterator cprIterator;

struct pointResiduUnwPha
{
    bool operator<(const pointResiduUnwPha& p)const
    {
        if(X == p.X) return Y<p.Y;
        return X < p.X;     
    }
    bool operator==(const pointResiduUnwPha& p)const
    {
        return X == p.X && Y == p.Y;        
    }
    pointResiduUnwPha(float x,float y): X(x),Y(y){}
    pointResiduUnwPha(const pointResiduUnwPha &p): X(p.X),Y(p.Y),index(p.index){
        for(int i=0;i<100;i++)
            residuePha[i]=p.residuePha[i];
    }
    pointResiduUnwPha(): X(0),Y(0){}
    float X;
    float Y;
    int index;   //点的标示
    float residuePha[100];  //解缠残余相位
};
typedef set<pointResiduUnwPha> prupSet;
typedef set<pointResiduUnwPha>::iterator prupIterator;
typedef set<pointResiduUnwPha>::const_iterator cprupIterator;


//记录所有相干点在所有干涉图中的相位，干涉图数默认设为100
class PSC
{
public:
    PointF m_pos;
    float data[100];//相位数据
    int deltaD[100];//时间基线
    float bv[100];  //空间基线
    float hv[100];
    float sRange[100]; //斜距
    float iAngle[100]; //入射角
    int index;  //点的标识

    PSC():m_pos(0.0,0.0){}
    PSC(int x,int y):m_pos(x,y){}
    PSC(float x,float y):m_pos(x,y){}
    PSC(int x,int y,int ind):m_pos(x,y),index(ind){}
    PSC(float x,float y,int ind):m_pos(x,y),index(ind){}

    bool operator<(const PSC& p)const
    {
        if(m_pos.X ==p.m_pos.X) return m_pos.Y<p.m_pos.Y;
        return m_pos.X < p.m_pos.X;
    }

    bool operator==(const PSC& p) const
    {
        return m_pos.X ==p.m_pos.X && m_pos.Y==p.m_pos.Y;
    }   
};

typedef set<PSC> pscSet;
typedef set<PSC>::iterator pscIterator;
typedef set<PSC>::const_iterator cpscIterator;

class vertex
{
public:
    PointF m_Pnt;
    vertex():m_Pnt(0.0,0.0) {}
    vertex(const vertex& v):m_Pnt(v.m_Pnt ){}
    vertex(float x, float y)        : m_Pnt(x, y)               {}
    vertex(int x, int y)        : m_Pnt((float) x, (float) y)   {}

    bool operator<(const vertex& v) const
    {
        if (m_Pnt.X == v.m_Pnt.X) return m_Pnt.Y < v.m_Pnt.Y;
        return m_Pnt.X < v.m_Pnt.X;
    }

    bool operator==(const vertex& v) const
    {
        return m_Pnt.X == v.m_Pnt.X && m_Pnt.Y == v.m_Pnt.Y;
    }
    

    float GetX()    const   { return m_Pnt.X; }
    float GetY() const  { return m_Pnt.Y; }

    void SetX(float x)      { m_Pnt.X = x; }
    void SetY(float y)      { m_Pnt.Y = y; }

    const PointF& GetPoint() const  { return m_Pnt; }

    
};
typedef set<vertex> vertexSet;
typedef set<vertex>::iterator vIterator;
typedef set<vertex>::const_iterator cvIterator;

//edge
class edge
{
public:
    edge(const edge& e):m_Pv0(e.m_Pv0),m_Pv1(e.m_Pv1),d_vel(e.d_vel),d_hei(e.d_hei),edgeIndex(e.edgeIndex){}
    edge():m_Pv0(0),m_Pv1(0),d_vel(0),d_hei(0),edgeIndex(0){}
    edge(const vertex * Pv0,const vertex * Pv1):m_Pv0(Pv0),m_Pv1(Pv1),d_vel(0),d_hei(0),edgeIndex(0){}

    bool operator<(const edge& e) const
    {
        if (m_Pv0 == e.m_Pv0) return * m_Pv1 < * e.m_Pv1;
        return * m_Pv0 < * e.m_Pv0;
    }

    float length()
    {
        float result=0;
        result=sqrt((m_Pv1->m_Pnt.X-m_Pv0->m_Pnt.X)*(m_Pv1->m_Pnt.X-m_Pv0->m_Pnt.X)+(m_Pv1->m_Pnt.Y-m_Pv0->m_Pnt.Y)*(m_Pv1->m_Pnt.Y-m_Pv0->m_Pnt.Y));
        return result;
    }


    const vertex * m_Pv0;
    const vertex * m_Pv1;
    float d_vel;
    float d_hei;
    int edgeIndex;
};
typedef set<edge> edgeSet;
typedef set<edge>::iterator edgeIterator;
typedef set<edge>::const_iterator cedgeIterator;


class edge_Re
{
public:
    PointF m_pos0,m_pos1;
    double dV;
    double dH;
    double edgeCoh;
    int index0;
    int index1;
    
    edge_Re() : m_pos0(0.0,0.0),m_pos1(0.0,0.0),dV(0.0),dH(0.0) {}
    edge_Re(PointF p1,PointF p2) : m_pos0(p1), m_pos1(p2) {}
    
    bool operator<(const edge_Re& p) const
    {
        if(m_pos0 == p.m_pos0) return m_pos1<p.m_pos1;
        return m_pos0 < p.m_pos0;
    }
    
    bool operator==(const edge_Re& p) const
    {
        return m_pos0 ==p.m_pos0 && m_pos1==p.m_pos1;
    }

};
typedef set<edge_Re> edgeReSet;
typedef set<edge_Re>::iterator edgeReIterator;
typedef set<edge_Re>::const_iterator cedgeReIterator;




//triangle
class triangle
{
public:
    triangle(const triangle& tri)
        : m_Center(tri.m_Center)
        , m_R(tri.m_R)
        , m_R2(tri.m_R2)
    {
        for (int i = 0; i < 3; i++) m_Vertices[i] = tri.m_Vertices[i];
    }

    triangle(const vertex * p0, const vertex * p1, const vertex * p2)
    {
        m_Vertices[0] = p0;
        m_Vertices[1] = p1;
        m_Vertices[2] = p2;
        SetCircumCircle();
    }
    triangle(const vertex * pV)
    {
        for (int i = 0; i < 3; i++) m_Vertices[i] = pV++;
        SetCircumCircle();
    }
    //
    const vertex * GetVertex(int i) const
    {
        assert(i >= 0);
        assert(i < 3);
        return m_Vertices[i];
    }

    bool operator<(const triangle& tri) const
    {
    if (m_Center.X == tri.m_Center.X) return m_Center.Y < tri.m_Center.Y;
        return m_Center.X < tri.m_Center.X;
    }

    //const triangle& operator=(triangle& tri) 
    //{
    //  if(&tri != this)
    //  {
    //      m_Center = tri.m_Center;
    //      m_R = tri.m_R;
    //      m_R2= tri.m_R2;
    //      for (int i = 0; i < 3; i++) 
    //          m_Vertices[i] = tri.m_Vertices[i];
    //  }
    //  return *this;
    //}

    void Tri2Edge(const triangle& tri,edgeSet &eSet)
    {

        eSet.insert(edge(tri.GetVertex(0),tri.GetVertex(1)));
        eSet.insert(edge(tri.GetVertex(1),tri.GetVertex(2)));
        eSet.insert(edge(tri.GetVertex(2),tri.GetVertex(0)));
    }

    bool IsLeftOf(cvIterator itVertex) const
    {
        // returns true if * itVertex is to the right of the triangle's circumcircle
        return itVertex->GetPoint().X > (m_Center.X + m_R);
    }
    bool CCEncompasses(cvIterator itVertex) const
    {
        // First check boundary box.判断点不在三角形的外包络矩形内
        /*float x = itVertex->GetPoint().X;
                
        if (x > (m_Center.X + m_R)) return false;
        if (x < (m_Center.X - m_R)) return false;

        float y = itVertex->GetPoint().Y;
                
        if (y > (m_Center.Y + m_R)) return false;
        if (y < (m_Center.Y - m_R)) return false;*/
        PointF dist = itVertex->GetPoint() - m_Center;      // the distance between v and the circle center
        float dist2 = dist.X * dist.X + dist.Y * dist.Y;        // squared
        return dist2 <= m_R2;                               // compare with squared radius
    }

protected:
    const vertex * m_Vertices[3];   // the three triangle vertices
    PointF m_Center;                // center of circumcircle
    float m_R;          // radius of circumcircle
    float m_R2;         // radius of circumcircle, squared

    void SetCircumCircle();
};
typedef multiset<triangle> triangleSet;
typedef multiset<triangle>::iterator tIterator;
typedef multiset<triangle>::const_iterator ctIterator;

class Delaunay
{
public:
    // create triangle by vertex
    void Triangulate(const vertexSet& vertices, triangleSet& output);

    //create complex tiangle by vertex
    void ComplexTriangulate(const vertexSet& vertices, edgeSet& edges,const float& dis);

    //get edges from triangle
    void TrianglesToEdges(const triangleSet& triangles, edgeSet& edges);

    //get edges count
    int  Count_edge(const edgeSet &edgeS);


    bool mtriangleIsCompleted(cvIterator itVertex, triangleSet& output, const vertex SuperTriangle[3],const triangle& tri );
    bool mvertexIsInCircumCircle(cvIterator itVertex, edgeSet& edges,const triangle& tri);
    bool mtriangleHasVertex(const vertex SuperTriangle[3],const triangle tri);
    bool misvertexInCircle(cvIterator itVertex0,cvIterator itVertex1 ,const float& dis);
    void mAnotherHandleEdge(const vertex * p0, const vertex * p1, edgeSet& edges);
    
protected:
        void HandleEdge(const vertex * p0, const vertex * p1, edgeSet& edges);
    
};

