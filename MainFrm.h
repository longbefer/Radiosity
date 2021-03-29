
// MainFrm.h: CMainFrame 类的接口
//

#pragma once
#include <afxcview.h>
// 自定义消息——开始渲染
#define WM_UpdatePaintPanel (WM_USER + 100)


class CMainFrame : public CFrameWnd
{
	
protected: // 仅从序列化创建
	CMainFrame() noexcept;
	DECLARE_DYNCREATE(CMainFrame)

// 特性
public:

// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 控件条嵌入成员
	CSplitterWnd      m_spliterWnd;         // 创建分隔栏
// 生成的消息映射函数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	afx_msg LRESULT OnUpdatePaintPanel(WPARAM wParam, LPARAM lParam);
public:
	//afx_msg void OnFileSave();
	afx_msg void OnClose();
	afx_msg void OnFileSaveUser();
};


