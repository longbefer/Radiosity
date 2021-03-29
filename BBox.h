#pragma once
#include "P3.h"
/*
* ��Χ�У������ཻ����
*/
class BBox
{
public:
	BBox() :BBox(Point3d(-1.0, -1.0, -1.0), Point3d(1.0, 1.0, 1.0)) {};
	BBox(const Point3d& p1, const Point3d& p2) {
		point[0] = p1;
		point[1] = p2;
	};

	bool IsHit(const Ray& ray)const {
		double ox = ray.o.x; double oy = ray.o.y; double oz = ray.o.z;
		double dx = ray.d.x; double dy = ray.d.y; double dz = ray.d.z;

		double tx_min, ty_min, tz_min;
		double tx_max, ty_max, tz_max;

		double a = 1.0 / dx;
		tx_min = (point[0].x - ox) * a;
		tx_max = (point[1].x - ox) * a;
		if (a < 0) std::swap(tx_min, tx_max);

		double b = 1.0 / dy;
		ty_min = (point[0].y - oy) * b;
		ty_max = (point[1].y - oy) * b;
		if (b < 0) std::swap(ty_min, ty_max);

		double c = 1.0 / dz;
		tz_min = (point[0].z - oz) * c;
		tz_max = (point[1].z - oz) * c;
		if (c < 0) std::swap(tz_min, tz_max);

		double t0, t1;
		//�ҵ�����tֵ
		t0 = std::max({ tx_min, ty_min, tz_min });
		//�ҵ���С��tֵ
		t1 = std::min({ tx_max, ty_max, tz_max });

		return (t0 < t1&& t1 > 0.0); //��ײ����
	};
	bool Inside(const Point3d& p)const {
		return ((p.x > point[0].x && p.x < point[1].x) &&
			(p.y > point[0].y && p.y < point[1].y) &&
			(p.z > point[0].z && p.z < point[1].z));
	};

	//void SetTransformMatrix(const Matrix& mat) {
		//for (int i = 0; i < sizeof(point) / sizeof(Point3d); ++i)
			//point[i] = mat * point[i];// �ʣ����Ҳ�ǲ��Եģ�
		// Ҫ��ȡ��Ӧ�İ�Χ�У�Ӧ�ñ������еĵ�ѡȡ�����С�ĵ�����ư�Χ��
		// �Ұ�Χ�Ͳ���Ҫ���ֱ任�����ǲ���ȷ������
	//}

public:
	Point3d point[2];
};

