#include "pch.h"
#include "Sphere.h"
#include "Patch.h"

bool Sphere::IsHit(const Ray& ray, double& tmin) const
{
	// Solve t^2*d.d + 2*t*(o-p).d + (o-p).(o-p)-R^2 = 0
	Vector3d ray_to_center = center - ray.o;//光线指向球心
	double b = Dot(ray_to_center, ray.d);//ax^2+bx+c=0
	double det = (b * b - Dot(ray_to_center, ray_to_center) + r * r);
	if (det < 0.0) return false;//光线前没有物体

	double dets = std::sqrt(det);
	if (b + dets < -kEpsilon) return false;
	tmin = (b - dets > kEpsilon) ? (b - dets) : (b + dets);
	return true;
}

std::vector<Patch> Sphere::ObjectPatch()
{
	N1 = divide;
	N2 = 2 * divide;
	P = std::make_unique<Point3d[]>(static_cast<size_t>(((size_t)N1 - 1U) * (size_t)N2 + 2U));
	ReadVertex();
    std::vector<Patch> patchs;
#if 1
	for (int j = 0; j < N2; j++)//构造南北极三角形网格
	{
		Patch patch;
		patch.obj = this;
		int tempj = j + 1;
		if (N2 == tempj) tempj = 0;//网格的首尾连接
		int NorthIndex[3]{ 0, j + 1, tempj + 1 };//北极三角形网格索引号数组
		for (int k = 0; k < 3; k++) {
			P3 point3;
			point3.position = P[NorthIndex[k]];
			point3.normal = (point3.position - center).Normalized();
			//point3.texture;
			patch.vertices.push_back(point3);
		}
		patchs.push_back(patch);
	}
	for (int i = 1; i < N1 - 1; i++)//构造球面四边形网格
	{
		for (int j = 0; j < N2; j++)
		{
			Patch patch;
			patch.obj = this;
			int tempi = i + 1;
			int tempj = j + 1;
			if (tempj == N2) tempj = 0;
			int BodyIndex[4] = {//球面四边形网格索引号数组
				(i - 1) * N2 + j + 1, (tempi - 1) * N2 + j + 1,
				(tempi - 1) * N2 + tempj + 1, (i - 1) * N2 + tempj + 1
			};
			for (int k = 0; k < 4; k++) {
				P3 point4;
				point4.position = P[BodyIndex[k]];
				point4.normal = (point4.position - center).Normalized();
				//point3.texture;
				patch.vertices.push_back(point4);
			}
			patchs.push_back(patch);
		}
	}
	for (int j = 0; j < N2; j++)//构造南极三角形网格
	{
		Patch patch;
		patch.obj = this;
		int tempj = j + 1;
		if (N2 == tempj) tempj = 0;
		int SouthIndex[3] = {//南极三角形网格索引号数组
			(N1 - 2) * N2 + j + 1,
			(N1 - 1) * N2 + 1,
			(N1 - 2) * N2 + tempj + 1 
		};
		for (int k = 0; k < 3; k++) {
			P3 point3;
			point3.position = P[SouthIndex[k]];
			point3.normal = (point3.position - center).Normalized();
			//point3.texture;
			patch.vertices.push_back(point3);
		}
		patchs.push_back(patch);
	}
#endif
    return patchs;
}

void Sphere::ReadVertex()
{
	P[0] = center + Point3d(0.0, r, 0.0, 0.0);
	double gAlpha, gBeta;
	for (int i = 0; i < N1 - 1; ++i) {
		gAlpha = (i + 1.0) * PI / divide;
		for (int j = 0; j < N2; ++j) {
			gBeta = j * PI / divide;
			size_t index = static_cast<size_t>((size_t)i * (size_t)N2 + (size_t)j + 1U);
			P[index].x = r * sin(gAlpha) * sin(gBeta) + center.x;
			P[index].y = r * cos(gAlpha) + center.y;
			P[index].z = r * sin(gAlpha) * cos(gBeta) + center.z;
			P[index].w = 1.0;
		}
	}
	P[static_cast<size_t>(((size_t)N1 - 1U) * (size_t)N2 + 1U)] = center - Point3d(0.0, r, 0.0, 0.0);
}
