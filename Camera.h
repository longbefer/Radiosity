#pragma once
#include "P3.h"

struct Cube {
#undef near
#undef far
	double left{ 0 }, right{ 0 }, top{ 0 }, bottom{ 0 }, near{ 0 }, far{ 0 };
};


/// <summary>
/// ����࣬����ͶӰ�任
/// </summary>
class Camera
{
public:
	/// <summary>
	/// �����۾�/���λ��
	/// </summary>
	/// <param name="p">λ��</param>
	void SetEyePosition(const Point3d& p) {
		this->eye = p;
	}
	/// <summary>
	/// �����۾�/���λ��
	/// </summary>
	/// <returns>λ��</returns>
	Point3d GetEyePosition() {
		return eye;
	}

	/// <summary>
	/// �����������
	/// </summary>
	/// <param name="p">�������</param>
	void SetGaze(const Vector3d& p) {
		this->gaze = p;
	}
	Vector3d GetGaze() {
		return gaze;
	}

	/// <summary>
	/// ���������������
	/// </summary>
	/// <param name="p">�����������</param>
	void SetUpDirection(const Vector3d& p) {
		this->up = p;
	}

	/// <summary>
	/// ����ͶӰ�任
	/// </summary>
	/// <param name="eye_fov">�ɼ���Χ��������</param>
	/// <param name="aspect_ratio">��߱�</param>
	/// <param name="zNear">����ɼ�����</param>
	/// <param name="zFar">��Զ�ɼ�����</param>
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
	/// ������Ļ��ߣ�����ͶӰ�任
	/// </summary>
	/// <param name="width">��</param>
	/// <param name="height">��</param>
	void SetViewport(int width, int height) {
		this->width = width;
		this->height = height;
	}
	/// <summary>
	/// ������Ļ��
	/// </summary>
	/// <returns>��Ļ��</returns>
	int GetViewportWidth() {
		return width;
	}
	/// <summary>
	/// ������Ļ��
	/// </summary>
	/// <returns>��Ļ��</returns>
	int GetViewportHeight() {
		return height;
	}

	/// <summary>
	/// ����ͶӰ�����øú���ǰ����Ҫ��SetProjectionMatrix() ��������Ļ��� SetViewport
	/// ��Ҫ����ʵ������ĵ㣬��������Ļ����ϵ�µĵ�
	/// </summary>
	/// <param name="p">ʵ�������ϵĵ�</param>
	/// <returns>������Ļ����ϵ�µĵ�</returns>
	Point3d OrthographicProjection(const Point3d&p) {
		return orthogonalMVP * p;
	}

	/// <summary>
	/// ͸��ͶӰ�����øú���ǰ��������SetProjectionMatrix() ��������Ļ���SetViewport()
	/// ��Ҫ����ʵ������ĵ㣬����͸��ͶӰ����Ļ����ϵ�µĵ�
	/// </summary>
	/// <param name="p">ʵ������ĵ�</param>
	/// <returns>������Ļ����ϵ�µĵ�</returns>
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
	/// ��������������꣬����������任����
	/// </summary>
	void SetCameraMatrix() {
		Vector3d u, w, v;
		w = -gaze.Normalized();
		u = Cross(up, w).Normalized();
		v = Cross(w, u);
		// gaze������up�����غ�
		if (gaze.x == 0.0 && gaze.z == 0.0 && gaze.y < 0.0)//�����ֱ���¹۲�
		{
			u = Vector3d(0.0, 0.0, 1.0, 0.0);
			v = Vector3d(1.0, 0.0, 0.0, 0.0);
			w = Vector3d(0.0, 1.0, 0.0, 0.0);
		}
		if (gaze.x == 0.0 && gaze.z == 0.0 && gaze.y < 0.0)//�����ֱ���Ϲ۲�
		{
			u = Vector3d(1.0, 0.0, 0.0, 0.0);
			v = Vector3d(0.0, 0.0, 1.0, 0.0);
			w = Vector3d(0.0, -1.0, 0.0, 0.0);
		}
		// ��������任����
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
	/// �����Ӵ�����
	/// </summary>
	void SetViewportMatrix() {
		assert(width != 0 && height != 0 && "width and height is empty, please use SetViewport() at Class Camera");
		this->viewportMatrix.SetMatrix({ width / 2.0, .0, .0, width / 2.0,
										.0, -height / 2.0, .0, height / 2.0,
										.0, .0, 1., .0,
										.0, .0, .0, 1. });
		// ע�⣬����ʹ��bmp��Ϊ���ƶ���
		// ����Ҫ��y���򣬹�Ϊ -height/2.0
		// ������Ҫ�������Ϊ height / 2.0
		// ͬ��������Ҫ�ƶ����������ģ��������
		// width / 2.0 �� height / 2.0��Ϊ0����
	}
	/// <summary>
	/// ����ͶӰ����
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
	Point3d eye = Point3d(.0, .0, 300.0);        // �۾�λ��
	Vector3d gaze = Vector3d(.0, .0, -1.0, .0);  // �۲췽��
	Vector3d up = Vector3d(.0, 1., .0, .0);    // �ӵ����Ϸ���

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

