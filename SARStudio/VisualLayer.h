#pragma once
/// @file VisualLayer.h
/// @brief VisualLayer.h�ļ�����ʾͼ��ṹ����
/// @author Zhaolong

enum ENUM_LAYER_TYPE
{
	Primary,

};

class CVisualLayer
{
public:
	CVisualLayer(void);
	~CVisualLayer(void);


	byte* data;								//<ͼ��ͼ������	
	CSize size;								//<ͼ��ͼ�����ݴ�С



	
};

