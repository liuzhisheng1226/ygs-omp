#pragma once

/// @addtogroup Core 
/// @brief Core模块,定义了系统的图像的元数据、接口
/// @details Core模块为静态lib,定义了RMG数据文件格式、RMG头文件格式以及插件模块的通用处理接口。所有插件模块必须引用该模块

/// @file Core.h
/// @brief Core模块引用文件
#include "Point.h"
#include "PreOrb.h"
#include "RdWrIni.h"
#include "Rect.h"
#include "RMGBase.h"
#include "RMGHeader.h"
#include "RMGImage.h"

#include "IProcessable.h"
