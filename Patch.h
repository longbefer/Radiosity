#pragma once
#include "GameObject.h"
/*
* ��Ƭ�࣬��������
*/
class Patch
{
public:
	double GetArea() {
		// һ����ά�ռ�ͼ��������㹫ʽ
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

public:
	std::vector<P3> vertices;      // vertex for each patch
	Normal normal = XNormal;       // face normal

	Color incident = Black;        // all light from other patch
	Color excident = Black;        // the energy 

	GameObject* obj = nullptr;     // the object of patch
};


/*
* ���嶥����Ϣ�࣬��������Ķ������Ϣ�����ڵ���*.obj�ļ�
*/
class Object {
public:
	std::vector<Point3d> vertices; // ����
	std::vector<Normal3d> normals; // �����Ӧ����
	std::vector<Texture> texture;  // ��������
};