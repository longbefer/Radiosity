#include "pch.h"
#include "Scene.h"
#include "Paint.h"

#include "Box.h"
#include "Rectangle.h"
#include "Triangle.h"
#include "Light.h"
#include "Teapot.h"
#include "Sphere.h"
//#include "ObjFile.h"
#include "SmoothRectangle.h"
#include "SmoothTriangle.h"

#include "resource.h"

Scene::Scene()
{
	// 绑定hitObject函数
	radiation.hitObject = std::bind(&Scene::HitObject, this, std::placeholders::_1);
	Build();
}

Scene::~Scene()
{
	size_t n = gameObject.size();
	for (size_t i = 0; i < n; ++i) {
		if (gameObject[i]) {
			delete gameObject[i];
			gameObject[i] = nullptr;
		}
	}
	gameObject.erase(gameObject.begin(), gameObject.end());
}

void Scene::Init()
{
	bFinish = false;
	radiation.patchs.clear(); // for control panel 便于重新绘制
	CollectPatch();
	radiation.Init();
}

void Scene::Rendered()
{
	radiation.Rendered(16);
	bFinish = true;
}

#ifdef USE_CAMERA
void Scene::Draw(Paint& p, std::unique_ptr<BYTE[]>& buff)
{
	radiation.Draw(p, buff);
}
#else
void Scene::Draw(CDC* pDC)
{
	radiation.Draw(pDC);
}
#endif

double Scene::HitObject(const Ray& ray) const
{
	double t, tmin = 1.0E10;

	for (auto& obj : gameObject) {
		if (obj && obj->IsHit(ray, t) && (t < tmin)) {
			tmin = t;
		}
	}

	return tmin;
}

void Scene::Build()
{
	Light* light = new Light(Point3d(-100, 275, -100), Point3d(100, 295, 100));
	light->SetColor(White);
	light->SetEmmision(10*White);
	light->SetRecursionTimes(3);

	Color gray(0.6, 0.6, 0.6, 1.0);
	// 左边
	User::Rectangle* box = new User::Rectangle(Point3d(-300, -300, -300), Vector3d(0, 600, 0, 0.0), Vector3d(0, 0, 600, 0.0), Normal(1, 0, 0, 0));
	box->SetColor(Red);
	box->SetEmmision(Black);
	box->SetRecursionTimes(5);
	// 下面
	User::Rectangle* box1 = new User::Rectangle(Point3d(-300, -300, -300), Vector3d(600, 0, 0, 0.0), Vector3d(0, 0, 600, 0.0), Normal(0, 1, 0, 0));
	box1->SetColor(gray);
	box1->SetEmmision(Black);
	box1->SetRecursionTimes(5);
	// 后面
	User::Rectangle* box2 = new User::Rectangle(Point3d(-300, -300, -300), Vector3d(0, 600, 0, 0.0), Vector3d(600, 0, 0, 0.0), Normal(0, 0, 1, 0));
	box2->SetColor(gray);
	box2->SetEmmision(Black);
	box2->SetRecursionTimes(5);
	// 上面
	User::Rectangle* box3 = new User::Rectangle(Point3d(300, 300, 300), Vector3d(-600, 0, 0, 0.0), Vector3d(0, 0, -600, 0.0), Normal(0, -1, 0, 0));
	box3->SetColor(gray);
	box3->SetEmmision(Black);
	box3->SetRecursionTimes(5);
	// 右面
	User::Rectangle* box4 = new User::Rectangle(Point3d(300, 300, 300), Vector3d(0, -600, 0, 0.0), Vector3d(0, 0, -600, 0.0), Normal(-1, 0, 0, 0));
	box4->SetColor(Green);
	box4->SetEmmision(Black);
	box4->SetRecursionTimes(5);

	//Box* box5 = new Box(Point3d(-100, -300, -100), Point3d(100, -50, 100));
	//box5->SetColor(Color(0.5, 0.5, 0.0, 0.0));
	//box5->SetEmmision(Black);
	//box5->SetRecursionTimes(3);
	// -------------------------------------------------------------------------
	//Triangle* box5 = new Triangle(Point3d(-200, -200, -200), Point3d(200, -200, -200), Point3d(-200, -200, 200));
	//box5->SetNormal(YNormal);
	//box5->SetColor(Color(1.0, 1.0, 0));
	//box5->SetEmmision(Black);
	//box5->SetRecursionTimes(2);
	// --------------------------------------------------------------------------
	//User::Rectangle* box5 = new User::Rectangle(Point3d(-200, -200, -200), Vector3d(400, 0, 0, 0, 0.0), Vector3d(0, 0, 400, 0, 0.0), Normal(0, 1, 0, 0));
	//box5->SetColor(Color(1.0, 0.0, 1.0));
	//box5->SetEmmision(Black);
	//box5->SetRecursionTimes(5);
	// --------------------------------------------------------------------------
	//Sphere* box5 = new Sphere(Point3d(0, -200, 0), 100);
	//box5->SetColor(gray);
	//box5->SetEmmision(Black);
	//box5->SetRecursionTimes(4);
	// 将box旋转（注意其使用应该是先使用含Set的变换之后不允许使用含Set的变换了）
	//Matrix rotateMatrix = rotateMatrix.SetRotate(0, Matrix::AXIS::Y).Scale(1.0, 1.0, 1.0).Translate(0, -50, 0);
	//box5->SetTransformMatrix(rotateMatrix);
	// --------------------------------------------------------------------------
	//SmoothTriangle* box5 = new SmoothTriangle(Point3d(-200, -200, -200), Point3d(200, -200, -200), Point3d(-200, -200, 200));
	//box5->SetNormal(YNormal, YNormal, YNormal);
	//box5->SetColor(Color(1.0, 1.0, 0.0));
	//box5->SetEmmision(Black);
	//box5->SetRecursionTimes(3);
	// -------------------------------------------------------------------------------
	SmoothRectangle* box5 = new SmoothRectangle(Point3d(-200, -200, -290), Vector3d(400, 0, 0, 0.0), Vector3d(0, 400, 0, 0.0), Normal(0, 0, 1, 0.0));
	box5->SetNormal(ZNormal, ZNormal, ZNormal, ZNormal);
	box5->SetColor(White);
	box5->SetEmmision(Black);
	box5->SetRecursionTimes(4);
	box5->SetTexture();
	box5->SetTextureImage(IDB_BITMAP3);
	//box5->SetTransformMatrix(rotateMatrix);
	//box5->SetTextureImage("res\\3.bmp");
	//box5->SetTextureImage("D:\\User\\Picture\\Saved Pictures\\Temp\\888.jpg");

	// 茶壶
	Teapot* t = new Teapot();
	t->SetColor(Color(1.0, 1.0, 0.0, 0.0));
	t->SetEmmision(Black);
	t->SetRecursionTimes(4);
	Matrix scaleMatrix = scaleMatrix.SetScale(50, 50, 50).Translate(0, -200, 0).Rotate(45, Matrix::AXIS::Y);
	t->SetTransformMatrix(scaleMatrix);

	//double r = 300.0;
	//double m = 0.5523;
	//Point3d P3[16];
	//P3[0].x = -250.; P3[0].y = -80.; P3[0].z = 200.;//P00
	//P3[1].x = -50.; P3[1].y = 100.; P3[1].z = 150.;//P01
	//P3[2].x = 50.; P3[2].y = 100.; P3[2].z = 150.;//P02
	//P3[3].x = 250.; P3[3].y = -80.; P3[3].z = 200.;//P03

	//P3[4].x = -150.; P3[4].y = 150.; P3[4].z = 50.;//P10
	//P3[5].x = -50.; P3[5].y = 150.; P3[5].z = 50.;//p11
	//P3[6].x = 50.; P3[6].y = 150.; P3[6].z = 50.;//p12
	//P3[7].x = 150.; P3[7].y = 150.; P3[7].z = 50.;//p13

	//P3[8].x = -150.; P3[8].y = 150.; P3[8].z = -50.;//P20
	//P3[9].x = -50.; P3[9].y = 150.; P3[9].z = -50.;//P21
	//P3[10].x = 50.; P3[10].y = 150.; P3[10].z = -50.;//P22
	//P3[11].x = 150.; P3[11].y = 150.; P3[11].z = -50.;//P23

	//P3[12].x = -350.; P3[12].y = -120.; P3[12].z = -200.;//P30
	//P3[13].x = -50.; P3[13].y = 100.; P3[13].z = -150.;//P31
	//P3[14].x = 50.; P3[14].y = 100.; P3[14].z = -150.;//P32
	//P3[15].x = 250.; P3[15].y = -100.; P3[15].z = -200.;//P33	

	//BezierPatch* bezier = new BezierPatch();
	//bezier->SetColor(Blue);
	//bezier->SetEmmision(Black);
	//bezier->SetRecursionTimes(3);
	//bezier->SetTransformMatrix(rotateMatrix);
	//bezier->ReadControlPoint(P3);
	//bezier->Accelerate(); // 加速

	// -----------------------------------------------------------------------------------

	//ObjFile* obj = new ObjFile("res/obj/1/box_stack.obj");
	//obj->SetTransformMatrix(rotateMatrix);
	//obj->AddObject();

	gameObject.push_back(box);
	gameObject.push_back(box1);
	gameObject.push_back(box2);
	gameObject.push_back(box3);
	gameObject.push_back(box4);
	//gameObject.push_back(bezier);
	gameObject.push_back(box5);
	//gameObject.push_back(t);
	//gameObject.push_back(obj);
	gameObject.push_back(light);
}

void Scene::CollectPatch()
{
	// 复制面片
	for (auto& obj : gameObject) {
		auto temp = std::move(obj->ObjectPatch());
		radiation.patchs.insert(radiation.patchs.end(), temp.begin(), temp.end());
	}
}
