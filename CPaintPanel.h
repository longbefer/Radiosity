#pragma once


// CPaintPanel 视图
#include "Scene.h"
#ifdef USE_CAMERA
#include "Paint.h"
#include "Camera.h"
#endif

class CPaintPanel : public CView
{
	DECLARE_DYNCREATE(CPaintPanel)

protected:
	CPaintPanel();           // 动态创建所使用的受保护的构造函数
	virtual ~CPaintPanel();

public:
	friend DWORD WINAPI DoubleBuffer(void*);
	friend DWORD WINAPI RenderThread(void*);
	virtual void OnDraw(CDC* pDC);      // 重写以绘制该视图
	void SetScene(Scene*);
	void Init();
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
protected:
#ifdef USE_CAMERA
	Paint paint;
	Camera camera;
	bool bCatch = false;
#endif
public:
	Scene* s = nullptr;
	bool bDraw = true;
	std::mutex drawMutex;
	std::condition_variable cv;
	bool bRender = false;
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};


