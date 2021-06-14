#pragma once
#include "P3.h"

struct Cube {
#undef near
#undef far
	double left{ 0 }, right{ 0 }, top{ 0 }, bottom{ 0 }, near{ 0 }, far{ 0 };
};


/// <summary>
/// 相机类，用于投影变换
/// </summary>
class Camera
{
public:
	/// <summary>
	/// 设置眼睛/相机位置
	/// </summary>
	/// <param name="p">位置</param>
	void SetEyePosition(const Point3d& p) {
		this->eye = p;
	}
	/// <summary>
	/// 返回眼睛/相机位置
	/// </summary>
	/// <returns>位置</returns>
	Point3d GetEyePosition() {
		return eye;
	}

	/// <summary>
	/// 设置相机朝向
	/// </summary>
	/// <param name="p">相机朝向</param>
	void SetGaze(const Vector3d& p) {
		this->gaze = p;
	}
	Vector3d GetGaze() {
		return gaze;
	}

	/// <summary>
	/// 设置相机顶部方向
	/// </summary>
	/// <param name="p">相机顶部方向</param>
	void SetUpDirection(const Vector3d& p) {
		this->up = p;
	}

	/// <summary>
	/// 设置投影变换
	/// </summary>
	/// <param name="eye_fov">可见范围（度数）</param>
	/// <param name="aspect_ratio">宽高比</param>
	/// <param name="zNear">最近可见距离</param>
	/// <param name="zFar">最远可见距离</param>
	void SetProjectionMatrix(double eye_fov, double aspect_ratio, double zNear, double zFar) {
		SetCameraMatrix();
		double radian = PI / 180.0;
		double h = tan(eye_fov / 2.0 * radian) * zNear * 2.0;
		double w = h * aspect_ratio; // aspect_ratio = w / h
		l = -w / 2.0, r = w / 2.0, t = h / 2.0,
			b = -h / 2.0, n = -zNear, f = -zFar;
		SetViewportMatrix();
		SetProjectionMatrix();
	}

	/// <summary>
	/// 设置屏幕宽高，用于投影变换
	/// </summary>
	/// <param name="width">宽</param>
	/// <param name="height">高</param>
	void SetViewport(int width, int height) {
		this->width = width;
		this->height = height;
	}
	/// <summary>
	/// 返回屏幕宽
	/// </summary>
	/// <returns>屏幕宽</returns>
	int GetViewportWidth() {
		return width;
	}
	/// <summary>
	/// 返回屏幕高
	/// </summary>
	/// <returns>屏幕高</returns>
	int GetViewportHeight() {
		return height;
	}

	/// <summary>
	/// 正交投影，调用该函数前，需要先SetProjectionMatrix() 并设置屏幕宽高 SetViewport
	/// 需要传递实际物体的点，将返回屏幕坐标系下的点
	/// </summary>
	/// <param name="p">实际物体上的点</param>
	/// <returns>返回屏幕坐标系下的点</returns>
	Point3d OrthographicProjection(const Point3d&p) {
		return orthogonalMVP * p;
	}

	/// <summary>
	/// 透视投影，调用该函数前请先设置SetProjectionMatrix() 并设置屏幕宽高SetViewport()
	/// 需要传递实际物体的点，返回透视投影下屏幕坐标系下的点
	/// </summary>
	/// <param name="p">实际物体的点</param>
	/// <returns>返回屏幕坐标系下的点</returns>
	Point3d PerspectiveProjection(const Point3d& p) {
		return perspectiveMVP * p;
	}

	Cube GetProjectCube() {
		Cube cube{ 0 };
		cube.left = width * -0.5;
		cube.right = -cube.left;
		cube.top = height * 0.5;
		cube.bottom = -cube.top;
		cube.far = eye.z + gaze.z * f;
		cube.near = eye.z + gaze.z * n;
		return cube;
	}


private:
	/// <summary>
	/// 设置相机各项坐标，并设置相机变换矩阵
	/// </summary>
	void SetCameraMatrix() {
		Vector3d u, w, v;
		w = -gaze.Normalized();
		u = Cross(up, w).Normalized();
		v = Cross(w, u);
		// gaze不能与up向量重合
		if (gaze.x == 0.0 && gaze.z == 0.0 && gaze.y < 0.0)//相机垂直向下观察
		{
			u = Vector3d(0.0, 0.0, 1.0, 0.0);
			v = Vector3d(1.0, 0.0, 0.0, 0.0);
			w = Vector3d(0.0, 1.0, 0.0, 0.0);
		}
		if (gaze.x == 0.0 && gaze.z == 0.0 && gaze.y < 0.0)//相机垂直向上观察
		{
			u = Vector3d(1.0, 0.0, 0.0, 0.0);
			v = Vector3d(0.0, 0.0, 1.0, 0.0);
			w = Vector3d(0.0, -1.0, 0.0, 0.0);
		}
		// 设置相机变换矩阵
		Matrix cameraMatrix({ u.x, u.y, u.z, .0,
							v.x, v.y, v.z, .0,
							w.x, w.y, w.z, .0,
							.0,   .0,  .0, 1.
			});
		Matrix eyeMatrix({ 1., .0, .0, -eye.x,
						.0, 1., .0, -eye.y,
						.0, .0, 1., -eye.z,
						.0, .0, .0, 1.
			});
		this->cameraMatrix = cameraMatrix * eyeMatrix;
	}
	/// <summary>
	/// 设置视窗矩阵
	/// </summary>
	void SetViewportMatrix() {
		assert(width != 0 && height != 0 && "width and height is empty, please use SetViewport() at Class Camera");
		this->viewportMatrix.SetMatrix({ width / 2.0, .0, .0, width / 2.0,
										.0, -height / 2.0, .0, height / 2.0,
										.0, .0, 1., .0,
										.0, .0, .0, 1. });
		// 注意，由于使用bmp作为绘制对象
		// 故需要将y反向，故为 -height/2.0
		// 若不需要反向，则改为 height / 2.0
		// 同理，若不需要移动视区到中心，将后面的
		// width / 2.0 和 height / 2.0改为0即可
	}
	/// <summary>
	/// 设置投影矩阵
	/// </summary>
	void SetProjectionMatrix() {
		this->orthogonalMatrix.SetMatrix({ 2.0 / (r - l), .0, .0, (r + l) / (l - r),
											.0, 2.0 / (t - b), .0, (t + b) / (b - t),
											.0, .0, 2.0 / (n - f), (n + f) / (f - n),
											.0, .0, .0, 1. });
		Matrix projection_to_orthogonal({ n, .0, .0, .0,
										 .0, n, .0, .0,
										 .0, .0, n + f, -1 * n * f,
										 .0, .0, 1., .0 });
		//this->perspectiveMatrix.SetMatrix({ 2.0 * n / (r - l), .0, (l + r) / (l - r), .0,
		//									.0, 2.0 * n / (t - b), (b + t) / (b - t), .0,
		//									.0, .0, (f + n) / (n - f), 2.0 * f * n / (f - n),
		//									.0, .0, 1., .0 });
		this->perspectiveMatrix = orthogonalMatrix * projection_to_orthogonal;

		this->orthogonalMVP = viewportMatrix * orthogonalMatrix * cameraMatrix;
		this->perspectiveMVP = viewportMatrix * perspectiveMatrix * cameraMatrix;
	}

private:
	Point3d eye = Point3d(.0, .0, 300.0);        // 眼睛位置
	Vector3d gaze = Vector3d(.0, .0, -1.0, .0);  // 观察方向
	Vector3d up = Vector3d(.0, 1., .0, .0);    // 视点正上方向

	Matrix cameraMatrix;
	Matrix orthogonalMatrix;
	Matrix viewportMatrix;
	Matrix perspectiveMatrix;

	Matrix perspectiveMVP;
	Matrix orthogonalMVP;

	int width = 0, height = 0;
	// left, right, top, buttom, near, far
	double l = .0, r = .0, t = .0, b = .0, n = .0, f = .0;
};

