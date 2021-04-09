#pragma once
#include "GameObject.h"

class Triangle
	:public GameObject
{
public:
	Triangle() {
		for (size_t i = 0; i < 3; ++i) {
			vertices[i] = ZERO;
			normal[i] = Normal(0.0, 0.0, 0.0, 0.0);
		}
		this->name = "三角形";
	};
	Triangle(const Point3d& a, const Point3d& b, const Point3d& c) {
		this->vertices[0] = a;
		this->vertices[1] = b;
		this->vertices[2] = c;
		this->box = BBox(Point3d(std::min({ vertices[0].x, vertices[1].x, vertices[2].x }) - kEpsilon,
								 std::min({ vertices[0].y, vertices[1].y, vertices[2].y }) - kEpsilon,
								 std::min({ vertices[0].z, vertices[1].z, vertices[2].z }) - kEpsilon ), 
						 Point3d(std::max({ vertices[0].x, vertices[1].x, vertices[2].x }) + kEpsilon,
								 std::max({ vertices[0].y, vertices[1].y, vertices[2].y }) + kEpsilon,
								 std::max({ vertices[0].z, vertices[1].z, vertices[2].z }) + kEpsilon));
		Vector3d vector01 = vertices[1] - vertices[0];
		Vector3d vector02 = vertices[2] - vertices[0];
		Normal fNormal = Cross(vector01, vector02).Normalized();
		for (size_t i = 0; i < 3; ++i)
			this->normal[i] = fNormal;
		this->name = "三角形";
	}
	Triangle(const Point3d& a, const Point3d& b, const Point3d& c, const Normal& n1, const Normal& n2, const Normal& n3) {
		Triangle(a, b, c);
		normal[0] = n1;
		normal[1] = n2;
		normal[2] = n3;
	}
public:
	virtual bool IsHit(const Ray&, double&) const;
	virtual std::vector<Patch> ObjectPatch();

	virtual void SetTransformMatrix(const Matrix& matrix) {
		GameObject::SetTransformMatrix(matrix);
		for (size_t i = 0; i < sizeof(vertices) / sizeof(Point3d); ++i) {
			this->vertices[i] = transformMatrix * this->vertices[i];
			this->normal[i] = (transformMatrix * this->normal[i]).Normalized();
		}
	}

	virtual const BBox GetBoundingBox() {
		this->box = BBox(Point3d(std::min({ vertices[0].x, vertices[1].x, vertices[2].x }) - kEpsilon,
			std::min({ vertices[0].y, vertices[1].y, vertices[2].y }) - kEpsilon,
			std::min({ vertices[0].z, vertices[1].z, vertices[2].z }) - kEpsilon),
			Point3d(std::max({ vertices[0].x, vertices[1].x, vertices[2].x }) + kEpsilon,
				std::max({ vertices[0].y, vertices[1].y, vertices[2].y }) + kEpsilon,
				std::max({ vertices[0].z, vertices[1].z, vertices[2].z }) + kEpsilon));
		return this->box;
	}

	void SetNormal(const Normal& v0, const Normal& v1, const Normal& v2) {
		normal[0] = v0;
		normal[1] = v1;
		normal[2] = v2;
	}
protected:
	void DividenPatch(std::vector<Patch>&, Point3d[3], const int&);
	Normal InterpolateNormal(const double& beta, const double& gamma);

protected:
	Point3d vertices[3]; // 注意：可能需要拷贝构造函数/赋值构造函数
	Normal normal[3];
};

