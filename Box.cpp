#include "pch.h"
#include "Box.h"
#include "Patch.h"

bool Box::IsHit(const Ray& ray, double& tmin) const
{
	Ray transformRay; // box的旋转和（缩放、平移）不能同时，由于缩放必须改变point但旋转改变point会导致hit的计算错误
	transformRay.o = this->transformMatrix * ray.o;
	transformRay.d = this->transformMatrix * ray.d; // 这里只能应用旋转
	double ox = transformRay.o.x; double oy = transformRay.o.y; double oz = transformRay.o.z;
	double dx = transformRay.d.x; double dy = transformRay.d.y; double dz = transformRay.d.z;

	double tx_min, ty_min, tz_min;
	double tx_max, ty_max, tz_max;

	double a = 1.0 / dx;
	tx_min = (point[0].x - ox) * a;
	tx_max = (point[7].x - ox) * a;
	if (a < 0) std::swap(tx_min, tx_max);

	double b = 1.0 / dy;
	ty_min = (point[0].y - oy) * b;
	ty_max = (point[7].y - oy) * b;
	if (b < 0) std::swap(ty_min, ty_max);

	double c = 1.0 / dz;
	tz_min = (point[0].z - oz) * c;
	tz_max = (point[7].z - oz) * c;
	if (c < 0) std::swap(tz_min, tz_max);

	double t0, t1;
	//找到最大的t值
	t0 = std::max({ tx_min, ty_min, tz_min });
	//找到最小的t值
	t1 = std::min({ tx_max, ty_max, tz_max });

	if ((t0 <= t1 && t1 > kEpsilon) ||
		// 但当两个值解近时，可以特殊处理
		(t1 > kEpsilon && std::abs(t0 - t1) < kEpsilon)) {
		if (t0 > kEpsilon) tmin = t0;
		else tmin = t1;
		return true;
	}
	return false;
}

std::vector<Patch> Box::ObjectPatch()
{
	std::vector<Patch> patchs;

	auto& x0 = point[0].x; auto& y0 = point[0].y;
	auto& z0 = point[0].z; auto& x1 = point[7].x;
	auto& y1 = point[7].y; auto& z1 = point[7].z;

	Point3d facet[6][4];
	// 前
	facet[0][0] = Point3d(x1, y0, z1, 1.0);
	facet[0][1] = Point3d(x1, y1, z1, 1.0);
	facet[0][2] = Point3d(x0, y1, z1, 1.0);
	facet[0][3] = Point3d(x0, y0, z1, 1.0);
	DividenPatch(patchs, facet[0], divide, 5);
	// 后
	facet[1][0] = Point3d(x0, y1, z0, 1.0);
	facet[1][1] = Point3d(x1, y1, z0, 1.0);
	facet[1][2] = Point3d(x1, y0, z0, 1.0);
	facet[1][3] = Point3d(x0, y0, z0, 1.0);
	DividenPatch(patchs, facet[1], divide, 2);
	// 左
	facet[2][0] = Point3d(x0, y1, z1, 1.0);
	facet[2][1] = Point3d(x0, y1, z0, 1.0);
	facet[2][2] = Point3d(x0, y0, z0, 1.0);
	facet[2][3] = Point3d(x0, y0, z1, 1.0);
	DividenPatch(patchs, facet[2], divide, 0);
	// 右
	facet[3][0] = Point3d(x1, y1, z0, 1.0);
	facet[3][1] = Point3d(x1, y1, z1, 1.0);
	facet[3][2] = Point3d(x1, y0, z1, 1.0);
	facet[3][3] = Point3d(x1, y0, z0, 1.0);
	DividenPatch(patchs, facet[3], divide, 3);
	// 上
	facet[4][0] = Point3d(x0, y1, z1, 1.0);
	facet[4][1] = Point3d(x1, y1, z1, 1.0);
	facet[4][2] = Point3d(x1, y1, z0, 1.0);
	facet[4][3] = Point3d(x0, y1, z0, 1.0);
	DividenPatch(patchs, facet[4], divide, 4);
	// 下
	facet[5][0] = Point3d(x0, y0, z1, 1.0);
	facet[5][1] = Point3d(x0, y0, z0, 1.0);
	facet[5][2] = Point3d(x1, y0, z0, 1.0);
	facet[5][3] = Point3d(x1, y0, z1, 1.0);
	DividenPatch(patchs, facet[5], divide, 1);

	return patchs;
}

void Box::DividenPatch(std::vector<Patch>& patchs, const Point3d point[4], const int& n, const int& face)
{
	if (n <= 0) {
		Patch patch;
		patch.normal = GetNormal(face);
		patch.obj = this;
		for (int i = 0; i < 4; ++i) {
			P3 p;
			p.position = transformMatrix * point[i];
			p.normal = GetNormal(face);
			//p.texture;
			patch.vertices.push_back(p);
		}
		patchs.push_back(patch);
		return;
	}
	Point3d p[4][4];
	p[0][0] = point[0];
	p[0][1] = (point[0] + point[1]) / 2.0;
	p[0][2] = (point[0] + point[2]) / 2.0;
	p[0][3] = (point[0] + point[3]) / 2.0;
	DividenPatch(patchs, p[0], n - 1, face);

	p[1][0] = (point[0] + point[1]) / 2.0;
	p[1][1] = point[1];
	p[1][2] = (point[1] + point[2]) / 2.0;
	p[1][3] = (point[0] + point[2]) / 2.0;
	DividenPatch(patchs, p[1], n - 1, face);

	p[2][0] = (point[0] + point[2]) / 2.0;
	p[2][1] = (point[1] + point[2]) / 2.0;
	p[2][2] = point[2];
	p[2][3] = (point[2] + point[3]) / 2.0;
	DividenPatch(patchs, p[2], n - 1, face);

	p[3][0] = (point[0] + point[3]) / 2.0;
	p[3][1] = (point[0] + point[2]) / 2.0;
	p[3][2] = (point[2] + point[3]) / 2.0;
	p[3][3] = point[3];
	DividenPatch(patchs, p[3], n - 1, face);
}

Normal Box::GetNormal(const int& face) const
{
	switch (face)
	{
	case 0:return(transformMatrix * Normal3d(-1.0, 0.0, 0.0, 0.0));//-x face
	case 1:return(transformMatrix * Normal3d(0.0, -1.0, 0.0, 0.0));//-y face
	case 2:return(transformMatrix * Normal3d(0.0, 0.0, -1.0, 0.0));//-z face
	case 3:return(transformMatrix * Normal3d(1.0, 0.0, 0.0, 0.0));//+x face
	case 4:return(transformMatrix * Normal3d(0.0, 1.0, 0.0, 0.0));//+y face
	case 5:return(transformMatrix * Normal3d(0.0, 0.0, 1.0, 0.0));//+z face
	default: return(Normal(0.0, 0.0, 0.0, 0.0)); // error
	}
}
