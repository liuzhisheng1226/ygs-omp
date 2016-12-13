#pragma once
#include "Point.h"
class Rect{
public:
   int left,right,top,bottom;
   Point TopLeft_t;
   Point BottomRight_t;
public:
   Rect();
   Rect(int, int, int, int);
   void SetRect(int,int,int,int);
   Point &BottomRight();
   Point &TopLeft();
};

