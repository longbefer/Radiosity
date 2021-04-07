#pragma once
#include "Grid.h"
/*
* Bezier�����࣬���ڲ���Bezier����
* ע�����������岻ͬ���ǣ������ȵ���ReadControlPoint����4*4����
* �����޷����Ƴ�Bezierͼ�Σ�ע������������SetTransform����ã�����任����Ч��
* ���Ե���Accelerate������ʹ��ʹ�ð�Χ�м���
*/
class BezierPatch :
    public Grid
{
public:
	BezierPatch(void) { this->name = "Bezier����"; };
	virtual~BezierPatch(void) {};
	void ReadControlPoint(Point3d* CtrPt);//����16�����Ƶ�

	virtual bool IsHit(const Ray& ray, double& tmin) const { 
		if (bAccelerate) return Grid::IsHit(ray, tmin);
		return Compound::IsHit(ray, tmin); 
	}
	virtual std::vector<Patch> ObjectPatch() { 
		if (bAccelerate) return Grid::ObjectPatch();
		return Compound::ObjectPatch(); // ��ObjectPatch����SetupCells()
	};

	void Accelerate() {
		bAccelerate = true;
	}

	virtual void SetTransformMatrix(const Matrix& matrix) {
		//Compound::SetTransformMatrix(matrix);
		GameObject::SetTransformMatrix(matrix);
	}

private:
	void SetCurvedPatch(void);//��������
	void Recursion(int ReNumber, Texture* mesh);//�ݹ麯��
	void Tessellation(Texture* Mesh, Point3d* P, Vector3d* N);//ϸ�ֺ���
	void LeftMultiplyMatrix(double M[4][4], Point3d P[4][4]);//��˶������
	void RightMultiplyMatrix(Point3d P[4][4], double M[4][4]);//�ҳ˶������
	void TransposeMatrix(double M[4][4]);//ת�þ���
private:
	Point3d CtrPt[16];  // �����16�����Ƶ�
	bool bAccelerate = false;
};

