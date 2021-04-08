// CControlPanel.cpp: 实现文件
//

#include "pch.h"
#include "Test.h"
#include "CControlPanel.h"
#include "CAddObjectDlg.h"
#include "MainFrm.h"

extern volatile bool ENDDRAW;
extern volatile bool ENDPROGRAM;

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
ON_NOTIFY(NM_DBLCLK, IDC_ObjectList, &CControlPanel::OnNMDblclkObjectlist)
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
	ENDPROGRAM = true;
	while (!ENDDRAW) {} // 没有结束绘制则等待
#if 1
	// 回收面片中的对象（也可以直接移除对应的面片，但是留下的阴影太难看了）
	auto& patchs = scene.GetRadiationPatchs();
	for (size_t i = 0; i < patchs.size(); ++i)
		if (patchs[i].obj == delObj)
			patchs[i].obj = nullptr;
	delete delObj; delObj = nullptr;
#else
	// 若这样太耗时了，可以选择不删除对象，但是在该类中
	// 创建一个保存删除对象的vector 当结束程序时一并删除
#endif
	// 从objList中删除当前对象
	objList.erase(objList.begin() + location);
	// 更新list列表
	UpdateListCtrl();
	ENDPROGRAM = false;

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
	dlg.SetType(CAddObjectDlg::Operator::Add);
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


void CControlPanel::OnNMDblclkObjectlist(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	int m_Row = pNMListView->iItem;

	if (pNMListView->iItem == -1) {
		return;
	}

	if (!scene.bFinish) {
		MessageBox(TEXT("当前渲染还未结束，请耐心等该渲染完成后修改物体。"), TEXT("警告"));
		return;
	}
	//CString pos;
	//pos.Format(TEXT("选中的位置为： %d"), m_Row);
	//MessageBox(pos);

	CAddObjectDlg dlg;
	dlg.SetScene(&scene);
	dlg.SetType(CAddObjectDlg::Operator::Modify);
	dlg.SetObjectIndex(m_Row);
	dlg.DoModal();

	UpdateListCtrl();
	//UpdateData();

	*pResult = 0;
}
