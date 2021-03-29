#pragma once
#include "P3.h"

class Paint
{
public:
	Paint();
	virtual ~Paint();
	void SetPoint(CP3 P0, CP3 P1, CP3 P2);//���������㹹��������
	void GouraudShading(CDC* pDC);//���������
	void EdgeFlag(CPoint3 PStart, CPoint3 PEnd, BOOL bFeature);//�߱��
	void InitDeepBuffer(int Width, int Height, double Depth);
	Normal  LinearInterp(double t, double tStart, double tEnd, Normal vStart, Normal vEnd);//��ʸ�����Բ�ֵ
	//�������Բ�ֵ
	CRGB LinearInterp(double t, double mStart, double mEnd, CRGB cStart, CRGB cEnd);//��ɫ���Բ�ֵ
	void SortVertex(void);//�����ζ�������
private:
	CPoint3 point0, point1, point2;//�����ε���������
	CPoint2* pLeft; //��ȵ���������־
	CPoint2* pRight;//��ȵ��յ������־
	int nIndex;//ɨ��������
	//ZBuffer
	double A, B, C, D;
	int Width, Height;
	double** zBuffer;//����ȳ���
};

