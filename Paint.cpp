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

void Paint::SetPoint(CP3 P0, CP3 P1, CP3 P2)
{
	point0.position.x = Round(P0.position.x);
	point0.position.y = Round(P0.position.y);
	point0.position.z = Round(P0.position.z);
	point0.normal = P0.normal;
	point0.excident = P0.excident;
	point0.texture = P0.texture;
	point1.position.x = Round(P1.position.x);
	point1.position.y = Round(P1.position.y);
	point1.position.z = Round(P1.position.z);
	point1.normal = P1.normal;
	point1.excident = P1.excident;
	point1.texture = P1.texture;
	point2.position.x = Round(P2.position.x);
	point2.position.y = Round(P2.position.y);
	point2.position.z = Round(P2.position.z);
	point2.normal = P2.normal;
	point2.excident = P2.excident;
	point2.texture = P2.texture;
}

void Paint::GouraudShading(CDC* pDC)
{
	//point0��Ϊy������С�ĵ�,point1��Ϊy�������ĵ�,point2���y����λ�ڶ���֮�䡣���yֵ��ͬ��ȡx��С�ĵ�
	SortVertex();
	//���������θ��ǵ�ɨ��������
	int nTotalLine = point1.position.y - point0.position.y + 1;
	//����span��������յ�����
	pLeft = new CPoint2[nTotalLine];
	pRight = new CPoint2[nTotalLine];
	//�ж���������P0P1�ߵ�λ�ù�ϵ��0-1-2Ϊ����ϵ
	int nDeltz = (point1.position.x - point0.position.x) * (point2.position.y - point1.position.y) - 
		         (point1.position.y - point0.position.y) * (point2.position.x - point1.position.x);//��ʸ�������z����
	if (nDeltz > 0)//������λ��P0P1�ߵ����
	{
		nIndex = 0;
		EdgeFlag(point0, point2, TRUE);
		EdgeFlag(point2, point1, TRUE);
		nIndex = 0;
		EdgeFlag(point0, point1, FALSE);
	}
	else//������λ��P0P1�ߵ��Ҳ�
	{
		nIndex = 0;
		EdgeFlag(point0, point1, TRUE);
		nIndex = 0;
		EdgeFlag(point0, point2, FALSE);
		EdgeFlag(point2, point1, FALSE);
	}
	double	CurrentDepth = 0.0;//��ǰɨ���ߵ����
	Vector3d Vector01(Point3d(point1.position.x, point1.position.y, point1.position.z) -
		              Point3d(point0.position.x, point0.position.y, point0.position.z));
	Vector3d Vector02(Point3d(point2.position.x, point2.position.y, point2.position.z) -
		              Point3d(point0.position.x, point0.position.y, point0.position.z));
	Vector3d fNormal = Cross(Vector01, Vector02);//����ƽ�淨ʸ��
	double A = fNormal.x, B = fNormal.y, C = fNormal.z;//ƽ�淽��Ax+By+Cz��D=0��ϵ��
	double D = -A * point0.position.x - B * point0.position.y - C * point0.position.z;//��ǰɨ��������x��������Ȳ���
	if (fabs(C) < 1e-4)
		C = 1.0;
	double DepthStep = -A / C;//��Ȳ���
	for (int y = point0.position.y; y < point1.position.y; y++)//�±��Ͽ�
	{
		int n = y - point0.position.y;
		BOOL bInFlag = FALSE;//���������Ա�־����ʼֵΪ�ٱ�ʾ�������ⲿ
		for (int x = pLeft[n].position.x; x < pRight[n].position.x; x++)//����ҿ�
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
				// Խ�紦��
				if (offsetX >= Width || offsetX < 0 || offsetY >= Height || offsetY < 0) continue;
				//�����ǰ����������С��֡��������ԭ����������)
				if (CurrentDepth < zBuffer[offsetX][offsetY])
				{
					// �������������������ɫ�����Բ�ֵ������������ȡ������ɫ�뵱ǰ��Ƭ��incident��˼�Ϊ��ǰ����ɫ
					CRGB clr = LinearInterp(x, pLeft[n].position.x, pRight[n].position.x, pLeft[n].excident, pRight[n].excident);
					zBuffer[offsetX][offsetY] = CurrentDepth;//ʹ�õ�ǰ���������ȸ�����Ȼ�����
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

void Paint::EdgeFlag(CPoint3 PStart, CPoint3 PEnd, BOOL bFeature)
{
	CRGB crColor = PStart.excident;
	//Normal vect = PStart.normal;
	int dx = abs(PEnd.position.x - PStart.position.x);
	int dy = abs(PEnd.position.y - PStart.position.y);
	BOOL bInterChange = FALSE;//bInterChangeΪ�٣���λ�Ʒ���Ϊx����
	int e, s1, s2, temp;
	s1 = (PEnd.position.x > PStart.position.x) ? 1 : ((PEnd.position.x < PStart.position.x) ? -1 : 0);
	s2 = (PEnd.position.y > PStart.position.y) ? 1 : ((PEnd.position.y < PStart.position.y) ? -1 : 0);
	if (dy > dx)//bInterChangeΪ�棬��λ�Ʒ���Ϊy����
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
			pLeft[nIndex] = CPoint2(x, y, crColor);
		else
			pRight[nIndex] = CPoint2(x, y, crColor);
		if (bInterChange)
		{
			y += s2;
			crColor = LinearInterp(y, PStart.position.y, PEnd.position.y, PStart.excident, PEnd.excident);
			//vect = LinearInterp(y, PStart.position.y, PEnd.position.y, PStart.normal, PEnd.normal);//��ʸ�����Բ�ֵ
			if (bFeature)
				pLeft[++nIndex] = CPoint2(x, y, crColor);
			else
				pRight[++nIndex] = CPoint2(x, y, crColor);
		}
		else
		{
			x += s1;
			crColor = LinearInterp(x, PStart.position.x, PEnd.position.x, PStart.excident, PEnd.excident);
			//vect = LinearInterp(x, PStart.position.x, PEnd.position.x, PStart.normal, PEnd.normal);//��ʸ�����Բ�ֵ
		}
		e += 2 * dy;
		if (e >= 0)
		{
			if (bInterChange)
			{
				x += s1;
				crColor = LinearInterp(y, PStart.position.y, PEnd.position.y, PStart.excident, PEnd.excident);
				//vect = LinearInterp(y, PStart.position.y, PEnd.position.y, PStart.normal, PEnd.normal);//��ʸ�����Բ�ֵ
			}
			else
			{
				y += s2;
				crColor = LinearInterp(x, PStart.position.x, PEnd.position.x, PStart.excident, PEnd.excident);
				//vect = LinearInterp(x, PStart.position.x, PEnd.position.x, PStart.normal, PEnd.normal);//��ʸ�����Բ�ֵ
				//ptTexture = LinearInterp(y, PStart.position.y, PEnd.position.y, PStart.position.t, PEnd.position.t);//��ֵ������һ����������
				if (bFeature)
					pLeft[++nIndex] = CPoint2(x, y, crColor);
				else
					pRight[++nIndex] = CPoint2(x, y, crColor);
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
	for (int i = 0; i < Width; i++)//��ʼ����Ȼ���
		for (int j = 0; j < Height; j++)
			zBuffer[i][j] = Depth;
}

Normal Paint::LinearInterp(double t, double tStart, double tEnd, Normal vStart, Normal vEnd)
{
	Normal vector;
	vector = (tEnd - t) / (tEnd - tStart) * vStart + (t - tStart) / (tEnd - tStart) * vEnd;
	return vector;
}

CRGB Paint::LinearInterp(double t, double mStart, double mEnd, CRGB cStart, CRGB cEnd)
{
	CRGB color;
	color = (t - mEnd) / (mStart - mEnd) * cStart + (t - mStart) / (mEnd - mStart) * cEnd;
	return color;
}

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
