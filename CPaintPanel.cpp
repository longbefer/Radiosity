// CPaintPanel.cpp: 实现文件
//

#include "pch.h"
#include "Test.h"
#include "CPaintPanel.h"
extern volatile bool ENDDRAW;
extern volatile bool ENDRENDER;

// CPaintPanel

IMPLEMENT_DYNCREATE(CPaintPanel, CView)

CPaintPanel::CPaintPanel()
{
	//s->Init();
	bRender = false;
	bDraw = true;
}

CPaintPanel::~CPaintPanel()
{
}

BEGIN_MESSAGE_MAP(CPaintPanel, CView)
	ON_WM_TIMER()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CPaintPanel 绘图

DWORD __stdcall DoubleBuffer(void* pWind)
{
	ENDDRAW = false;

	CPaintPanel* pView = (CPaintPanel*)(pWind);
	CRect rect;//客户区矩形
	pView->GetClientRect(&rect);
	CDC* pDC = pView->GetDC(); // 获取当前绘制板块

	pDC->SetMapMode(MM_ANISOTROPIC);//pDC自定义坐标系
	pDC->SetWindowExt(rect.Width(), rect.Height());//设置窗口范围
	pDC->SetViewportExt(rect.Width(), -rect.Height());//x轴水平向右，y轴垂直向上
	pDC->SetViewportOrg(rect.Width() / 2, rect.Height() / 2);//客户区中心为原点
	CDC memDC;//内存DC	
	memDC.CreateCompatibleDC(pDC);//建立与显示pDC兼容的memDC
	CBitmap NewBitmap, * pOldBitmap;//内存中承载图像的临时位图 
	NewBitmap.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());//创建兼容位图 
	pOldBitmap = memDC.SelectObject(&NewBitmap);//将兼容位图选入memDC
	rect.OffsetRect(-rect.Width() / 2, -rect.Height() / 2);
	memDC.SetMapMode(MM_ANISOTROPIC);//memDC自定义坐标系
	memDC.SetWindowExt(rect.Width(), rect.Height());
	memDC.SetViewportExt(rect.Width(), -rect.Height());
	memDC.SetViewportOrg(rect.Width() / 2, rect.Height() / 2);
	//memDC.FillSolidRect(rect, pDC->GetBkColor());//设置背景色
	if (pView->s) pView->s->Draw(&memDC); // 绘制对象
	//if (pView->s) pView->s->Draw(pDC);
	pDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &memDC, -rect.Width() / 2, -rect.Height() / 2, SRCCOPY);//将内存memDC中的位图拷贝到显示pDC中
	memDC.SelectObject(pOldBitmap);//恢复位图
	NewBitmap.DeleteObject();//删除位图
	memDC.DeleteDC();//删除memDC
	ENDDRAW = true;
	return ::GetCurrentThreadId();
}

void CPaintPanel::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO:  在此添加绘制代码

	if (s && bRender) {
		HANDLE hThread;
		DWORD ThreadID;
		// 使用其他线程来渲染
		hThread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)RenderThread, this, 0, &ThreadID);
		CloseHandle(hThread);
		bRender = false;
	}
	if (s && ENDDRAW/*bDraw*/ /*&& (!s->bFinish)*/) { // 启用线程绘制，互斥使用
		bDraw = false;
		HANDLE hThread;
		DWORD ThreadID;
		hThread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)DoubleBuffer, this, 0, &ThreadID);
		CloseHandle(hThread);
		bDraw = true;
	}
	SetTimer(1, 1500, nullptr);
}

void CPaintPanel::SetScene(Scene*s)
{
	this->s = s;
	Init();
}

void CPaintPanel::Init()
{
	while (!bDraw) {} // 阻塞线程，等绘制完成再初始化（否则会导致初始化删除patchs的内容，导致访问出现问题）
	s->Init();
	bRender = true;
}


// CPaintPanel 诊断

#ifdef _DEBUG
void CPaintPanel::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CPaintPanel::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CPaintPanel 消息处理程序


void CPaintPanel::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	Invalidate(false);
	CView::OnTimer(nIDEvent);
}

DWORD __stdcall RenderThread(void*pWind)
{
	ENDRENDER = false;
	// 获取Scene进行渲染
	CPaintPanel* pView = (CPaintPanel*)(pWind);
	srand(static_cast<unsigned>(std::time(nullptr)));
	if (pView->s) pView->s->Rendered();
	else throw std::bad_alloc();
	ENDRENDER = true;
	return ::GetCurrentThreadId();
}


void CPaintPanel::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//MessageBox(TEXT("正在运行中是否关闭"));
	//while (!bDraw) {};
	//size_t time = 0ULL;
	//while (bRender) {
	//	++time;
	//};
	CView::OnClose();
}
