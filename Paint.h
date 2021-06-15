#pragma once
#include "Patch.h"

class Paint
{
public:
	Paint();
	virtual ~Paint();
	void SetPoint(CP3 P0, CP3 P1, CP3 P2);//浮点数顶点构造三角形
	void DeleteBuffer(); // 删除Buffer
#ifdef USE_CAMERA
	void GouraudShading(std::unique_ptr<BYTE[]>&, const Patch&);//填充三角形
	bool OutOfDrawArea() {
		// 若三个点均不在范围内，则取消该三角形的绘制（注意：此方法并非正确裁剪，但可以近似正常显示裁剪结果）
		return ((((point0.position.x <= .0 || point0.position.x >= static_cast<double>(Width)) ||
			(point0.position.y <= .0 || point0.position.y >= static_cast<double>(Height))) &&
			((point1.position.x <= .0 || point1.position.x >= static_cast<double>(Width)) ||
				(point1.position.y <= .0 || point1.position.y >= static_cast<double>(Height))) &&
			((point2.position.x <= .0 || point2.position.x >= static_cast<double>(Width)) ||
				(point2.position.y <= .0 || point2.position.y >= static_cast<double>(Height))))
			// 若不在z的范围内，即使xy在范围内也需要消除，若没有规范到-1.0 ~ 1.0则需要注释掉下面内容
			|| ((point0.position.z < -1.0 || point0.position.z > 1.0)) &&
			(point1.position.z < -1.0 || point1.position.z > 1.0) &&
			(point2.position.z < -1.0 || point2.position.z > 1.0));
	}
#else
	void GouraudShading(CDC* pDC, const Patch&);//填充三角形
#endif
	void EdgeFlag(CPoint3 PStart, CPoint3 PEnd, BOOL bFeature);//边标记
	void InitDeepBuffer(int Width, int Height, double Depth);
	Normal  LinearInterp(double t, double tStart, double tEnd, Normal vStart, Normal vEnd);//法矢量线性插值
	Texture LinearInterp(double t, double tStart, double tEnd, Texture vStart, Texture vEnd);
	//纹理线性插值
	CRGB LinearInterp(double t, double mStart, double mEnd, CRGB cStart, CRGB cEnd);//颜色线性插值
	void SortVertex(void);//三角形顶点排序
private:
#ifdef USE_CAMERA
	CP3 point0, point1, point2;//三角形的整数顶点
#else
	CPoint3 point0, point1, point2;//三角形的整数顶点
#endif
	CPoint2* pLeft; //跨度的起点数组标志
	CPoint2* pRight;//跨度的终点数组标志
	int nIndex;//扫描线索引
	//ZBuffer
	double A, B, C, D;
	int Width, Height;
	double** zBuffer;//缓深度冲区
};

