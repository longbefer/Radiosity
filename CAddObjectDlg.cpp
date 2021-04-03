// CAddObjectDlg.cpp: 实现文件
//

#include "pch.h"
#include "Test.h"
#include "CAddObjectDlg.h"
#include "afxdialogex.h"
#include "Box.h"
#include "Rectangle.h"
#include "Triangle.h"
#include "Light.h"
#include "Teapot.h"
#include "Sphere.h"
#include "ObjFile.h"

#include "SmoothRectangle.h"
#include "SmoothTriangle.h"

#include <atlconv.h> /* 编码转换 */
extern volatile bool ENDDRAW;

// CAddObjectDlg 对话框

IMPLEMENT_DYNAMIC(CAddObjectDlg, CDialogEx)

CAddObjectDlg::CAddObjectDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_AddObjDlg, pParent)
	, PosX(0)
	, PosY(0)
	, PosZ(0)
	, RotateVal(0)
	, ScaleVal(1)
	, colorR(0)
	, colorG(0)
	, colorB(0)
	, s(nullptr)
	, rescueTime(3)
{

}

CAddObjectDlg::~CAddObjectDlg()
{
}

void CAddObjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SelectObject, selectObj);
	DDX_Text(pDX, IDC_PosX, PosX);
	DDX_Text(pDX, IDC_PosY, PosY);
	DDX_Text(pDX, IDC_PosZ, PosZ);
	DDX_Text(pDX, IDC_RotateAngle, RotateVal);
	DDX_Text(pDX, IDC_ScaleValue, ScaleVal);
	DDX_Text(pDX, IDC_ColorR, colorR);
	DDX_Text(pDX, IDC_ColorG, colorG);
	DDX_Text(pDX, IDC_ColorB, colorB);
	DDX_Text(pDX, IDC_Rescuer, rescueTime);
	DDX_Control(pDX, IDC_CheckX, rotateX);
	DDX_Control(pDX, IDC_CheckY, rotateY);
	DDX_Control(pDX, IDC_CheckZ, rotateZ);

	selectObj.SetCurSel(2);
	rotateY.SetCheck(TRUE);
}


BEGIN_MESSAGE_MAP(CAddObjectDlg, CDialogEx)
	ON_BN_CLICKED(ID_ConfirmAdd, &CAddObjectDlg::OnBnClickedConfirmadd)
	ON_BN_CLICKED(ID_CancelAdd, &CAddObjectDlg::OnBnClickedCanceladd)
	ON_BN_CLICKED(IDC_CheckX, &CAddObjectDlg::OnBnClickedCheckx)
	ON_BN_CLICKED(IDC_CheckY, &CAddObjectDlg::OnBnClickedChecky)
	ON_BN_CLICKED(IDC_CheckZ, &CAddObjectDlg::OnBnClickedCheckz)
	ON_WM_DROPFILES()
	ON_CBN_DROPDOWN(IDC_SelectObject, &CAddObjectDlg::OnCbnDropdownSelectobject)
END_MESSAGE_MAP()


// CAddObjectDlg 消息处理程序


void CAddObjectDlg::OnBnClickedConfirmadd()
{
	// TODO: 在此添加控件通知处理程序代码
	// Bezier的代码块
	double r = 300.0;
	double m = 0.5523;
	Point3d P3[16];
	P3[0].x = -250.; P3[0].y = -80.; P3[0].z = 200.;//P00
	P3[1].x = -50.; P3[1].y = 100.; P3[1].z = 150.;//P01
	P3[2].x = 50.; P3[2].y = 100.; P3[2].z = 150.;//P02
	P3[3].x = 250.; P3[3].y = -80.; P3[3].z = 200.;//P03

	P3[4].x = -150.; P3[4].y = 150.; P3[4].z = 50.;//P10
	P3[5].x = -50.; P3[5].y = 150.; P3[5].z = 50.;//p11
	P3[6].x = 50.; P3[6].y = 150.; P3[6].z = 50.;//p12
	P3[7].x = 150.; P3[7].y = 150.; P3[7].z = 50.;//p13

	P3[8].x = -150.; P3[8].y = 150.; P3[8].z = -50.;//P20
	P3[9].x = -50.; P3[9].y = 150.; P3[9].z = -50.;//P21
	P3[10].x = 50.; P3[10].y = 150.; P3[10].z = -50.;//P22
	P3[11].x = 150.; P3[11].y = 150.; P3[11].z = -50.;//P23

	P3[12].x = -350.; P3[12].y = -120.; P3[12].z = -200.;//P30
	P3[13].x = -50.; P3[13].y = 100.; P3[13].z = -150.;//P31
	P3[14].x = 50.; P3[14].y = 100.; P3[14].z = -150.;//P32
	P3[15].x = 250.; P3[15].y = -100.; P3[15].z = -200.;//P33	

	// 获取选中的对象
	CString objName;
	selectObj.GetWindowTextW(objName);
	UpdateData(TRUE);
	Matrix::AXIS choosed = Matrix::AXIS::Y;
	if (rotateX.GetCheck()) choosed = Matrix::AXIS::X;
	if (rotateZ.GetCheck()) choosed = Matrix::AXIS::Z;
	// 矩形; 茶壶; 三角形; 球; 立方体; Bezier曲面;
	GameObject* obj = nullptr;
	// 设置值
	if (objName == TEXT("矩形"))
		obj = new User::Rectangle(Point3d(-200, -200, -200), Vector3d(400, 0, 0), Vector3d(0, 0, 400), YNormal);
	else if (objName == TEXT("茶壶")) {
		obj = new Teapot();
		//ScaleVal = 50;
	}
	else if (objName == TEXT("三角形")) {
		obj = new Triangle(Point3d(-std::sqrt(3.0), -1.0, .5) * 100.0, Point3d(std::sqrt(3.0), -1.0, .5) * 100.0, Point3d(.0, -1.0, -1.0) * 100.0);
		((Triangle*)(obj))->SetNormal(YNormal, YNormal, YNormal);
	}
	else if (objName == TEXT("球"))
		obj = new Sphere(ZERO, 100);
	else if (objName == TEXT("立方体")) {
		// 对于立方体缩放和旋转不能放在一个矩阵的bug，可以给box添加一个用来设置旋转的函数
		obj = new Box(Point3d(-100., -100., -100.), Point3d(100., 100., 100.));
		ScaleVal = 1.0; // 不响应缩放
	}
	else if (objName == TEXT("Bezier曲面")) {
		obj = new BezierPatch();
	}
	else if (ObjFile::IsLoaded((LPCSTR)(CStringA)objName)){
		obj = new ObjFile((LPCSTR)(CStringA)objName);
	}

	if (obj == nullptr) {
		MessageBox(TEXT("选择错误ERROR，添加物体失败"), TEXT("警告"));
		return;
	}
	obj->SetColor(Color((double)colorR / 255.0, (double)colorG / 255.0, (double)colorB / 255.0).Clamp());
	obj->SetEmmision(Black);
	obj->SetRecursionTimes(static_cast<int>(rescueTime));
	Matrix transform = transform.SetScale(ScaleVal, ScaleVal, ScaleVal).Translate(PosX, PosY, PosZ).Rotate(RotateVal, choosed);
	obj->SetTransformMatrix(transform);
	if (objName == TEXT("Bezier曲面")) {
		auto bezierObj = dynamic_cast<BezierPatch*>(obj);
		bezierObj->ReadControlPoint(P3);
		bezierObj->Accelerate();
	}
	if (dynamic_cast<ObjFile*>(obj)) dynamic_cast<ObjFile*>(obj)->AddObject();

	while (!ENDDRAW) {} // 没有结束绘制则等待
	// 在场景中加入添加的文件
	s->AddObject(obj);

	OnOK();
}


void CAddObjectDlg::OnBnClickedCanceladd()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}

// 编组无效，只好这样了。。。
void CAddObjectDlg::OnBnClickedCheckx()
{
	// TODO: 在此添加控件通知处理程序代码
	if (rotateY.GetCheck()) rotateY.SetCheck(0);
	if (rotateZ.GetCheck()) rotateZ.SetCheck(0);
	rotateX.SetCheck(1);
}


void CAddObjectDlg::OnBnClickedChecky()
{
	// TODO: 在此添加控件通知处理程序代码
	if (rotateX.GetCheck()) rotateX.SetCheck(0);
	if (rotateZ.GetCheck()) rotateZ.SetCheck(0);
	rotateY.SetCheck(1);
}


void CAddObjectDlg::OnBnClickedCheckz()
{
	// TODO: 在此添加控件通知处理程序代码
	if (rotateY.GetCheck()) rotateY.SetCheck(0);
	if (rotateX.GetCheck()) rotateX.SetCheck(0);
	rotateZ.SetCheck(1);
}


void CAddObjectDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//UINT nFileCount = ::DragQueryFile(hDropInfo, (UINT)-1, NULL, 0);
	//TCHAR szFileName[1024] = TEXT("");
	//::DragQueryFile(hDropInfo, 0, szFileName, sizeof(szFileName));
	//DWORD dwAttribute = ::GetFileAttributes(szFileName);
	//MessageBox(szFileName);
	CDialogEx::OnDropFiles(hDropInfo);
}


void CAddObjectDlg::OnCbnDropdownSelectobject()
{
	// TODO: 在此添加控件通知处理程序代码
	//MessageBox(TEXT("插入文件？"));
}
