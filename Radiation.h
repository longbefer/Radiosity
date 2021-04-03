#pragma once
#include "Patch.h"
// �Ƿ�ʹ�ö���������
#define USERADIATIONSMOOTH 1
// ע�⣬�������ֶ���Ϊ��ͬ�㷨�Ķ��壬�벻Ҫͬʱ��
// -----------------------------------
//#define Gauss_Siedel     // Gauss Siedel����
//#define Normal_Result    // ��һ�㷽��
//#define Southwell        // ����
// -----------------------------------
class Radiation
{
#if !USERADIATIONSMOOTH
public:
	~Radiation() {
		size_t n = patchs.size();
#ifndef Southwell
		if (factors) {
			for (size_t i = 0; i < n; ++i) {
				if (factors[i]) {
					delete[] factors[i];
					factors[i] = nullptr;
				}
			}
			delete factors;
			factors = nullptr;
		}
#endif
#ifdef Southwell
		if (increase) {
			delete[] increase;
			increase = nullptr;
		}
#endif
	}
public:
	void Init();
	void Rendered(size_t n);
	void Draw(CDC* pDC);

private:
	// ��������
	void Calculate();

	// ��ȡ��״����
	double GetViewFactor(const Patch&, const Patch&);

public:
	std::function<double(const Ray&)> hitObject;   // hit the object

	std::vector<Patch> patchs;                     // all of sence patchs

#ifdef Southwell
	//double* factors = nullptr;                   // view factor
	Color avgReflect = Black;                      // ���ڷ������
	double sumArea = 0.0;                          // ���ڷ������

	Color* increase = nullptr;                     // ����������������������㷨��
#else 
	double** factors = nullptr;
#endif
	bool bFinish = false;
#else
public:
#ifdef Southwell
	~Radiation() {
		if (increase) {
			delete[] increase;
			increase = nullptr;
		}
	}
#endif
	void Init();
	void Rendered(size_t n);
	void Draw(CDC* pDC);

private:
	// ��������
	void Calculate();

	double GetViewFactor(const Patch&, const Patch&, const size_t&, const size_t&);

public:
	std::function<double(const Ray&)> hitObject;   // hit the object

	std::vector<Patch> patchs;                     // all of sence patchs

	bool bFinish = false;
#ifdef Southwell
	Color* increase = nullptr;
#endif
#endif
};

