#pragma once
/// @file VisualLayer.h
/// @brief VisualLayer.h文件，显示图层结构基类
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


	byte* data;								//<图层图像数据	
	CSize size;								//<图层图像数据大小



	
};

