#pragma once


// CAddObjectDlg 对话框
#include "Scene.h"
#include "Light.h"

class CAddObjectDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAddObjectDlg)

public:
	CAddObjectDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CAddObjectDlg();

	void SetScene(Scene*s) {
		this->s = s;
	}

	enum class Operator { Add, Modify };

	void SetType(Operator op) {
		this->op = op;
	}

	void SetObjectIndex(size_t index) {
		if (op != Operator::Modify) {
			MessageBox(TEXT("设置ObjectIndex将会自动转为修改模式，若非修改，请检查代码"));
			SetType(Operator::Modify);
		}
		this->modifyIndex = index;

	}

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_AddObjDlg };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	// 选中的物体
	CComboBox selectObj;
	double PosX;
	double PosY;
	double PosZ;
	double RotateVal;
	double ScaleVal;
	size_t colorR;
	size_t colorG;
	size_t colorB;

	Scene* s;
	size_t rescueTime;
	afx_msg void OnBnClickedConfirmadd();
	afx_msg void OnBnClickedCanceladd();
	CButton rotateX;
	CButton rotateY;
	CButton rotateZ;
	afx_msg void OnBnClickedCheckx();
	afx_msg void OnBnClickedChecky();
	afx_msg void OnBnClickedCheckz();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnCbnDropdownSelectobject();
	// 设置选中的对象是否为灯光
	BOOL bLight;
	afx_msg void OnBnClickedbtchoosetexture();
	// 纹理图片的路径
	CString fileName;
	// bLight失效，使用控件访问
	CButton LightCheckButton;
	virtual BOOL OnInitDialog();

	Operator op = Operator::Add;
	size_t modifyIndex = 0ULL;
};
