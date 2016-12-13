#pragma once
#include "VisualBand.h"

/*
*��ʾ�ṹ��
*/


class CVisualView
{
public:
	CVisualView(CVisualBand* b);
	CVisualView(CVisualBand* br,CVisualBand* bg,CVisualBand* bb);
	~CVisualView(void);


	bool m_bIsRgb;


	//StructMeta META_IMAGE_ZOM;
	StructMeta META_IMAGE_SCR;
	//StructMeta META_IMAGE_IMG;
	StructMeta META_IMAGE_IZC;
	

	/*
	*@name :	Visualize
	*@func :	��3���������ݿ��ӻ�
	*/
	//void Visualize(CRect imgRect,CRect zomRect,double zomScale);
	bool Visualize(CRect rect);

	BYTE *Bytes2Vision(BYTE* br,BYTE *bg,BYTE *bb,CSize size);
	BYTE *Bytes2Vision(BYTE* b,CSize size);

public:
	CVisualBand *m_oBand_R;
	CVisualBand *m_oBand_G;
	CVisualBand *m_oBand_B;

};

