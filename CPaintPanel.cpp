// CPaintPanel.cpp: 实现文件
//

#include "pch.h"
#include "Test.h"
#include "CPaintPanel.h"
extern volatile bool ENDDRAW;
extern volatile bool ENDRENDER;
extern volatile bool ENDPROGRAM;

// CPaintPanel

IMPLEMENT_DYNCREATE(CPaintPanel, CView)

CPaintPanel::CPaintPanel()
{
	//s->Init();
	bRender = false;
	bDraw = true;
	//drawMutex = CreateMutex(NULL, FALSE, TEXT("Draw"));
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
	CPaintPanel* pView = (CPaintPanel*)(pWind);

	// 线程互斥锁
	std::unique_lock<std::mutex> lk(pView->drawMutex);
	pView->bDraw = false;
	pView->cv.wait(lk, [] {return ENDDRAW && (!ENDPROGRAM); });
	ENDDRAW = false;

	CRect rect;//客户区矩形
	pView->GetClientRect(&rect);
	CDC* pDC = pView->GetDC(); // 获取当前绘制板块

	pDC->SetMapMode(MM_ANISOTROPIC);//pDC自定义坐标系
	pDC->SetWindowExt(rect.Width(), rect.Height());//设置窗口范围
	pDC->SetViewportExt(rect.Width(), -rect.Height());//x轴水平向右，y轴垂直向上
	pDC->SetViewportOrg(rect.Width() / 2, rect.Height() / 2);//客户区中心为原点
	rect.OffsetRect(-rect.Width() / 2, -rect.Height() / 2);
	CDC memDC;//内存DC	
	memDC.CreateCompatibleDC(pDC);//建立与显示pDC兼容的memDC
	memDC.SetMapMode(MM_ANISOTROPIC);//memDC自定义坐标系
	memDC.SetWindowExt(rect.Width(), rect.Height());
	memDC.SetViewportExt(rect.Width(), -rect.Height());
	memDC.SetViewportOrg(rect.Width() / 2, rect.Height() / 2);
	CBitmap NewBitmap, * pOldBitmap;//内存中承载图像的临时位图 
#if 1
	NewBitmap.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());//创建兼容位图 
	pOldBitmap = memDC.SelectObject(&NewBitmap);//将兼容位图选入memDC
	//memDC.FillSolidRect(rect, pDC->GetBkColor());//设置背景色
	if (pView->s) pView->s->Draw(&memDC); // 绘制对象
	//if (pView->s) pView->s->Draw(pDC);
#else 
	// 减少SetPixel的调用，以减少变换绘制的不流畅
	Paint* paint = new Paint;
	paint->InitDeepBuffer(rect.Width(), rect.Height(), 1.0E1);

	// 执行绘制函数，获取点
	size_t totalSize = rect.Width() * rect.Height() * 4ULL; // 4为rgba
	std::unique_ptr<BYTE[]> buff = std::make_unique<BYTE[]>(totalSize);
	memset(buff.get(), 0, totalSize);
	if (pView->s) {
		pView->s->SetWindowsRect(rect.Width(), rect.Height()); // 设置绘制窗口的宽高
		pView->s->SetProject(project); // 视点
		pView->s->Draw(paint, buff, totalSize);  // 绘制，缓冲器
	}
	delete paint;

	// 执行赋值，将图片给memDC
	NewBitmap.CreateBitmap(rect.Width(), rect.Height(), sizeof(BYTE), 32, buff.get());
	pOldBitmap = memDC.SelectObject(&NewBitmap);
#endif
	pDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &memDC, -rect.Width() / 2, -rect.Height() / 2, SRCCOPY);//将内存memDC中的位图拷贝到显示pDC中
	memDC.SelectObject(pOldBitmap);//恢复位图
	NewBitmap.DeleteObject();//删除位图
	memDC.DeleteDC();//删除memDC

	lk.unlock();
	ENDDRAW = true;
	pView->cv.notify_one();
	pView->bDraw = true;
	return *(reinterpret_cast<DWORD*>(&std::this_thread::get_id()));
}

void CPaintPanel::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO:  在此添加绘制代码

	if (s && bRender) {
		std::thread renderThread(RenderThread, this);
		renderThread.detach();
		bRender = false;
	}

	if (s && bDraw) {
		std::thread bufferThread(DoubleBuffer, this);
		bufferThread.detach();
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
	//drawMutex.lock();
	std::unique_lock<std::mutex> lk(drawMutex);
	cv.wait(lk, [] {return ENDDRAW; });
	s->Init();
	lk.unlock();
	cv.notify_one();
	//drawMutex.unlock();
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
	//return ::GetCurrentThreadId();
	return *(reinterpret_cast<DWORD*>(&std::this_thread::get_id()));
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
