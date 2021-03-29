#pragma once
#include "GameObject.h"

/*
* ��������ʹ�������������ɶ������
*/
class Compound
	:public GameObject
{
public:
	Compound() { this->name = "�����"; };
	virtual ~Compound() {
		objects.erase(objects.begin(), objects.end());
	}

	// for gameObject;
	virtual bool IsHit(const Ray&ray, double&tmin) const {
		bool bHit = false;
		double t, tHit = 1.0E10;
		for (auto& obj : objects) {
			if (obj->IsHit(ray, t) && (t < tHit)) {
				bHit = true;
				tmin = t;
			}
		}
		return bHit;
	}
	virtual std::vector<Patch> ObjectPatch() {
		std::vector<Patch> patchs;
		for (auto& obj : objects) {
			auto temp = std::move(obj->ObjectPatch());
			patchs.insert(patchs.end(), temp.begin(), temp.end());
		}
		return patchs;
	}

	virtual const BBox GetBoundingBox() {

		Point3d minPoint(1.0E10, 1.0E10, 1.0E10), maxPoint(-1.0E10, -1.0E10, -1.0E10);
		// ��ȡ�����С�İ�Χ��
		for (auto& obj : objects) {
			BBox box = obj->GetBoundingBox();
			minPoint.x = std::min(box.point[0].x, minPoint.x);
			minPoint.y = std::min(box.point[0].y, minPoint.y);
			minPoint.z = std::min(box.point[0].z, minPoint.z);

			maxPoint.x = std::max(box.point[1].x, maxPoint.x);
			maxPoint.y = std::max(box.point[1].y, maxPoint.y);
			maxPoint.z = std::max(box.point[1].z, maxPoint.z);
		}
		this->box = BBox(minPoint - Point3d(1.0e-4, 1.0e-4, 1.0e-4, 0.0),
						 maxPoint + Point3d(1.0e-4, 1.0e-4, 1.0e-4, 0.0));
		return this->box;
	}

	virtual void SetTransformMatrix(const Matrix& matrix) {
		GameObject::SetTransformMatrix(matrix);
		for (auto& obj : objects)
			obj->SetTransformMatrix(matrix);
	}

	// ע�⣺���������ͬ��ָ��ͬһ�����󣩵�����
	// �ᵼ��ɾ����ɾ���Ķ��󱨴�ͬʱҲ���ظ�
	// ���ø�����ı任����
	void AddObject(GameObject* obj) {
		this->objects.push_back(obj);
	}
	void DeleteObjects() {
		size_t n = objects.size();
		for (size_t i = 0; i < n; ++i) {
			if (objects[i]) {
				delete objects[i];
				objects[i] = nullptr;
			}
		}
	}

protected:
	std::vector<GameObject*> objects;
};

