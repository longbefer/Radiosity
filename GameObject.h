#pragma once
#include "BBox.h"
class Patch;
/*
* 几何对象，所有已知对象的抽象类
*/
class GameObject
{
public:
	GameObject() : name("几何对象") {};
	virtual ~GameObject() {};

	virtual bool IsHit(const Ray&, double&) const = 0;
	virtual std::vector<Patch> ObjectPatch() = 0;

	// radiosity
	void SetColor(const Color& c) { this->reflectance = c; }
	void SetEmmision(const Color& c) { this->emmision = c; }

	const Color GetEmmision() const { return emmision; }
	const Color GetReflectance() const { return reflectance; }

	// ray trace
	virtual const BBox GetBoundingBox() { return box; }

	// for recursion
	void SetRecursionTimes(const int& n) { this->divide = n; }
	//virtual size_t GetPatchsNumber() const = 0;

	// for transform
	virtual void SetTransformMatrix(const Matrix& matrix) {
		// 设置变换矩阵时，请即使更新包围盒、点或向量
		this->transformMatrix = matrix;
		//this->box.SetTransformMatrix(transformMatrix);
	}

	const Matrix GetTransformMatrix() const {
		return this->transformMatrix;
	}

	// for texture;
	void SetTextureImage(UINT nIDResource) {
		img = std::make_unique<Image>();
		this->img->SetResource(nIDResource);
	}
	//virtual 
	Color GetTextureImagePixel(const Texture&t) {
		if (img == nullptr) return this->reflectance;
		/*检测图片的边界，防止越界*/
		size_t u = static_cast<size_t>(t.u * img->bmp.bmWidth), v = static_cast<size_t>(t.v * img->bmp.bmHeight);
		if (u < 0ULL) u = 0ULL; if (v < 0ULL) v = 0ULL;
		if (u > img->bmp.bmWidth - 1ULL) 	u = img->bmp.bmWidth - 1ULL;
		if (v > img->bmp.bmHeight - 1ULL)	v = img->bmp.bmHeight - 1ULL;
		/*查找对应纹理空间的颜色值*/
		v = img->bmp.bmHeight - 1ULL - v;
		size_t position = v * img->bmp.bmWidthBytes + 4ULL * u;//颜色分量位置
		COLORREF color = RGB(img->image.get()[position + 2ULL], img->image.get()[position + 1ULL], img->image.get()[position]);//获取颜色值
		return  CRGB(GetRValue(color) / 255.0, GetGValue(color) / 255.0, GetBValue(color) / 255.0);
	}

	Image* GetImage() {
		return img.get();
	}

	virtual std::string GetName() const {
		return name;
	}

protected:
	// radiosity
	Color emmision = Black;
	Color reflectance = Black;

	// ray trace
	BBox box;

	// for dividen
	int divide = 3;

	// for transform
	Matrix transformMatrix;

	// for texture;
	std::unique_ptr<Image> img;

	// for control panel
	std::string name;
};

