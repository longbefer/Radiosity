#pragma once
#include "GameObject.h"
#include "Radiation.h"
#include "Camera.h"
#include "Paint.h"

// 矩形, 茶壶, 三角形, 球, 立方体, Bezier曲面
enum class GameObjections { Rectangle, Teapot, Triangle, Sphere, Box, BezierPatch };

class Scene
{
public:
	Scene();
	virtual ~Scene();

	void Init();
	void Rendered();
#ifdef USE_CAMERA
	void Draw(Paint&, std::unique_ptr<BYTE[]>&);
#else
	void Draw(CDC*);
#endif

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
#ifdef USE_CAMERA
	void SetCamera(Camera& p) {
		radiation.SetCamera(p);
	}
#endif

private:
	void Build();
	void CollectPatch();

public:
	bool bFinish = true; // 用于指示渲染是否结束
private:
	std::vector<GameObject*> gameObject;

	Radiation radiation;
};

