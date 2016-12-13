#pragma once
#include "DlgEnviWindow.h"
#include "VisualView.h"
#include "DlgEnviLinkDisplay.h"
#include "DlgEnviPointPicker.h"
#include "DlgEnviColorTable.h"

/*
*����Envi�����ڽ���
*/
class CEnviWindowSuite
{
#pragma region �Զ���RECT�ṹ��
	struct StructRectPair
	{
		CRect client;
		CRect window;
		void Update(CDlgEnviWindow &dlg)
		{
			dlg.GetClientRect(&client);
			dlg.GetWindowRect(&window);
		}
	};

	struct StructEnviRect
	{
		StructRectPair img;
		StructRectPair scr;
		StructRectPair zom;
	private:
		CDlgEnviWindow *_img;
		CDlgEnviWindow *_scr;
		CDlgEnviWindow *_zom;
	public:
		StructEnviRect(){}
		StructEnviRect(CDlgEnviWindow &img,CDlgEnviWindow &scr,CDlgEnviWindow &zom)
		{
			this->_img=&img;
			this->_scr=&scr;
			this->_zom=&zom;
			Update();
		}
		void Update()
		{
			this->img.Update(*_img);
			this->scr.Update(*_scr);
			this->zom.Update(*_zom);
		}
	};
#pragma endregion


public:
	//CEnviWindowSuite();
	/// <summary>
	/// ���캯��
	/// </summary>
	/// <param name="id">����id</param>
	CEnviWindowSuite(int id);
	~CEnviWindowSuite(void);

	
	/// <summary>
	/// ����λ�õ���
	/// </summary>
	/// <param name="x">��Ļx����</param>
	/// <param name="y">��Ļy����</param>
	/// <param name="isDrag">�Ƿ�ͨ����Ͱ�޸Ĵ���</param>
	void WindowLocate(int x,int y,bool isDrag);
	/// <summary>
	/// ������ʾ��View����
	/// </summary>
	/// <param name="view">view����</param>
	void ResetView(CVisualView* view);
	/// <summary>
	/// ������ʾ��View����
	/// </summary>	
	void ResetView();
	/// <summary>
	/// ��ʾ��ǰ��EnviWinsow
	/// </summary>
	/// <param name="show">true��ʾfalse����</param>
	void EnviWindowShow(bool show);

	/// <summary>
	/// ����ZOM����ͼ��
	/// </summary>
	/// <param name="way">0��С��1�Ŵ�</param>
	void Zoom(int way);

	void WindowResize();
	
	/// <summary>
	/// ָ��������ֵ
	/// </summary>
	/// <param name="p">ͼ������</param>
	/// <param name="lpPxValue">ͼ��ԭ����ֵ</param>
	/// <param name="lpDpValue">ͼ����Ļֵ</param>
	void CursorValue(CPoint p,CString &lpPxValue);
	
	void Destroy();

	void LinkDisplay(int cur);
	void LinkUpdate();
	void LinkSwap();
	void LinkRestore();
	void UnLinkDisplay();

	void Enable(bool flag);

	
	byte* Filtering(byte* data,CSize size);
	
	/// <summary>
	/// �жϴ����Ƿ����ָ���ļ�
	/// </summary>
	/// <param name="lpFilename">�ļ�ȫ·����</param>
	/// <return></return>
	bool IsInclude(CString lpFilename);

	void ColorTable();
	void ColorMapping(StructColorTable colorTable);
	void ColorMapping();

	void PointPicker();
	bool m_bIsPointPicker;

private:
	/// <summary>
	/// ���ڳ�ʼ������
	/// </summary>
	/// <param name="&window">��������</param>
	/// <param name="windowSize">���ڴ�С</param>
	/// <param name="id">���������׼�id</param>
	/// <param name="windowType">��������id</param>
	void WindowInit(CDlgEnviWindow &window, int windowSize,int id,ENUM_WINDOW_TYPE windowType);
	/// <summary>
	/// ����EnviWindow״̬��ʹ���ܹ�(����)��Ӧ�û�����
	/// </summary>
	/// <param name="flag">trueʹEnviWindow����Ӧ��false����Ӧ</param>
	void ResetState(bool flag);
	/// <summary>
	/// ����Box��Сλ��
	/// </summary>	
	void ResetBox();	
	
	
	
public:

	int m_nId;													//<���ڱ�� ��1��ʼ
	CVisualView* VISUAL_VIEW;									//<��ǰEnviWindow��Ӧ��View

	CDlgEnviWindow ENVI_WINDOW_SCR;								//<Scroll ����
	CDlgEnviWindow ENVI_WINDOW_ZOM;								//<Zoom	����
	CDlgEnviWindow ENVI_WINDOW_IMG;								//<Image ����
	StructEnviRect ENVI_RECT;									//<Window ����λ�ô�С��Ϣ
	

private:
	int m_nScale;												//< ��ǰZOM���ڷŴ���
	StructColorTable m_ColorTable;
	bool m_bIsFiltering;
	

	static const int CONST_WINDOW_SIZE_IMG=400;					
	static const int CONST_WINDOW_SIZE_ZOM=200;
	static const int CONST_WINDOW_SIZE_SCR=256;
	
	static const int CONST_SCALE_ZOM=4;
	static const int CONST_SCALE_MAX=100;
	static const int CONST_BOX_SIZE=50;
	
	CEnviWindowSuite *LINK_WINDOW;


	CDlgEnviColorTable m_dlgColorTable;
	CDlgEnviPointPicker m_dlgPointPicker;
	

};
