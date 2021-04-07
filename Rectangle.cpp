#include "pch.h"
#include "Rectangle.h"
#include "Patch.h"
namespace User {
    bool Rectangle::IsHit(const Ray& ray, double& tmin) const
    {
        double t = Dot((point[0] - ray.o), normal) / Dot(ray.d, normal);
        // 光线与平面平行或反向延长线
        if (t <= kEpsilon) return false;

        Point3d p = ray.o + t * ray.d;
        Vector3d d = p - point[0];

        const double kLEpsilon = -kEpsilon; // 保证0点内的内容可以落在该范围内
        // 是否存在由向量a、b构成的矩形里
        auto a = this->point[1] - this->point[0];
        double ddota = Dot(d, a);
        if (ddota < kLEpsilon || ddota > a.Length_Square())
            return false;

        auto b = this->point[3] - this->point[0];
        double ddotb = Dot(d, b);
        if (ddotb < kLEpsilon || ddotb > b.Length_Square())
            return false;

        tmin = t;
        return true;
    }

    std::vector<Patch> Rectangle::ObjectPatch()
    {
        std::vector<Patch> patchs;
        DividenPatch(patchs, point, divide);
        return patchs;
    }

    void Rectangle::DividenPatch(std::vector<Patch>& patchs, const Point3d patchPoint[4], const int& t)
    {
        if (t <= 0) {
            Patch patch;
            patch.obj = this;
            normal.Normalize();
            patch.normal = normal;
            for (int i = 0; i < 4; ++i) {
                CP3 patchP;
                patchP.position = patchPoint[i];
                patchP.normal = normal;
                //patchP.texture = patchPoint[i].texture;
                patch.vertices.push_back(patchP);
            }
            patchs.push_back(patch);
            return;
        }
        // 递归划分
        Point3d div[4];

        div[0] = patchPoint[0];
        div[1] = (patchPoint[1] + patchPoint[0]) / 2.0;
        div[2] = (patchPoint[0] + patchPoint[2]) / 2.0;
        div[3] = (patchPoint[0] + patchPoint[3]) / 2.0;
        DividenPatch(patchs, div, t - 1);

        div[0] = (patchPoint[1] + patchPoint[0]) / 2.0;
        div[1] = patchPoint[1];
        div[2] = (patchPoint[1] + patchPoint[2]) / 2.0;
        div[3] = (patchPoint[0] + patchPoint[2]) / 2.0;
        DividenPatch(patchs, div, t - 1);

        div[0] = (patchPoint[0] + patchPoint[2]) / 2.0;
        div[1] = (patchPoint[1] + patchPoint[2]) / 2.0;
        div[2] = patchPoint[2];
        div[3] = (patchPoint[2] + patchPoint[3]) / 2.0;
        DividenPatch(patchs, div, t - 1);

        div[0] = (patchPoint[0] + patchPoint[3]) / 2.0;
        div[1] = (patchPoint[0] + patchPoint[2]) / 2.0;
        div[2] = (patchPoint[2] + patchPoint[3]) / 2.0;
        div[3] = patchPoint[3];
        DividenPatch(patchs, div, t - 1);
    }
}