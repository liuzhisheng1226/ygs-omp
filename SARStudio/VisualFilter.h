#pragma once
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <math.h>



class CVisualFilter
{
public:
	CVisualFilter(void);
	~CVisualFilter(void);


	//static BYTE Convert2Grayscale(double intensity,double lw,double up);
	static BYTE Convert2Grayscale(double intensity,double lw,double up,double factor);
	static BYTE* Zoom(BYTE* data,CSize srcSize,CSize dstSize);
	static BYTE* Zoom(BYTE* data,CSize srcSize,double scale);
};



