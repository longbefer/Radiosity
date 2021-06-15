#pragma once
#include "Patch.h"
#include "Camera.h"
#include "Paint.h"
// 是否使用多点采样（雾）
#define USERADIATIONSMOOTH 1
// 注意，以下三种定义为不同算法的定义，请不要同时打开
// -----------------------------------
//#define Gauss_Siedel     // Gauss Siedel方法
//#define Normal_Result    // 解一般方程
#define Southwell        // 逐步求精
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
	// 计算辐射度
	void Calculate();

	// 获取形状因子
	double GetViewFactor(const Patch&, const Patch&);

	// 删除矩阵
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
	Color avgReflect = Black;                      // 用于泛光计算
	double sumArea = 0.0;                          // 用于泛光计算

	Color* increase = nullptr;                     // 辐射度增量（仅用于逐步求精算法）
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
	// 计算辐射度
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
#endif // 使用逐步求精的算法实现
#endif // 使用四角点来绘制
#ifdef USE_CAMERA
	void SetCamera(Camera& p) {
		this->camera = &p;
	}

	Camera* camera = nullptr;
#endif
};

