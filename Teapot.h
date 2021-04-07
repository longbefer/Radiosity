#pragma once
#include "BezierPatch.h"

class Teapot :
    public GameObject
{
public:
	Teapot(void) {
		ReadVertex();
		ReadPatch();
		this->name = "茶壶";
	};
	virtual~Teapot(void) {};

	virtual bool IsHit(const Ray& ray, double& tmin) const { return bezier.IsHit(ray, tmin); }
	virtual std::vector<Patch> ObjectPatch() { 
		this->Init(); // 茶壶的Init必须放在最后
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
	void ReadVertex(void);//读入控制多边形顶点
	void ReadPatch(void);//读入双三次曲面片

	void Init(void);//绘制整壶
	void DrawRim(void);//壶边
	void DrawBody(void);//壶体
	void DrawHandle(void);//壶柄
	void DrawSpout(void);//壶嘴
	void DrawLid(void);//壶盖
	void DrawBottom(void);//壶底
	void SigleCurvedPatch(void) {//绘制一个双三次曲面片
		bezier.ReadControlPoint(Ver3);//16个控制点
	};
public:

	Point3d Ver[306];//茶壶总控制点
private:
	std::array<int, 16> Pat[32];//茶壶总面片
	Point3d Ver3[16];//三维控制点
	BezierPatch bezier;//bezier曲面片
};

