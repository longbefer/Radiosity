#pragma once
#include "P3.h"

class Paint
{
public:
	Paint();
	virtual ~Paint();
	void SetPoint(CP3 P0, CP3 P1, CP3 P2);//浮点数顶点构造三角形
	void GouraudShading(CDC* pDC);//填充三角形
	void EdgeFlag(CPoint3 PStart, CPoint3 PEnd, BOOL bFeature);//边标记
	void InitDeepBuffer(int Width, int Height, double Depth);
	Normal  LinearInterp(double t, double tStart, double tEnd, Normal vStart, Normal vEnd);//法矢量线性插值
	//纹理线性插值
	CRGB LinearInterp(double t, double mStart, double mEnd, CRGB cStart, CRGB cEnd);//颜色线性插值
	void SortVertex(void);//三角形顶点排序
private:
	CPoint3 point0, point1, point2;//三角形的整数顶点
	CPoint2* pLeft; //跨度的起点数组标志
	CPoint2* pRight;//跨度的终点数组标志
	int nIndex;//扫描线索引
	//ZBuffer
	double A, B, C, D;
	int Width, Height;
	double** zBuffer;//缓深度冲区
};

