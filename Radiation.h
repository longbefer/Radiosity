#pragma once
#include "Patch.h"
#include "Camera.h"
#include "Paint.h"
// �Ƿ�ʹ�ö���������
#define USERADIATIONSMOOTH 1
// ע�⣬�������ֶ���Ϊ��ͬ�㷨�Ķ��壬�벻Ҫͬʱ��
// -----------------------------------
//#define Gauss_Siedel     // Gauss Siedel����
//#define Normal_Result    // ��һ�㷽��
#define Southwell        // ����
// -----------------------------------
class Radiation
{
#if !USERADIATIONSMOOTH
public:
	~Radiation() {
		DeleteMatrix();
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

	// ɾ������
	void DeleteMatrix() {
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
		DeleteMatrix();
	}
#endif
	void Init();
	void Rendered(size_t n);
#ifdef USE_CAMERA
	void Draw(Paint&, std::unique_ptr<BYTE[]>&);
#else
	void Draw(CDC* pDC);
#endif

private:
	// ��������
	void Calculate();

	double GetViewFactor(const Patch&, const Patch&, const size_t&, const size_t&);

#ifdef Southwell
	void DeleteMatrix() {
		if (increase) {
			delete[] increase;
			increase = nullptr;
		}
	}
#endif

public:
	std::function<double(const Ray&)> hitObject;   // hit the object

	std::vector<Patch> patchs;                     // all of sence patchs

	bool bFinish = false;
#ifdef Southwell
	Color* increase = nullptr;
#endif // ʹ�����󾫵��㷨ʵ��
#endif // ʹ���Ľǵ�������
#ifdef USE_CAMERA
	void SetCamera(Camera& p) {
		this->camera = &p;
	}

	Camera* camera = nullptr;
#endif
};

