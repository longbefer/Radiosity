﻿// CControlPanel.cpp: 实现文件
//

#include "pch.h"
#include "Test.h"
#include "CControlPanel.h"
#include "CAddObjectDlg.h"
#include "MainFrm.h"

// CControlPanel

IMPLEMENT_DYNCREATE(CControlPanel, CFormView)

CControlPanel::CControlPanel()
	: CFormView(IDD_Control)
{

}

CControlPanel::~CControlPanel()
{
}

void CControlPanel::UpdateListCtrl()
{
	lstObject.DeleteAllItems();
	size_t index = 0U;
	auto& objList = scene.GetObjects();
	for (auto& obj : objList) {
		// 对象名称
		CString strObjName(obj->GetName().c_str()/*typeid(obj).name()*/);
		// 颜色
		CString strObjColor;
		Color objColor = obj->GetReflectance();
		strObjColor.Format(TEXT("%d,%d,%d"), (int)(objColor.r * 255.0), (int)(objColor.g * 255.0), (int)(objColor.b * 255.0));
		// 位置
		CString strObjPosition;
		BBox objBox = obj->GetBoundingBox();
		Point3d objPoint = (objBox.point[0] + objBox.point[1]) / 2.0;
		strObjPosition.Format(TEXT("%f,%f,%f"), objPoint.x, objPoint.y, objPoint.z);

		lstObject.InsertItem((int)index, strObjName);
		lstObject.SetItemText((int)index, 1, strObjColor);
		lstObject.SetItemText((int)index, 2, strObjPosition);
		++index;
	}
}

void CControlPanel::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ObjectList, lstObject);
	DDX_Control(pDX, IDC_AddObject, btAddObject);
	DDX_Control(pDX, IDC_Confirm, btConfirm);

	CString str[] = { TEXT("物体名称"), TEXT("物体颜色(r,g,b)"), TEXT("位置(x,y,z)") };
	for (int i = 0; i < sizeof(str) / sizeof(CString); ++i)
		lstObject.InsertColumn(i, str[i], LVCFMT_LEFT, 100 * (i + 1));

	UpdateListCtrl();

	// 设置lst属性（整行选中和添加物网格线）
	lstObject.SetExtendedStyle(lstObject.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);


}

BEGIN_MESSAGE_MAP(CControlPanel, CFormView)
	ON_BN_CLICKED(IDC_DeleteObject, &CControlPanel::OnBnClickedDeleteobject)
	ON_BN_CLICKED(IDC_AddObject, &CControlPanel::OnBnClickedAddobject)
	ON_BN_CLICKED(IDC_Confirm, &CControlPanel::OnBnClickedConfirm)
//	ON_WM_SIZE()
END_MESSAGE_MAP()


// CControlPanel 诊断

#ifdef _DEBUG
void CControlPanel::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CControlPanel::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CControlPanel 消息处理程序


void CControlPanel::OnBnClickedDeleteobject()
{
	// TODO: 在此添加控件通知处理程序代码
	size_t n = lstObject.GetSelectedCount();     // 获取选中的物体数量
	if (n == 0) {
		MessageBox(TEXT("请选择物体后删除！"), TEXT("⚠警告"));
		return;
	}
	// 判断是否正在渲染，若渲染未完成不可删除物体
	if (!scene.bFinish) {
		MessageBox(TEXT("当前渲染还未结束，请耐心等该渲染完成后删除物体。"), TEXT("警告"));
		return;
	}
	// 删除选中的物体
	POSITION fstSelect = lstObject.GetFirstSelectedItemPosition();
	int location = static_cast<int>(reinterpret_cast<size_t>(fstSelect)) - 1;
	//lstObject.DeleteItem(location);

	auto& objList = scene.GetObjects();
	// 获取当前Object并删除它
	auto& delObj = objList[location];
	delete delObj; delObj = nullptr;
	// 从objList中删除当前对象
	objList.erase(objList.begin() + location, objList.begin() + location + 1);
	// 更新list列表
	UpdateListCtrl();

	//do { // 多选删除无效，不清楚原因
		//lstObject.DeleteItem((int)fstSelect - 1);
		//fstSelect = (POSITION)lstObject.GetNextSelectedItem(fstSelect);
	//} while (--n);
}


void CControlPanel::OnBnClickedAddobject()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!scene.bFinish) {
		MessageBox(TEXT("当前渲染还未结束，请耐心等该渲染完成后添加物体。"), TEXT("警告"));
		return;
	}
	CAddObjectDlg dlg;
	dlg.SetScene(&scene);
	dlg.DoModal();
	UpdateListCtrl();
}


void CControlPanel::OnBnClickedConfirm()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!scene.bFinish) {// scene还没有完成，不再进行渲染
		MessageBox(TEXT("当前渲染还未结束，请耐心等该渲染完成。"), TEXT("警告"));
		return;
	}
	::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_UpdatePaintPanel, (WPARAM)WM_UpdatePaintPanel, (LPARAM)(&scene));
}


//void CControlPanel::OnSize(UINT nType, int cx, int cy)
//{
//	CFormView::OnSize(nType, cx, cy);
//
//	// TODO: 在此处添加消息处理程序代码
//	//lstObject.Wid
//}