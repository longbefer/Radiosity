#pragma once
#include "P3.h"

class Projection
{
public:
	Projection() {
		R = 1200, d = 800, Phi = 90.0, Psi = 0.0;//视点位于正前方
		InitialParameter();
	};
	virtual ~Projection() {};
	void InitialParameter(void) {
		k[0] = sin(PI * Phi / 180);//Phi代表φ
		k[1] = cos(PI * Phi / 180);
		k[2] = sin(PI * Psi / 180);//Psi代表ψ
		k[3] = cos(PI * Psi / 180);
		k[4] = k[0] * k[2];
		k[5] = k[0] * k[3];
		k[6] = k[1] * k[2];
		k[7] = k[1] * k[3];
		eyePoint.x = R * k[4];//设置视点	
		eyePoint.y = R * k[1];
		eyePoint.z = R * k[5];
	}
	Point3d PerspectiveProjection(Point3d worldPoint) {
		Point3d ViewPoint;//观察坐标系三维点
		ViewPoint.x = k[3] * worldPoint.x - k[2] * worldPoint.z;
		ViewPoint.y = -k[6] * worldPoint.x + k[0] * worldPoint.y - k[7] * worldPoint.z;
		ViewPoint.z = -k[4] * worldPoint.x - k[1] * worldPoint.y - k[5] * worldPoint.z + R;
		Point3d ScreenPoint;//屏幕坐标系三维点
		ScreenPoint.x = d * ViewPoint.x / ViewPoint.z;
		ScreenPoint.y = d * ViewPoint.y / ViewPoint.z;
		ScreenPoint.z = (ViewPoint.z - d) * d / ViewPoint.z;//Bouknight公式;
		return ScreenPoint;
	}
	Point3d OrthogonalProjection(Point3d worldPoint) {
		Point3d screenPoint;
		screenPoint.x = worldPoint.x;
		screenPoint.y = worldPoint.y;
		screenPoint.z = -worldPoint.z;
		return screenPoint;
	}
public:
	Point3d eyePoint;      // 视点
private:
	double k[8];           // 透视投影常数
	double R, Phi, Psi, d; // 视点球坐标
};

