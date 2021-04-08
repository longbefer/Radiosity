#pragma once
#include "GameObject.h"
#include "Radiation.h"

// ����, ���, ������, ��, ������, Bezier����
enum class GameObjections { Rectangle, Teapot, Triangle, Sphere, Box, BezierPatch };

class Scene
{
public:
	Scene();
	virtual ~Scene();

	void Init();
	void Rendered();
	void Draw(CDC*);

	double HitObject(const Ray&)const;

	// for ControlPanel
	void AddObject(GameObject*obj) {
		this->gameObject.push_back(obj);
	}
	std::vector<GameObject*>& GetObjects() {
		return gameObject;
	}
	auto& GetRadiationPatchs() {
		return radiation.patchs;
	}

private:
	void Build();
	void CollectPatch();

public:
	bool bFinish = true; // ����ָʾ��Ⱦ�Ƿ����
private:
	std::vector<GameObject*> gameObject;

	Radiation radiation;
};

