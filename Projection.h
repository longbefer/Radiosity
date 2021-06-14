#pragma once
#include "P3.h"

inline double DEG2RAD(double deg) { return deg * PI / 180.0; }

class Projection
{
#if 1
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
#else

public:
	Projection() {
		SetEyePosition(Point3d(0.0, 0.0, 500.0));
		SetProjectionMatrix(90.0, 1.0, 0.1, 1000.0);
	}

	/// <summary>
	/// 设置视点
	/// </summary>
	/// <param name="eye">视点</param>
	void SetEyePosition(Point3d eye) {
		this->eye = eye;
	}

	/// <summary>
	/// 设置投影矩阵
	/// </summary>
	/// <param name="eye_fov">可见范围</param>
	/// <param name="aspect_ratio">宽高比</param>
	/// <param name="zNear">可视的最近距离</param>
	/// <param name="zFar">可视的最远距离</param>
	void SetProjectionMatrix(double eye_fov, double aspect_ratio, double zNear, double zFar) {
		double top = -tan(DEG2RAD(eye_fov / 2.0) * std::abs(zNear));
		double right = top * aspect_ratio;

		double matrix[4][4] = {
			zNear / right, 0.0, 0.0, 0.0,
			0.0, zNear / top, 0.0, 0.0,
			0.0, 0.0, (zNear + zFar) / (zNear - zFar), (2.0 * zNear * zFar) / (zFar - zNear),
			0.0, 0.0, 1.0, 0.0
		};
		transformMatrix.SetMatrix(matrix);
		Matrix eyeMatrix = eyeMatrix.SetTranslate(-eye.x, -eye.y, -eye.z);
		transformMatrix = transformMatrix * eyeMatrix;
	}

	Point3d PerspectiveProjection(Point3d worldPoint) {
		return transformMatrix * worldPoint;
	}

private:
	Point3d eye = ZERO;

	Matrix transformMatrix;
#endif
};

