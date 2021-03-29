#pragma once
#include "BBox.h"
class Patch;
/*
* ���ζ���������֪����ĳ�����
*/
class GameObject
{
public:
	GameObject() : name("���ζ���") {};
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
		// ���ñ任����ʱ���뼴ʹ���°�Χ�С��������
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

