#include "pch.h"
#include "Paint.h"

Paint::Paint()
{
	zBuffer = nullptr;
	nIndex = 0;
	A = B = C = D = 0.0;
	Height = Width = 0;
	pLeft = pRight = nullptr;
}

Paint::~Paint()
{
	DeleteBuffer();
}

void Paint::SetPoint(CP3 P0, CP3 P1, CP3 P2)
{
	point0.position.x = Round(P0.position.x);
	point0.position.y = Round(P0.position.y);
#ifdef USE_CAMERA
	point0.position.z = P0.position.z;
#else
	point0.position.z = Round(P0.position.z);
#endif
	point0.normal = P0.normal;
	point0.excident = P0.excident;
	point0.texture = P0.texture;
	point1.position.x = Round(P1.position.x);
	point1.position.y = Round(P1.position.y);
#ifdef USE_CAMERA
	point1.position.z = P1.position.z;
#else
	point1.position.z = Round(P1.position.z);
#endif
	point1.normal = P1.normal;
	point1.excident = P1.excident;
	point1.texture = P1.texture;
	point2.position.x = Round(P2.position.x);
	point2.position.y = Round(P2.position.y);
#ifdef USE_CAMERA
	point2.position.z = P2.position.z;
#else
	point2.position.z = Round(P2.position.z);
#endif
	point2.normal = P2.normal;
	point2.excident = P2.excident;
	point2.texture = P2.texture;
}

void Paint::DeleteBuffer()
{
	if (zBuffer != NULL)
	{
		for (int i = 0; i < Width; i++)
		{
			delete[] zBuffer[i];
			zBuffer[i] = NULL;
		}
		delete zBuffer;
		zBuffer = NULL;
	}
}

#ifdef USE_CAMERA

inline CPoint2 ToPoint2(const CP3& p) {
	CPoint2 point;
	point.excident = p.excident;
	//point.incident = p.incident;
	point.normal = p.normal;
	point.position = Point3i(Round(p.position.x), Round(p.position.y), Round(p.position.z), 1);
	point.texture = p.texture;
	return point;
}

inline CPoint3 ToPoint3(const CP3& p) {
	CPoint3 point;
	point.excident = p.excident;
	//point.incident = p.incident;
	point.normal = p.normal;
	point.position = Point3i(Round(p.position.x), Round(p.position.y), Round(p.position.z), 1);
	point.texture = p.texture;
	return point;
}

void Paint::GouraudShading(std::unique_ptr<BYTE[]>& buffer, const Patch& patch)
{
	// 判断是否超出范围，全部点超出范围取消绘制，减少执行时间
	if (OutOfDrawArea()) return; // 注意：使用该函数必须保证z被规范到[-1,1]之间
	//point0点为y坐标最小的点,point1点为y坐标最大的点,point2点的y坐标位于二者之间。如果y值相同，取x最小的点
	SortVertex();
	//定义三角形覆盖的扫描线条数
	int nTotalLine = static_cast<int>(point1.position.y - point0.position.y + 1.0);
	if (nTotalLine >= 8192) return; // 当绘制的距离太大，舍弃，防止内存不足
	//定义span的起点与终点数组
	pLeft = new(std::nothrow) CPoint2[nTotalLine];
	pRight = new(std::nothrow) CPoint2[nTotalLine];
	if (pLeft == nullptr || pRight == nullptr) return;
	//判断三角形与P0P1边的位置关系，0-1-2为右手系
	double nDeltz = (point1.position.x - point0.position.x) * (point2.position.y - point1.position.y) -
		(point1.position.y - point0.position.y) * (point2.position.x - point1.position.x);//点矢量叉积的z坐标
	if (nDeltz > 0.0)//三角形位于P0P1边的左侧
	{
		nIndex = 0;
		EdgeFlag(ToPoint2(point0), ToPoint2(point2), TRUE);
		EdgeFlag(ToPoint2(point2), ToPoint2(point1), TRUE);
		nIndex = 0;
		EdgeFlag(ToPoint2(point0), ToPoint2(point1), FALSE);
	}
	else//三角形位于P0P1边的右侧
	{
		nIndex = 0;
		EdgeFlag(ToPoint2(point0), ToPoint2(point1), TRUE);
		nIndex = 0;
		EdgeFlag(ToPoint2(point0), ToPoint2(point2), FALSE);
		EdgeFlag(ToPoint2(point2), ToPoint2(point1), FALSE);
	}
	double	CurrentDepth = 0.0;//当前扫描线的深度
	Vector3d Vector01(point1.position - point0.position);
	Vector3d Vector02(point2.position - point0.position);
	Vector3d fNormal = Cross(Vector01, Vector02);//这是平面法矢量
	double A = fNormal.x, B = fNormal.y, C = fNormal.z;//平面方程Ax+By+Cz＋D=0的系数
	double D = -A * point0.position.x - B * point0.position.y - C * point0.position.z;//当前扫描线随着x增长的深度步长
	if (fabs(C) < 1e-4)
		C = 1.0;
	double DepthStep = -A / C;//深度步长
	for (int y = static_cast<int>(point0.position.y); y < point1.position.y && y < Height; ++y)//下闭上开
	{
		if (y < 0) continue;
		int n = static_cast<int>(y - point0.position.y);
		BOOL bInFlag = FALSE;//跨度内外测试标志，初始值为假表示三角形外部
		for (int x = pLeft[n].position.x; x < pRight[n].position.x; ++x)//左闭右开
		{
			if (bInFlag == FALSE)
			{
				CurrentDepth = -(A * x + B * y + D) / C;//z=-(Ax+By+D)/C
				bInFlag = TRUE;
				x -= 1;
			}
			else
			{
				//如果当前采样点的深度小于帧缓冲器中原采样点的深度)
				if (x >= 0 && x < Width && CurrentDepth >= zBuffer[x][y])
				{
					Color clr = LinearInterp(x, pLeft[n].position.x, pRight[n].position.x, pLeft[n].excident, pRight[n].excident);
					// 若不存在纹理则进行颜色的线性插值，存在纹理则取纹理颜色与当前面片的incident相乘即为当前的颜色
					if (patch.obj && patch.obj->GetImage()) {
						Texture t = LinearInterp(x, pLeft[n].position.x, pRight[n].position.x, pLeft[n].texture, pRight[n].texture);
						clr = (patch.obj->GetTextureImagePixel(t) * patch.excident).Clamp();
					}
					zBuffer[x][y] = CurrentDepth;//使用当前采样点的深度更新深度缓冲器
					// pDC->SetPixelV(x, y, RGB(clr.r * 255, clr.g * 255, clr.b * 255));
					size_t index = y * 4ULL * Width + 4ULL * x;
					buffer[index] = static_cast<BYTE>(clr.b * 255);
					buffer[index + 1] = static_cast<BYTE>(clr.g * 255);
					buffer[index + 2] = static_cast<BYTE>(clr.r * 255);
					buffer[index + 3] = static_cast<BYTE>(clr.alpha * 255);
				}
				CurrentDepth += DepthStep;
			}
		}
	}
	if (pLeft)
	{
		delete[]pLeft;
		pLeft = NULL;
	}
	if (pRight)
	{
		delete[]pRight;
		pRight = NULL;
	}
}
#else
void Paint::GouraudShading(CDC* pDC, const Patch&patch)
{
	//point0点为y坐标最小的点,point1点为y坐标最大的点,point2点的y坐标位于二者之间。如果y值相同，取x最小的点
	SortVertex();
	//定义三角形覆盖的扫描线条数
	int nTotalLine = point1.position.y - point0.position.y + 1;
	//定义span的起点与终点数组
	pLeft = new CPoint2[nTotalLine];
	pRight = new CPoint2[nTotalLine];
	//判断三角形与P0P1边的位置关系，0-1-2为右手系
	int nDeltz = (point1.position.x - point0.position.x) * (point2.position.y - point1.position.y) - 
		         (point1.position.y - point0.position.y) * (point2.position.x - point1.position.x);//点矢量叉积的z坐标
	if (nDeltz > 0)//三角形位于P0P1边的左侧
	{
		nIndex = 0;
		EdgeFlag(point0, point2, TRUE);
		EdgeFlag(point2, point1, TRUE);
		nIndex = 0;
		EdgeFlag(point0, point1, FALSE);
	}
	else//三角形位于P0P1边的右侧
	{
		nIndex = 0;
		EdgeFlag(point0, point1, TRUE);
		nIndex = 0;
		EdgeFlag(point0, point2, FALSE);
		EdgeFlag(point2, point1, FALSE);
	}
	double	CurrentDepth = 0.0;//当前扫描线的深度
	Vector3d Vector01(Point3d(point1.position.x, point1.position.y, point1.position.z) -
		              Point3d(point0.position.x, point0.position.y, point0.position.z));
	Vector3d Vector02(Point3d(point2.position.x, point2.position.y, point2.position.z) -
		              Point3d(point0.position.x, point0.position.y, point0.position.z));
	Vector3d fNormal = Cross(Vector01, Vector02);//这是平面法矢量
	double A = fNormal.x, B = fNormal.y, C = fNormal.z;//平面方程Ax+By+Cz＋D=0的系数
	double D = -A * point0.position.x - B * point0.position.y - C * point0.position.z;//当前扫描线随着x增长的深度步长
	if (fabs(C) < 1e-4)
		C = 1.0;
	double DepthStep = -A / C;//深度步长
	for (int y = point0.position.y; y < point1.position.y; y++)//下闭上开
	{
		int n = y - point0.position.y;
		BOOL bInFlag = FALSE;//跨度内外测试标志，初始值为假表示三角形外部
		for (int x = pLeft[n].position.x; x < pRight[n].position.x; x++)//左闭右开
		{
			if (bInFlag == FALSE)
			{
				CurrentDepth = -(A * x + B * y + D) / C;//z=-(Ax+By+D)/C
				bInFlag = TRUE;
				x -= 1;
			}
			else
			{
				int offsetX = x + Width / 2;
				int offsetY = y + Height / 2;
				// 越界处理
				if (offsetX >= Width || offsetX < 0 || offsetY >= Height || offsetY < 0) continue;
				//如果当前采样点的深度小于帧缓冲器中原采样点的深度)
				if (CurrentDepth < zBuffer[offsetX][offsetY])
				{
					Color clr = LinearInterp(x, pLeft[n].position.x, pRight[n].position.x, pLeft[n].excident, pRight[n].excident);
					// 若不存在纹理则进行颜色的线性插值，存在纹理则取纹理颜色与当前面片的incident相乘即为当前的颜色
					if (patch.obj && patch.obj->GetImage()) {
						Texture t = LinearInterp(x, pLeft[n].position.x, pRight[n].position.x, pLeft[n].texture, pRight[n].texture);
						clr = (patch.obj->GetTextureImagePixel(t) * patch.excident).Clamp();
					}
					zBuffer[offsetX][offsetY] = CurrentDepth;//使用当前采样点的深度更新深度缓冲器
					pDC->SetPixelV(x, y, RGB(clr.r * 255, clr.g * 255, clr.b * 255));
				}
				CurrentDepth += DepthStep;
			}
		}
	}
	if (pLeft)
	{
		delete[]pLeft;
		pLeft = NULL;
	}
	if (pRight)
	{
		delete[]pRight;
		pRight = NULL;
	}
}
#endif

void Paint::EdgeFlag(CPoint3 PStart, CPoint3 PEnd, BOOL bFeature)
{
	CRGB crColor = PStart.excident;
	//Normal vect = PStart.normal;
	Texture ptTexture = PStart.texture;
	int dx = abs(PEnd.position.x - PStart.position.x);
	int dy = abs(PEnd.position.y - PStart.position.y);
	BOOL bInterChange = FALSE;//bInterChange为假，主位移方向为x方向
	int e, s1, s2, temp;
	s1 = (PEnd.position.x > PStart.position.x) ? 1 : ((PEnd.position.x < PStart.position.x) ? -1 : 0);
	s2 = (PEnd.position.y > PStart.position.y) ? 1 : ((PEnd.position.y < PStart.position.y) ? -1 : 0);
	if (dy > dx)//bInterChange为真，主位移方向为y方向
	{
		temp = dx;
		dx = dy;
		dy = temp;
		bInterChange = TRUE;
	}
	e = -dx;
	int x = PStart.position.x, y = PStart.position.y;
	for (int i = 0; i < dx; i++)
	{
		if (bFeature)
			pLeft[nIndex] = CPoint2(x, y, crColor, ptTexture);
		else
			pRight[nIndex] = CPoint2(x, y, crColor, ptTexture);
		if (bInterChange)
		{
			y += s2;
			crColor = LinearInterp(y, PStart.position.y, PEnd.position.y, PStart.excident, PEnd.excident);
			ptTexture = LinearInterp(y, PStart.position.y, PEnd.position.y, PStart.texture, PEnd.texture);
			if (bFeature)
				pLeft[++nIndex] = CPoint2(x, y, crColor, ptTexture);
			else
				pRight[++nIndex] = CPoint2(x, y, crColor, ptTexture);
		}
		else
		{
			x += s1;
			crColor = LinearInterp(x, PStart.position.x, PEnd.position.x, PStart.excident, PEnd.excident);
			ptTexture = LinearInterp(x, PStart.position.x, PEnd.position.x, PStart.texture, PEnd.texture);
		}
		e += 2 * dy;
		if (e >= 0)
		{
			if (bInterChange)
			{
				x += s1;
				crColor = LinearInterp(y, PStart.position.y, PEnd.position.y, PStart.excident, PEnd.excident);
				ptTexture = LinearInterp(y, PStart.position.y, PEnd.position.y, PStart.texture, PEnd.texture);
			}
			else
			{
				y += s2;
				crColor = LinearInterp(x, PStart.position.x, PEnd.position.x, PStart.excident, PEnd.excident);
				ptTexture = LinearInterp(x, PStart.position.x, PEnd.position.x, PStart.texture, PEnd.texture);
				if (bFeature)
					pLeft[++nIndex] = CPoint2(x, y, crColor, ptTexture);
				else
					pRight[++nIndex] = CPoint2(x, y, crColor, ptTexture);
			}
			e -= 2 * dx;
		}
	}
}

void Paint::InitDeepBuffer(int Width, int Height, double Depth)
{
	this->Width = Width, this->Height = Height;
	zBuffer = new double* [Width];
	for (int i = 0; i < Width; i++)
		zBuffer[i] = new double[Height];
	for (int i = 0; i < Width; i++)//初始化深度缓冲
		for (int j = 0; j < Height; j++)
			zBuffer[i][j] = Depth;
}

Normal Paint::LinearInterp(double t, double tStart, double tEnd, Normal vStart, Normal vEnd)
{
	Normal vector;
	vector = (tEnd - t) / (tEnd - tStart) * vStart + (t - tStart) / (tEnd - tStart) * vEnd;
	return vector;
}

Texture Paint::LinearInterp(double t, double tStart, double tEnd, Texture vStart, Texture vEnd)
{
	Texture texture;
	texture = vStart * ((tEnd - t) / (tEnd - tStart)) + vEnd * ((t - tStart) / (tEnd - tStart));
	return texture;
}

CRGB Paint::LinearInterp(double t, double mStart, double mEnd, CRGB cStart, CRGB cEnd)
{
	CRGB color;
	color = (t - mEnd) / (mStart - mEnd) * cStart + (t - mStart) / (mEnd - mStart) * cEnd;
	return color;
}

#ifdef USE_CAMERA
void Paint::SortVertex(void)
{
	CP3 pt[3];
	pt[0] = point0;
	pt[1] = point1;
	pt[2] = point2;
	for (int i = 0; i < 2; i++)
	{
		for (int j = i + 1; j < 3; j++)
		{
			int k = i;
			if (pt[k].position.y >= pt[j].position.y)
				k = j;
			if (k == j)
			{
				CP3 ptTemp = pt[i];
				pt[i] = pt[k];
				pt[k] = ptTemp;
			}
		}
	}
	point0 = pt[0];
	point1 = pt[2];
	point2 = pt[1];
}
#else
void Paint::SortVertex(void)
{
	CPoint3 pt[3];
	pt[0] = point0;
	pt[1] = point1;
	pt[2] = point2;
	for (int i = 0; i < 2; i++)
	{
		for (int j = i + 1; j < 3; j++)
		{
			int k = i;
			if (pt[k].position.y >= pt[j].position.y)
				k = j;
			if (k == j)
			{
				CPoint3 ptTemp = pt[i];
				pt[i] = pt[k];
				pt[k] = ptTemp;
			}
		}
	}
	point0 = pt[0];
	point1 = pt[2];
	point2 = pt[1];
}
#endif

