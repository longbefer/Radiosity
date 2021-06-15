#pragma once
#include "Patch.h"

class Paint
{
public:
	Paint();
	virtual ~Paint();
	void SetPoint(CP3 P0, CP3 P1, CP3 P2);//���������㹹��������
	void DeleteBuffer(); // ɾ��Buffer
#ifdef USE_CAMERA
	void GouraudShading(std::unique_ptr<BYTE[]>&, const Patch&);//���������
	bool OutOfDrawArea() {
		// ������������ڷ�Χ�ڣ���ȡ���������εĻ��ƣ�ע�⣺�˷���������ȷ�ü��������Խ���������ʾ�ü������
		return ((((point0.position.x <= .0 || point0.position.x >= static_cast<double>(Width)) ||
			(point0.position.y <= .0 || point0.position.y >= static_cast<double>(Height))) &&
			((point1.position.x <= .0 || point1.position.x >= static_cast<double>(Width)) ||
				(point1.position.y <= .0 || point1.position.y >= static_cast<double>(Height))) &&
			((point2.position.x <= .0 || point2.position.x >= static_cast<double>(Width)) ||
				(point2.position.y <= .0 || point2.position.y >= static_cast<double>(Height))))
			// ������z�ķ�Χ�ڣ���ʹxy�ڷ�Χ��Ҳ��Ҫ��������û�й淶��-1.0 ~ 1.0����Ҫע�͵���������
			|| ((point0.position.z < -1.0 || point0.position.z > 1.0)) &&
			(point1.position.z < -1.0 || point1.position.z > 1.0) &&
			(point2.position.z < -1.0 || point2.position.z > 1.0));
	}
#else
	void GouraudShading(CDC* pDC, const Patch&);//���������
#endif
	void EdgeFlag(CPoint3 PStart, CPoint3 PEnd, BOOL bFeature);//�߱��
	void InitDeepBuffer(int Width, int Height, double Depth);
	Normal  LinearInterp(double t, double tStart, double tEnd, Normal vStart, Normal vEnd);//��ʸ�����Բ�ֵ
	Texture LinearInterp(double t, double tStart, double tEnd, Texture vStart, Texture vEnd);
	//�������Բ�ֵ
	CRGB LinearInterp(double t, double mStart, double mEnd, CRGB cStart, CRGB cEnd);//��ɫ���Բ�ֵ
	void SortVertex(void);//�����ζ�������
private:
#ifdef USE_CAMERA
	CP3 point0, point1, point2;//�����ε���������
#else
	CPoint3 point0, point1, point2;//�����ε���������
#endif
	CPoint2* pLeft; //��ȵ���������־
	CPoint2* pRight;//��ȵ��յ������־
	int nIndex;//ɨ��������
	//ZBuffer
	double A, B, C, D;
	int Width, Height;
	double** zBuffer;//����ȳ���
};

