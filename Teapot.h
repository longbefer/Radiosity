#pragma once
#include "BezierPatch.h"

class Teapot :
    public GameObject
{
public:
	Teapot(void) {
		ReadVertex();
		ReadPatch();
		this->name = "���";
	};
	virtual~Teapot(void) {};

	virtual bool IsHit(const Ray& ray, double& tmin) const { return bezier.IsHit(ray, tmin); }
	virtual std::vector<Patch> ObjectPatch() { 
		this->Init(); // �����Init����������
		return bezier.ObjectPatch(); 
	}
	virtual void SetTransformMatrix(const Matrix& matrix) {
		GameObject::SetTransformMatrix(matrix);
		//bezier.SetTransformMatrix(matrix);
		for (size_t i = 0; i < sizeof(Ver)/sizeof(Point3d); ++i) {
			Ver[i] = this->transformMatrix * Ver[i];
		}
	}

	virtual const BBox GetBoundingBox() {
		Point3d minPoint(1.0E10, 1.0E10, 1.0E10), maxPoint(-1.0E10, -1.0E10, -1.0E10);
		for (size_t i = 0; i < 306; ++i) {
			minPoint.x = std::min(Ver[i].x, minPoint.x);
			minPoint.y = std::min(Ver[i].y, minPoint.y);
			minPoint.z = std::min(Ver[i].z, minPoint.z);

			maxPoint.x = std::max(Ver[i].x, maxPoint.x);
			maxPoint.y = std::max(Ver[i].y, maxPoint.y);
			maxPoint.z = std::max(Ver[i].z, maxPoint.z);
		}
		this->box = BBox(minPoint - Point3d(1.0E-5, 1.0E-5, 1.0E-5),
						maxPoint + Point3d(1.0E-5, 1.0E-5, 1.0E-5));
		return this->box;
	}

private:
	void ReadVertex(void);//������ƶ���ζ���
	void ReadPatch(void);//����˫��������Ƭ

	void Init(void);//��������
	void DrawRim(void);//����
	void DrawBody(void);//����
	void DrawHandle(void);//����
	void DrawSpout(void);//����
	void DrawLid(void);//����
	void DrawBottom(void);//����
	void SigleCurvedPatch(void) {//����һ��˫��������Ƭ
		bezier.ReadControlPoint(Ver3);//16�����Ƶ�
	};
public:

	Point3d Ver[306];//����ܿ��Ƶ�
private:
	std::array<int, 16> Pat[32];//�������Ƭ
	Point3d Ver3[16];//��ά���Ƶ�
	BezierPatch bezier;//bezier����Ƭ
};

