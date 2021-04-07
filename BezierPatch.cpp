#include "pch.h"
#include "BezierPatch.h"
#include "Patch.h"
#include "Rectangle.h"
#include "Triangle.h"
#include "SmoothTriangle.h"

void BezierPatch::ReadControlPoint(Point3d* CtrPt)
{
	for (int i = 0; i < 16; i++)
		this->CtrPt[i] = this->transformMatrix * CtrPt[i];
	SetCurvedPatch();
}

void BezierPatch::SetCurvedPatch(void)
{
	Texture Rect[4];
	Rect[0] = Texture(0.0, 0.0); Rect[1] = Texture(1.0, 0.0);//初始化uv
	Rect[2] = Texture(1.0, 1.0); Rect[3] = Texture(0.0, 1.0);
	Recursion(divide, Rect);//递归函数
}

void BezierPatch::Recursion(int ReNumber, Texture* pRect)
{
	if (0 == ReNumber)
	{
		Point3d P[4];////四边形平面网格顶点
		Vector3d N[4];//顶点法向量
		Tessellation(pRect, P, N);//细分曲面，将（u，v）点转换为（x，y）点
		return;
	}
	else
	{
		double HalfU = (pRect[0].u + pRect[1].u) / 2.0;
		double HalfV = (pRect[0].v + pRect[3].v) / 2.0;
		Texture SubRect[4][4];//四叉树
		//左下子长方形
		SubRect[0][0] = pRect[0];
		SubRect[0][1].u = HalfU;      SubRect[0][1].v = pRect[0].v;
		SubRect[0][2].u = HalfU;      SubRect[0][2].v = HalfV;
		SubRect[0][3].u = pRect[0].u; SubRect[0][3].v = HalfV;
		//右下子长方形
		SubRect[1][0] = SubRect[0][1];
		SubRect[1][1] = pRect[1];
		SubRect[1][2].u = pRect[1].u; SubRect[1][2].v = HalfV;
		SubRect[1][3] = SubRect[0][2];
		//右上子长方形
		SubRect[2][0] = SubRect[1][3];
		SubRect[2][1] = SubRect[1][2];
		SubRect[2][2] = pRect[2];
		SubRect[2][3].u = HalfU;      SubRect[2][3].v = pRect[2].v;
		//左上子长方形
		SubRect[3][0] = SubRect[0][3];
		SubRect[3][1] = SubRect[2][0];
		SubRect[3][2] = SubRect[2][3];
		SubRect[3][3] = pRect[3];
		Recursion(ReNumber - 1, SubRect[0]);
		Recursion(ReNumber - 1, SubRect[1]);
		Recursion(ReNumber - 1, SubRect[2]);
		Recursion(ReNumber - 1, SubRect[3]);
	}
}

void BezierPatch::Tessellation(Texture* mesh, Point3d* P, Vector3d* N)
{
	double M[4][4]{0};//系数矩阵M
	M[0][0] = -1, M[0][1] = 3, M[0][2] = -3, M[0][3] = 1;
	M[1][0] = 3, M[1][1] = -6, M[1][2] = 3, M[1][3] = 0;
	M[2][0] = -3, M[2][1] = 3, M[2][2] = 0, M[2][3] = 0;
	M[3][0] = 1, M[3][1] = 0, M[3][2] = 0, M[3][3] = 0;
	Point3d P3[4][4];//曲线计算用控制点数组
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			P3[i][j] = CtrPt[i * 4 + j];
	LeftMultiplyMatrix(M, P3);//系数矩阵左乘三维点矩阵
	TransposeMatrix(M);//计算转置矩阵
	RightMultiplyMatrix(P3, M);//系数矩阵右乘三维点矩阵
	double u0, u1, u2, u3, v0, v1, v2, v3;//u，v参数的幂
	double u[4] = { mesh[0].u,mesh[1].u ,mesh[2].u ,mesh[3].u };
	double v[4] = { mesh[0].v,mesh[1].v ,mesh[2].v ,mesh[3].v };
	for (int i = 0; i < 4; i++)
	{
		u3 = pow(u[i], 3.0), u2 = pow(u[i], 2.0), u1 = u[i], u0 = 1;
		v3 = pow(v[i], 3.0), v2 = pow(v[i], 2.0), v1 = v[i], v0 = 1;
		P[i] = (u3 * P3[0][0] + u2 * P3[1][0] + u1 * P3[2][0] + u0 * P3[3][0]) * v3
			+ (u3 * P3[0][1] + u2 * P3[1][1] + u1 * P3[2][1] + u0 * P3[3][1]) * v2
			+ (u3 * P3[0][2] + u2 * P3[1][2] + u1 * P3[2][2] + u0 * P3[3][2]) * v1
			+ (u3 * P3[0][3] + u2 * P3[1][3] + u1 * P3[2][3] + u0 * P3[3][3]) * v0;
		//计算顶点法向量
		Point3d uTagent, vTagent;
		double du3, du2, du1, du0;
		du3 = 3.0 * pow(u[i], 2.0), du2 = 2.0 * u[i], du1 = 1, du0 = 0.0;
		v3 = pow(v[i], 3.0), v2 = pow(v[i], 2.0), v1 = v[i], v0 = 1.0;
		uTagent = (du3 * P3[0][0] + du2 * P3[1][0] + du1 * P3[2][0] + du0 * P3[3][0]) * v3
			+ (du3 * P3[0][1] + du2 * P3[1][1] + du1 * P3[2][1] + du0 * P3[3][1]) * v2
			+ (du3 * P3[0][2] + du2 * P3[1][2] + du1 * P3[2][2] + du0 * P3[3][2]) * v1
			+ (du3 * P3[0][3] + du2 * P3[1][3] + du1 * P3[2][3] + du0 * P3[3][3]) * v0;

		double dv3, dv2, dv1, dv0;
		u3 = pow(u[i], 3.0), u2 = pow(u[i], 2.0), u1 = u[i], u0 = 1.0;
		dv3 = 3.0 * pow(v[i], 2.0), dv2 = 2.0 * v[i], dv1 = 1.0, dv0 = 0.0;
		vTagent = (u3 * P3[0][0] + u2 * P3[1][0] + u1 * P3[2][0] + u0 * P3[3][0]) * dv3
			+ (u3 * P3[0][1] + u2 * P3[1][1] + u1 * P3[2][1] + u0 * P3[3][1]) * dv2
			+ (u3 * P3[0][2] + u2 * P3[1][2] + u1 * P3[2][2] + u0 * P3[3][2]) * dv1
			+ (u3 * P3[0][3] + u2 * P3[1][3] + u1 * P3[2][3] + u0 * P3[3][3]) * dv0;
		N[i] = Cross(uTagent, vTagent);
		N[i].Normalize();//规范化顶点法向量
		Vector3d Pu = Vector3d(uTagent.x, uTagent.y, uTagent.z, 0.0), Pv = Vector3d(vTagent.x, vTagent.y, vTagent.z, 0.0);
		if (Pv.Distance() < 1e-4)
		{
			N[i] = Vector3d(P[i].x, P[i].y, P[i].z, 0.0);
			N[i].Normalize();
		}//南北极点的V向偏导数为零

		if (Pu.Distance() < 1e-4)
		{
			N[i] = Vector3d(P[i].x , P[i].y, P[i].z, 0.0);
			N[i].Normalize();
		}//南北极点的U向偏导数为零
	}

	SmoothTriangle* triangle_ptr1 = new SmoothTriangle(P[0], P[1], P[2]);
	triangle_ptr1->SetNormal(N[0], N[1], N[2]);
	triangle_ptr1->SetRecursionTimes(0);
	triangle_ptr1->SetColor(reflectance);
	triangle_ptr1->SetEmmision(emmision);
	objects.push_back(triangle_ptr1);

	SmoothTriangle* triangle_ptr2 = new SmoothTriangle(P[0], P[2], P[3]);
	triangle_ptr2->SetNormal(N[0], N[2], N[3]);
	triangle_ptr2->SetRecursionTimes(0);
	triangle_ptr2->SetColor(reflectance);
	triangle_ptr2->SetEmmision(emmision);
	objects.push_back(triangle_ptr2);
}

void BezierPatch::LeftMultiplyMatrix(double M[4][4], Point3d P[4][4])
{
	Point3d PTemp[4][4];//临时矩阵
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			PTemp[i][j] = M[i][0] * P[0][j] + M[i][1] * P[1][j] + M[i][2] * P[2][j] + M[i][3] * P[3][j];
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			P[i][j] = PTemp[i][j];
}

void BezierPatch::RightMultiplyMatrix(Point3d P[4][4], double M[4][4])
{
	Point3d PTemp[4][4];//临时矩阵
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			PTemp[i][j] = P[i][0] * M[0][j] + P[i][1] * M[1][j] + P[i][2] * M[2][j] + P[i][3] * M[3][j];
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			P[i][j] = PTemp[i][j];
}

void BezierPatch::TransposeMatrix(double M[4][4])
{
	double PTemp[4][4]{0};//临时矩阵
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			PTemp[j][i] = M[i][j];
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			M[i][j] = PTemp[i][j];
}
