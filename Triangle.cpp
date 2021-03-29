#include "pch.h"
#include "Triangle.h"
#include "Patch.h"

bool Triangle::IsHit(const Ray&ray, double&tmin) const
{
	auto &v0 = vertices[0], &v1 = vertices[1], &v2 = vertices[2];
	double a = v0.x - v1.x, b = v0.x - v2.x, c = ray.d.x, d = v0.x - ray.o.x;
	double e = v0.y - v1.y, f = v0.y - v2.y, g = ray.d.y, h = v0.y - ray.o.y;
	double i = v0.z - v1.z, j = v0.z - v2.z, k = ray.d.z, l = v0.z - ray.o.z;

	double m = f * k - g * j, n = h * k - g * l, p = f * l - h * j;
	double q = g * i - e * k, s = e * j - f * i;

	double inv_denom = 1.0 / (a * m + b * q + c * s);

	double e1 = d * m - b * n - c * p;
	double beta = e1 * inv_denom;

	if (beta < -kEpsilon)
		return (false);

	double r = r = e * l - h * i;
	double e2 = a * n + d * q + c * r;
	double gamma = e2 * inv_denom;

	if (gamma < -kEpsilon)
		return (false);

	if (beta + gamma - 1.0 > kEpsilon)
		return (false);

	double e3 = a * p - b * r + d * s;
	double t = e3 * inv_denom;

	if (t < kEpsilon)
		return (false);

	tmin = t;

	return (true);
}

std::vector<Patch> Triangle::ObjectPatch()
{
	std::vector<Patch> patchs;
	DividenPatch(patchs, this->vertices, divide);
	return patchs;
}

void Triangle::DividenPatch(std::vector<Patch>&patchs, Point3d point[3], const int&n)
{
	if (n == 0) {
		Patch patch;
		patch.obj = this;
		patch.normal = (normal[0] + normal[1] + normal[2]) / 3.0;
		P3 patchPoint[3];
		for (size_t i = 0; i < 3; ++i) {
			patchPoint[i].position = point[i];
			patchPoint[i].normal = normal[i]; // 这里必须传递P3以确定准确的向量
			//patchPoint[i].texture;
			patch.vertices.push_back(patchPoint[i]);
		}
		patchs.push_back(patch);
		return;
	}

	// 递归划分
	Point3d mid[3] = { (point[0] + point[1]) / 2.0, (point[1] + point[2]) / 2.0, (point[2] + point[0]) / 2.0 };
	Point3d eachPoint[4][3];
	eachPoint[0][0] = point[0];
	eachPoint[0][1] = mid[0];
	eachPoint[0][2] = mid[2];
	DividenPatch(patchs, eachPoint[0], n - 1);
	eachPoint[1][0] = mid[0];
	eachPoint[1][1] = point[1];
	eachPoint[1][2] = mid[1];
	DividenPatch(patchs, eachPoint[1], n - 1);
	eachPoint[2][0] = mid[2];
	eachPoint[2][1] = mid[1];
	eachPoint[2][2] = point[2];
	DividenPatch(patchs, eachPoint[2], n - 1);
	eachPoint[3][0] = mid[0];
	eachPoint[3][1] = mid[1];
	eachPoint[3][2] = mid[2];
	DividenPatch(patchs, eachPoint[3], n - 1);
}

Normal Triangle::InterpolateNormal(const double& beta, const double& gamma)
{
	Normal normalx((1.0 - beta - gamma) * normal[0] + beta * normal[1] + gamma * normal[2]);
	normalx.Normalize();
	return normalx;
}
