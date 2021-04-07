#pragma once
#include "GameObject.h"
/*
* 面片类，包含顶点
*/
class Patch
{
public:
	double GetArea() {
		// 一般三维空间图形面积计算公式
		double area = 0.0;
		size_t n = vertices.size();
		if (n < 3) throw std::length_error("object's vertices must larger than 3.");
		const Point3d p[3] = { vertices[0].position, vertices[1].position, vertices[2].position };
		double nx = (p[1].y - p[0].y) * (p[2].z - p[0].z) - (p[2].y - p[0].y) * (p[1].z - p[0].z);
		double ny = (p[2].x - p[0].x) * (p[1].z - p[0].z) - (p[1].x - p[0].x) * (p[2].z - p[0].z);
		double nz = (p[1].x - p[0].x) * (p[2].y - p[0].y) - (p[2].x - p[0].x) * (p[1].y - p[0].y);
		double x = (sqrt(pow(nx, 2) + pow(ny, 2) + pow(nz, 2)));
		if (x == 0.0) return 0.0;
		double inv_x = 1.0 / x;
		double cosnx = inv_x * nx;
		double cosny = inv_x * ny;
		double cosnz = inv_x * nz;
		for (size_t i = 0; i < n; ++i) {
			auto& p = vertices[i];
			auto& q = vertices[(i + 1U) % n];
			area += cosnz * (p.position.x * q.position.y - p.position.y * q.position.x)
				+ cosnx * (p.position.y * q.position.z - p.position.z * q.position.y)
				+ cosny * (p.position.z * q.position.x - p.position.x * q.position.z);
		}
		return abs(area) * 0.5;
	}

	Color GetReflectance() {
		// 只实现了矩形的平均反射率
		if (vertices.size() != 4ULL) return this->obj->GetReflectance();
		double u_min = std::min({ vertices[0].texture.u, vertices[1].texture.u, vertices[2].texture.u, vertices[3].texture.u });
		double u_max = std::max({ vertices[0].texture.u, vertices[1].texture.u, vertices[2].texture.u, vertices[3].texture.u });
		double v_min = std::min({ vertices[0].texture.v, vertices[1].texture.v, vertices[2].texture.v, vertices[3].texture.v });
		double v_max = std::max({ vertices[0].texture.v, vertices[1].texture.v, vertices[2].texture.v, vertices[3].texture.v });
		Color ref = Black; double count = 0.0;
		for (double i = u_min; i <= u_max; i += 0.01)
			for (double j = v_min; j <= v_max; j += 0.01) {
				ref += obj->GetTextureImagePixel(Texture(i, j));
				++count;
			}
		return ref / count;
	}

public:
	std::vector<P3> vertices;      // vertex for each patch
	Normal normal = XNormal;       // face normal

	Color incident = Black;        // all light from other patch
	Color excident = Black;        // the energy 

	GameObject* obj = nullptr;     // the object of patch
};


/*
* 物体顶点信息类，包含物体的顶点等信息，用于导入*.obj文件
*/
class Object {
public:
	std::vector<Point3d> vertices; // 顶点
	std::vector<Normal3d> normals; // 顶点对应向量
	std::vector<Texture> texture;  // 纹理坐标
};