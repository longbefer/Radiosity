#pragma once
#include "GameObject.h"
#include "Grid.h"
#include "OBJ_Loader.h"
#include "Triangle.h"
#include "Rectangle.h"
#include "SmoothRectangle.h"

class ObjFile :
    public GameObject
{
public:
    ObjFile(std::string patch) {
        if (!obj.LoadFile(patch))
            return;
    }
public:
    static bool IsLoaded(std::string path) {
        if (path.substr(path.size() - 4, 4) != ".obj")
            return false;
        std::ifstream file(path, std::ios::in); // 只读的方式打开
        if (!file.is_open()) return false;
        file.close();
        return true;
    }
    virtual bool IsHit(const Ray&ray, double&tmin) const {
        return grid.IsHit(ray, tmin);
    }
    virtual std::vector<Patch> ObjectPatch() {
        return grid.ObjectPatch();
    }
    virtual const BBox GetBoundingBox() { 
        this->box = grid.GetBoundingBox();
        return this->box;
    }
    virtual std::string GetName() const {
        return "*.Obj物体";
    }
    // 这里的设计模式问题（必须在AddObject前使用SetTransformMatrix）
    virtual void SetTransformMatrix(const Matrix& matrix) {
        GameObject::SetTransformMatrix(matrix);
        //if (obj.LoadedVertices)
        //for (auto& v : obj.LoadedVertices) {
        //    // 更改点
        //    Point3d p(v.Position.X, v.Position.Y, v.Position.Z);
        //    p = transformMatrix * p;
        //    v.Position = objl::Vector3(static_cast<float>(p.x), static_cast<float>(p.y), static_cast<float>(p.z));

        //    // 更改向量
        //    Normal n(v.Normal.X, v.Normal.Y, v.Normal.Z, 0.0);
        //    n = (transformMatrix * n).Normalized();
        //    v.Normal = objl::Vector3(static_cast<float>(n.x), static_cast<float>(n.y), static_cast<float>(n.z));
        //}
    }
    // 向grid中添加物体
    void AddObject() {
        auto& objs = obj.LoadedMeshes;
        for (auto& o : objs) { // 遍历每一个物体
            Point3d p3[3];
            Normal n3[3];
            Texture t3[3];
            for (size_t i = 0; i < o.Indices.size(); ++i) {
                p3[i % 3ULL] = Point3d(o.Vertices[o.Indices[i]].Position.X,
                    o.Vertices[o.Indices[i]].Position.Y,
                    o.Vertices[o.Indices[i]].Position.Z);
                n3[i % 3ULL] = Normal(o.Vertices[o.Indices[i]].Normal.X,
                    o.Vertices[o.Indices[i]].Normal.Y,
                    o.Vertices[o.Indices[i]].Normal.Z, 0.0);
                t3[i % 3ULL] = Texture(o.Vertices[o.Indices[i]].TextureCoordinate.X,
                    o.Vertices[o.Indices[i]].TextureCoordinate.Y);

                if (i % 3ULL == 2) {
                    Triangle* t1 = new Triangle(p3[0], p3[1], p3[2]);
                    t1->SetNormal(n3[0], n3[1], n3[2]);
                    auto& objC = o.MeshMaterial.Kd;
                    t1->SetColor(Color(objC.X, objC.Y, objC.Z));
                    t1->SetEmmision(Black);
                    t1->SetRecursionTimes(0);
                    t1->SetTransformMatrix(this->transformMatrix);
                    grid.AddObject(t1);
                }
            }
            //User::Rectangle* t = new User::Rectangle(p4[0], p4[1] - p4[0], p4[2] - p4[0], n4[0] + n4[1] + n4[2] + n4[3]);
            // t->SetColor(Color(objC.X, objC.Y, objC.Z));
            // t->SetEmmision(Black);
            // t->SetRecursionTimes(0);
            // grid.AddObject(t);

            //Triangle* t2 = new Triangle(p4[0], p4[2], p4[3]);
            //t2->SetNormal(n4[0], n4[2], n4[3]);
            //t2->SetColor(Color(objC.X, objC.Y, objC.Z));
            //t2->SetEmmision(Black);
            //t2->SetRecursionTimes(0);
            //grid.AddObject(t2);

        }
    }

protected:
    Grid grid;
    objl::Loader obj;
};

