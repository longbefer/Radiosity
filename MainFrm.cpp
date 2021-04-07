
// MainFrm.cpp: CMainFrame 类的实现
//

#include "pch.h"
#include "framework.h"
#include "Test.h"
#include "CControlPanel.h"
#include "CPaintPanel.h"
#include "MainFrm.h"
extern volatile bool ENDDRAW = true;
extern volatile bool ENDPROGRAM = false;
extern volatile bool ENDRENDER = true;
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_MESSAGE(WM_UpdatePaintPanel, &CMainFrame::OnUpdatePaintPanel)
	//ON_COMMAND(ID_FILE_SAVE, &CMainFrame::OnFileSave)
	ON_WM_CLOSE()
	ON_COMMAND(ID_FILE_SAVE_USER, &CMainFrame::OnFileSaveUser)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 构造/析构

CMainFrame::CMainFrame() noexcept
{
	// TODO: 在此添加成员初始化代码
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return TRUE;
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序



BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// TODO: 在此添加专用代码和/或调用基类

	// 创建拆分窗口
	m_spliterWnd.CreateStatic(this, 1, 2);
	m_spliterWnd.CreateView(0, 0, RUNTIME_CLASS(CControlPanel), CSize(350, 100), pContext);
	m_spliterWnd.CreateView(0, 1, RUNTIME_CLASS(CPaintPanel), CSize(100, 100), pContext);
	//return CFrameWnd::OnCreateClient(lpcs, pContext);
	return TRUE;
}


afx_msg LRESULT CMainFrame::OnUpdatePaintPanel(WPARAM wParam, LPARAM lParam)
{
	CCreateContext context;
	if (wParam == WM_UpdatePaintPanel) {
		// 开始渲染
		//MessageBox(TEXT("开始渲染"));

		// 更新CPaintPanel
		CPaintPanel* pPaintView = (CPaintPanel*)m_spliterWnd.GetPane(0, 1);
		if (pPaintView->s) pPaintView->Init();  // 若pPaintView界面已经存在scene，则直接渲染
		else pPaintView->SetScene((Scene*)(lParam));
	}
	return 0;
}


//void CMainFrame::OnFileSave()
//{
//	// TODO: 在此添加命令处理程序代码
//	CRect rect;
//	GetClientRect(&rect);
//	//将文件保存为命名
//	CFileDialog dialog(FALSE, L"Bitmap File(*.bmp)|*.bmp|All Files (*.*)|*.*||", NULL, NULL, L"Bitmap File(*.bmp)|*.bmp|All Files (*.*)|*.*||", this);
//	//False指定为保存文件，.bmp指定为保存文件扩展名
//	CString pathName = 0/*, fileHead, fileName*/;
//	if (dialog.DoModal() == IDOK) {
//		pathName = dialog.GetPathName();//获取文件名（路径）
//		CImage image;
//		image.Create(rect.Width(), rect.Height(), 32);//图像的长和宽
//		CDC* pDC = m_spliterWnd.GetPane(0, 1)->GetDC();
//		CDC dstDC;
//		dstDC.CreateCompatibleDC(pDC);
//		dstDC.SelectObject(image);
//		dstDC.StretchBlt(0, 0, rect.Width(), rect.Height(), pDC, 0, 0, rect.Width(), rect.Height(), SRCCOPY);
//		image.Save(pathName);
//	}
//}


void CMainFrame::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//MessageBox(TEXT("请稍等，正在关闭程序"));
	CPaintPanel* pPaintView = (CPaintPanel*)m_spliterWnd.GetPane(0, 1);
	pPaintView->KillTimer(1);
	CControlPanel* pControlView = (CControlPanel*)m_spliterWnd.GetPane(0, 0);
	ENDPROGRAM = true; // 在此结束程序
	while (!ENDRENDER) {} // 没有结束渲染则等待（这个可能要等一段时间了）
	while (!ENDDRAW) {} // 没有结束绘制则等待
	//while(!pControlView->scene.bFinish) {}
	//while (!pPaintView->bDraw) {};
	//pPaintView->OnClose();
	CFrameWnd::OnClose();
}


void CMainFrame::OnFileSaveUser()
{
	// TODO: 在此添加命令处理程序代码
	CPaintPanel* pPaintView = (CPaintPanel*)m_spliterWnd.GetPane(0, 1);
	CRect rect;
	pPaintView->GetClientRect(&rect);
	//将文件保存为命名
	CFileDialog dialog(FALSE, L"Bitmap File(*.bmp)|*.bmp|All Files (*.*)|*.*||", NULL, NULL, L"Bitmap File(*.bmp)|*.bmp|All Files (*.*)|*.*||", this);
	//False指定为保存文件，.bmp指定为保存文件扩展名
	CString pathName = 0/*, fileHead, fileName*/;
	if (dialog.DoModal() == IDOK) {
		pathName = dialog.GetPathName();//获取文件名（路径）
		CImage image;
		image.Create(rect.Width(), rect.Height(), 32);//图像的长和宽
		CDC* pDC = pPaintView->GetDC();
		CDC dstDC;
		dstDC.CreateCompatibleDC(pDC);
		dstDC.SelectObject(image);
		dstDC.StretchBlt(0, 0, rect.Width(), rect.Height(), pDC, 0, 0, rect.Width(), rect.Height(), SRCCOPY);
		image.Save(pathName);
	}
}
