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
extern volatile bool ENDPROGRAM;
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
	, bLight(FALSE)
	, fileName(_T(""))
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
	DDX_Control(pDX, IDC_CheckLight, LightCheckButton);
	//DDX_Radio(pDX, IDC_CheckLight, bLight);
	DDX_Text(pDX, IDC_FilePath, fileName);

	//selectObj.SetCurSel(2);
	//rotateY.SetCheck(TRUE);
}


BEGIN_MESSAGE_MAP(CAddObjectDlg, CDialogEx)
	ON_BN_CLICKED(ID_ConfirmAdd, &CAddObjectDlg::OnBnClickedConfirmadd)
	ON_BN_CLICKED(ID_CancelAdd, &CAddObjectDlg::OnBnClickedCanceladd)
	ON_BN_CLICKED(IDC_CheckX, &CAddObjectDlg::OnBnClickedCheckx)
	ON_BN_CLICKED(IDC_CheckY, &CAddObjectDlg::OnBnClickedChecky)
	ON_BN_CLICKED(IDC_CheckZ, &CAddObjectDlg::OnBnClickedCheckz)
	ON_WM_DROPFILES()
	ON_CBN_DROPDOWN(IDC_SelectObject, &CAddObjectDlg::OnCbnDropdownSelectobject)
	ON_BN_CLICKED(IDC_btChooseTexture, &CAddObjectDlg::OnBnClickedbtchoosetexture)
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
	bLight = LightCheckButton.GetCheck();
	// 矩形; 茶壶; 三角形; 球; 立方体; Bezier曲面;
	GameObject* obj = nullptr;
	if (op == Operator::Add) {  // 若为添加操作
		// 设置值
		if (objName == TEXT("矩形")) {
			if (!bLight) {
				obj = new SmoothRectangle(Point3d(-200.0, -200.0, -200.0), Vector3d(400., .0, .0, .0), Vector3d(.0, .0, 400., .0), YNormal);
				if (fileName != "") {
					if (!obj->SetTextureImage((LPCSTR)(CStringA)fileName)) {
						MessageBox(TEXT("读取图片失败，图片可能非bmp或jpg格式"));
						return;
					}
					dynamic_cast<SmoothRectangle*>(obj)->SetTexture();
				}
			}
			else {
				obj = new LightShape<SmoothRectangle>(Point3d(-200, -200, -200), Vector3d(400, 0, 0, .0), Vector3d(0, 0, 400.0, .0), YNormal);
			}
		}
		else if (objName == TEXT("茶壶")) {
			if (bLight) obj = new LightShape<Teapot>();
			else obj = new Teapot();
			//ScaleVal = 50;
		}
		else if (objName == TEXT("三角形")) {
			if (bLight) {
				obj = new LightShape<Triangle>(Point3d(-std::sqrt(3.0), -1.0, .5), Point3d(std::sqrt(3.0), -1.0, .5), Point3d(.0, -1.0, -1.0));
				dynamic_cast<LightShape<Triangle>*>(obj)->SetNormal(YNormal, YNormal, YNormal);
			}
			else {
				obj = new Triangle(Point3d(-std::sqrt(3.0), -1.0, .5), Point3d(std::sqrt(3.0), -1.0, .5), Point3d(.0, -1.0, -1.0));
				((Triangle*)(obj))->SetNormal(YNormal, YNormal, YNormal);
			}
		}
		else if (objName == TEXT("球")) {
			if (bLight) obj = new LightShape<Sphere>(ZERO, 100);
			else obj = new Sphere(ZERO, 100);
		}
		else if (objName == TEXT("立方体")) {
			// 对于立方体缩放和旋转不能放在一个矩阵的bug，可以给box添加一个用来设置旋转的函数
			if (bLight) obj = new Light(Point3d(-100., -100., -100.), Point3d(100., 100., 100.));
			else  obj = new Box(Point3d(-100., -100., -100.), Point3d(100., 100., 100.));

			Matrix moveMatrix = moveMatrix.SetTranslate(PosX, PosY, PosZ);
			if (bLight) dynamic_cast<Light*>(obj)->SetMoveMatrix(moveMatrix);
			else dynamic_cast<Box*>(obj)->SetMoveMatrix(moveMatrix);

			ScaleVal = 1.0; // 不响应缩放
			PosX = PosY = PosZ = 0.0; // 以下不设置位移
		}
		else if (objName == TEXT("Bezier曲面")) {
			if (bLight) obj = new LightShape<BezierPatch>();
			else obj = new BezierPatch();
		}
		else if (ObjFile::IsLoaded((LPCSTR)(CStringA)objName)) {
			if (bLight) obj = new LightShape<ObjFile>((LPCSTR)(CStringA)objName);
			else obj = new ObjFile((LPCSTR)(CStringA)objName);
		}

		if (obj == nullptr) {
			MessageBox(TEXT("选择错误ERROR，添加物体失败"), TEXT("警告"));
			return;
		}
		obj->SetColor(Color((double)colorR / 255.0, (double)colorG / 255.0, (double)colorB / 255.0).Clamp());
		if (bLight) obj->SetEmmision(White);
		else obj->SetEmmision(Black);
		obj->SetRecursionTimes(static_cast<int>(rescueTime));
		Matrix transform = transform.SetScale(ScaleVal, ScaleVal, ScaleVal).Translate(PosX, PosY, PosZ).Rotate(RotateVal, choosed);
		obj->SetTransformMatrix(transform);

		// 部分物体的后续处理
		if (objName == TEXT("Bezier曲面")) {
			auto bezierObj = bLight ? dynamic_cast<LightShape<BezierPatch>*>(obj) : dynamic_cast<BezierPatch*>(obj);
			bezierObj->ReadControlPoint(P3);
			bezierObj->Accelerate();
		}
		auto objFile = bLight ? dynamic_cast<LightShape<ObjFile>*>(obj) : dynamic_cast<ObjFile*>(obj);
		if (objFile) objFile->AddObject();

		ENDPROGRAM = true;
		while (!ENDDRAW) {} // 没有结束绘制则等待
		// 在场景中加入添加的文件
		s->AddObject(obj);
		ENDPROGRAM = false;
	} 
	else if (op == Operator::Modify) { // 为修改操作

		obj = this->s->GetObjects()[modifyIndex]; // 获取需要修改的对象

		ENDPROGRAM = true; // 暂时没想到好办法暂停绘制线程，只好假装结束程序来使得绘制线程停止了
		while (!ENDDRAW) {};
		// 特殊处理的物体
		if (obj->GetName().find("矩形") != std::string::npos) {// 判断是否为矩形
			if (fileName != "") {
				if (!obj->SetTextureImage((LPCSTR)(CStringA)fileName)) {
					MessageBox(TEXT("读取图片失败，图片可能非bmp或jpg格式"));
					return;
				}
				dynamic_cast<SmoothRectangle*>(obj)->SetTexture();
			}
		} else if (obj->GetName().find("立方体") != std::string::npos) { // 是否为立方体
			Matrix moveMatrix = moveMatrix.SetTranslate(PosX, PosY, PosZ);
			if (bLight) dynamic_cast<Light*>(obj)->SetMoveMatrix(moveMatrix);
			else dynamic_cast<Box*>(obj)->SetMoveMatrix(moveMatrix);

			ScaleVal = 1.0; // 不响应缩放
			PosX = PosY = PosZ = 0.0; // 以下不设置位移
		}

		obj->SetColor(Color((double)colorR / 255.0, (double)colorG / 255.0, (double)colorB / 255.0).Clamp());
		if (bLight) obj->SetEmmision(White);
		else obj->SetEmmision(Black);
		obj->SetRecursionTimes(static_cast<int>(rescueTime));
		Matrix transform = transform.SetScale(ScaleVal, ScaleVal, ScaleVal).Translate(PosX, PosY, PosZ).Rotate(RotateVal, choosed);
		obj->SetTransformMatrix(transform);

		// 部分物体的后续处理
		if (obj->GetName().find("Bezier曲面") != std::string::npos) { // Beizer也不想改，因为要保存点，难。。。
			auto bezierObj = bLight ? dynamic_cast<LightShape<BezierPatch>*>(obj) : dynamic_cast<BezierPatch*>(obj);
			bezierObj->DeleteObjects();
			bezierObj->ReadControlPoint(P3);
			bezierObj->Accelerate();
		}
		// ObjFile无法修改（这个不弄了，麻烦。。。）
		//auto objFile = bLight ? dynamic_cast<LightShape<ObjFile>*>(obj) : dynamic_cast<ObjFile*>(obj);
		//if (objFile) objFile->AddObject();
		ENDPROGRAM = false;

	}
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
	UpdateData(TRUE);
	if (rotateY.GetCheck()) rotateY.SetCheck(0);
	if (rotateZ.GetCheck()) rotateZ.SetCheck(0);
	rotateX.SetCheck(1);
	UpdateData(FALSE);
}


void CAddObjectDlg::OnBnClickedChecky()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	if (rotateX.GetCheck()) rotateX.SetCheck(0);
	if (rotateZ.GetCheck()) rotateZ.SetCheck(0);
	rotateY.SetCheck(1);
	UpdateData(FALSE);
}


void CAddObjectDlg::OnBnClickedCheckz()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	if (rotateY.GetCheck()) rotateY.SetCheck(0);
	if (rotateX.GetCheck()) rotateX.SetCheck(0);
	rotateZ.SetCheck(1);
	UpdateData(FALSE);
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


void CAddObjectDlg::OnBnClickedbtchoosetexture()
{
	// TODO: 在此添加控件通知处理程序代码
	CString objName;
	selectObj.GetWindowTextW(objName);

	if (objName != TEXT("矩形")) {
		MessageBox(TEXT("抱歉，暂时只为支持矩形添加纹理"), TEXT("抱歉"));
		return;
	}
	UpdateData(TRUE); // 先获取控件信息

	CFileDialog dialog(TRUE, TEXT(" Bitmap File(*.bmp) |*.bmp|| "), NULL, NULL, TEXT(" Bitmap File(*.bmp) |*.bmp| JPEG File(*.jpg,*.jpeg) |*.jpg|| "), this);
	fileName = "";
	if (dialog.DoModal() == IDOK) {
		fileName = dialog.GetPathName();
	}
	UpdateData(FALSE); // 再更新控件
}

bool IsLight(GameObject* obj) {
	if (dynamic_cast<Light*>(obj)) return true;
	else if (dynamic_cast<LightShape<Box>*>(obj)) return true;
	else if (dynamic_cast<LightShape<BezierPatch>*>(obj)) return true;
	else if (dynamic_cast<LightShape<Compound>*>(obj)) return true;
	else if (dynamic_cast<LightShape<Grid>*>(obj)) return true;
	else if (dynamic_cast<LightShape<ObjFile>*>(obj)) return true;
	else if (dynamic_cast<LightShape<User::Rectangle>*>(obj)) return true;
	else if (dynamic_cast<LightShape<SmoothRectangle>*>(obj)) return true;
	else if (dynamic_cast<LightShape<SmoothTriangle>*>(obj)) return true;
	else if (dynamic_cast<LightShape<Sphere>*>(obj)) return true;
	else if (dynamic_cast<LightShape<Teapot>*>(obj)) return true;
	else if (dynamic_cast<LightShape<Triangle>*>(obj)) return true;
	return false;
}

BOOL CAddObjectDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	// 修改控件内容
	if (this->s == nullptr) {
		MessageBox(TEXT("请先传递Scene，请检查代码"));
		return false;
	}
	if (op == Operator::Add) {
		selectObj.SetCurSel(2);
		rotateY.SetCheck(TRUE);
		UpdateData(FALSE);
	} else if (op == Operator::Modify) {
		// 设置窗体
		this->SetWindowTextW(TEXT("修改物体")); // 修改标题栏

		auto modifyObj = this->s->GetObjects()[modifyIndex];
		if (!modifyObj) {
			MessageBox(TEXT("index无效，请检查代码"));
			return FALSE;
		}
		UpdateData(TRUE);
		// 选择是否是灯光
		if (IsLight(modifyObj))
			LightCheckButton.SetCheck(TRUE);
		else LightCheckButton.SetCheck(FALSE);
		LightCheckButton.EnableWindow(false);
		// 设置递归选择
		rescueTime = modifyObj->GetRecursionTimes();
		// 去除下拉框的选择
		CString objName(modifyObj->GetName().c_str());
		selectObj.InsertString(0, objName);
		selectObj.SetCurSel(0);
		selectObj.EnableWindow(false);
		// 设置颜色
		colorR = static_cast<size_t>(modifyObj->GetReflectance().r * 255ULL);
		colorG = static_cast<size_t>(modifyObj->GetReflectance().g * 255ULL);
		colorB = static_cast<size_t>(modifyObj->GetReflectance().b * 255ULL);
		// 设置纹理
		//fileName = modifyObj->GetImage() ? "修改纹理" : "添加纹理";
		// 设置默认选项
		rotateY.SetCheck(TRUE);
		UpdateData(FALSE);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


#if 0
//selectObj.Clear();
//int objIndex = selectObj.GetCount();
//while (objIndex--) selectObj.DeleteString(0);
		// 去掉CComboBox的下拉选择
		//DWORD theStyle;
		//HWND theChild;
		//theChild = ::GetWindow(GetDlgItem(IDC_SelectObject)->m_hWnd, GW_CHILD);
		//theStyle = GetWindowLong(GetDlgItem(IDC_SelectObject)->m_hWnd, GWL_STYLE);
		//theStyle &= ~CBS_DROPDOWN;//去掉DROPDOWN  
		//theStyle |= CBS_DROPDOWNLIST; //添加DROPDOWNLIST 
		//::DestroyWindow(theChild);
		//SetWindowLong(GetDlgItem(IDC_SelectObject)->m_hWnd, GWL_STYLE, theStyle);
#endif