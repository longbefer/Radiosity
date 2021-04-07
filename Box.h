#pragma once
#include "GameObject.h"

class Box : public GameObject
{
public:
	Box() :Box(Point3d(-1.0, -1.0, -1.0), Point3d(1.0, 1.0, 1.0)) { this->name = "立方体"; };
	Box(const Point3d& p1, const Point3d& p2) {
		point[0] = p1;                        point[1] = Point3d(p2.x, p1.y, p1.z);
		point[2] = Point3d(p2.x, p1.y, p2.z); point[3] = Point3d(p1.x, p1.y, p2.z);
		point[4] = Point3d(p2.x, p2.y, p1.z); point[5] = Point3d(p1.x, p2.y, p1.z);
		point[6] = Point3d(p1.x, p2.y, p2.z); point[7] = p2;
		this->box = BBox(p1, p2);
		this->name = "立方体";
	};

	virtual bool IsHit(const Ray&, double&) const;
	virtual std::vector<Patch> ObjectPatch();
	virtual void SetTransformMatrix(const Matrix& matrix) {
		GameObject::SetTransformMatrix(matrix);
		// 对物体进行变换（由于击中的限制，不可以旋转点来实现这个,但是缩放就会有问题。。。）
		//for (size_t i = 0; i < sizeof(point) / sizeof(Point3d); ++i)
		//	point[i] = matrix * point[i]; // 这里只能应用缩放和移动矩阵
	}

	void SetMoveMatrix(const Matrix& matrix) {
		for (size_t i = 0; i < sizeof(point) / sizeof(Point3d); ++i)
			point[i] = matrix * point[i];
	}

	virtual const BBox GetBoundingBox() {
		Point3d minPoint(1.0E10, 1.0E10, 1.0E10), maxPoint(-1.0E10, -1.0E10, -1.0E10);
		for (size_t i = 0; i < sizeof(point)/sizeof(Point3d); ++i) {
			minPoint.x = std::min(point[i].x, minPoint.x);
			minPoint.y = std::min(point[i].y, minPoint.y);
			minPoint.z = std::min(point[i].z, minPoint.z);

			maxPoint.x = std::max(point[i].x, maxPoint.x);
			maxPoint.y = std::max(point[i].y, maxPoint.y);
			maxPoint.z = std::max(point[i].z, maxPoint.z);
		}
		this->box = BBox(minPoint - Point3d(1.0E-5, 1.0E-5, 1.0E-5),
			maxPoint + Point3d(1.0E-5, 1.0E-5, 1.0E-5));
		return this->box;
	}

protected:
	void DividenPatch(std::vector<Patch>&, const Point3d[4], const int&, const int&);
	Normal GetNormal(const int& face) const;
public:
	Point3d point[8];
};

