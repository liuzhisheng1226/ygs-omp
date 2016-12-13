#pragma once
#include "DlgEnviWindow.h"
#include "VisualView.h"
#include "DlgEnviLinkDisplay.h"
#include "DlgEnviPointPicker.h"
#include "DlgEnviColorTable.h"

/*
*管理Envi三窗口界面
*/
class CEnviWindowSuite
{
#pragma region 自定义RECT结构体
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
	/// 构造函数
	/// </summary>
	/// <param name="id">窗口id</param>
	CEnviWindowSuite(int id);
	~CEnviWindowSuite(void);

	
	/// <summary>
	/// 窗口位置调整
	/// </summary>
	/// <param name="x">屏幕x坐标</param>
	/// <param name="y">屏幕y坐标</param>
	/// <param name="isDrag">是否通过拖桶修改窗口</param>
	void WindowLocate(int x,int y,bool isDrag);
	/// <summary>
	/// 重置显示的View对象
	/// </summary>
	/// <param name="view">view对象</param>
	void ResetView(CVisualView* view);
	/// <summary>
	/// 重置显示的View对象
	/// </summary>	
	void ResetView();
	/// <summary>
	/// 显示当前的EnviWinsow
	/// </summary>
	/// <param name="show">true显示false隐藏</param>
	void EnviWindowShow(bool show);

	/// <summary>
	/// 缩放ZOM窗口图像
	/// </summary>
	/// <param name="way">0缩小，1放大</param>
	void Zoom(int way);

	void WindowResize();
	
	/// <summary>
	/// 指定点像素值
	/// </summary>
	/// <param name="p">图像坐标</param>
	/// <param name="lpPxValue">图像原像素值</param>
	/// <param name="lpDpValue">图像屏幕值</param>
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
	/// 判断窗口是否包含指定文件
	/// </summary>
	/// <param name="lpFilename">文件全路径名</param>
	/// <return></return>
	bool IsInclude(CString lpFilename);

	void ColorTable();
	void ColorMapping(StructColorTable colorTable);
	void ColorMapping();

	void PointPicker();
	bool m_bIsPointPicker;

private:
	/// <summary>
	/// 窗口初始化函数
	/// </summary>
	/// <param name="&window">窗口引用</param>
	/// <param name="windowSize">窗口大小</param>
	/// <param name="id">窗口所属套件id</param>
	/// <param name="windowType">窗口类型id</param>
	void WindowInit(CDlgEnviWindow &window, int windowSize,int id,ENUM_WINDOW_TYPE windowType);
	/// <summary>
	/// 重置EnviWindow状态，使其能够(不能)相应用户操作
	/// </summary>
	/// <param name="flag">true使EnviWindow可响应，false不响应</param>
	void ResetState(bool flag);
	/// <summary>
	/// 重置Box大小位置
	/// </summary>	
	void ResetBox();	
	
	
	
public:

	int m_nId;													//<窗口编号 从1开始
	CVisualView* VISUAL_VIEW;									//<当前EnviWindow对应的View

	CDlgEnviWindow ENVI_WINDOW_SCR;								//<Scroll 窗口
	CDlgEnviWindow ENVI_WINDOW_ZOM;								//<Zoom	窗口
	CDlgEnviWindow ENVI_WINDOW_IMG;								//<Image 窗口
	StructEnviRect ENVI_RECT;									//<Window 矩阵位置大小信息
	

private:
	int m_nScale;												//< 当前ZOM窗口放大倍数
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
