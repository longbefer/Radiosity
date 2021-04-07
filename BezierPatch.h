#pragma once
#include "Grid.h"
/*
* Bezier网格类，用于产生Bezier网格
* 注：与其他物体不同的是，必须先调用ReadControlPoint设置4*4个点
* 否则无法绘制出Bezier图形（注意必须设置完成SetTransform后调用，否则变换不生效）
* 可以调用Accelerate来启用使用使用包围盒加速
*/
class BezierPatch :
    public Grid
{
public:
	BezierPatch(void) { this->name = "Bezier曲面"; };
	virtual~BezierPatch(void) {};
	void ReadControlPoint(Point3d* CtrPt);//读入16个控制点

	virtual bool IsHit(const Ray& ray, double& tmin) const { 
		if (bAccelerate) return Grid::IsHit(ray, tmin);
		return Compound::IsHit(ray, tmin); 
	}
	virtual std::vector<Patch> ObjectPatch() { 
		if (bAccelerate) return Grid::ObjectPatch();
		return Compound::ObjectPatch(); // 该ObjectPatch不会SetupCells()
	};

	void Accelerate() {
		bAccelerate = true;
	}

	virtual void SetTransformMatrix(const Matrix& matrix) {
		//Compound::SetTransformMatrix(matrix);
		GameObject::SetTransformMatrix(matrix);
	}

private:
	void SetCurvedPatch(void);//绘制曲面
	void Recursion(int ReNumber, Texture* mesh);//递归函数
	void Tessellation(Texture* Mesh, Point3d* P, Vector3d* N);//细分函数
	void LeftMultiplyMatrix(double M[4][4], Point3d P[4][4]);//左乘顶点矩阵
	void RightMultiplyMatrix(Point3d P[4][4], double M[4][4]);//右乘顶点矩阵
	void TransposeMatrix(double M[4][4]);//转置矩阵
private:
	Point3d CtrPt[16];  // 曲面的16个控制点
	bool bAccelerate = false;
};

