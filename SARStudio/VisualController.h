#pragma once
#include "VisualLayer.h"

class CVisualController
{
public:
	CVisualController(void);
	~CVisualController(void);

	vector<CVisualLayer> Layers;
};

