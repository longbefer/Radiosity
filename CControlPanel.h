#pragma once



// CControlPanel 窗体视图
#include "Scene.h"

class CControlPanel : public CFormView
{
	DECLARE_DYNCREATE(CControlPanel)

protected:
	CControlPanel();           // 动态创建所使用的受保护的构造函数
	virtual ~CControlPanel();
	void UpdateListCtrl();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_Control };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	// 显示添加到绘制场景中的物体
	CListCtrl lstObject;
	// 点击按钮添加物体
	CButton btAddObject;
	// 点击按钮确认渲染
	CButton btConfirm;
	// 场景类
	Scene scene;
	afx_msg void OnBnClickedDeleteobject();
	afx_msg void OnBnClickedAddobject();
	afx_msg void OnBnClickedConfirm();
//	afx_msg void OnSize(UINT nType, int cx, int cy);
};


