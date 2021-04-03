#pragma once
#include "GameObject.h"
class Sphere :
    public GameObject
{
public:
    Sphere(const Point3d& p, const double& r = 1.0) : center(p), r(r) {
        N1 = divide;
        N2 = 2 * divide;
        this->name = "球";
    };

public:
    virtual bool IsHit(const Ray&, double&) const;
    virtual std::vector<Patch> ObjectPatch();

    virtual void SetTransformMatrix(const Matrix& matrix) {
        GameObject::SetTransformMatrix(matrix);
        center = this->transformMatrix * center;
        //this->r = transformMatrix.m[0][0] * r; // 只对设置Scale缩放x有效
        //ReadVertex();
    }
    virtual const BBox GetBoundingBox() {
        this->box = BBox(center - Point3d(r, r, r), center + Point3d(r, r, r));
        return this->box;
    }

protected:
    void ReadVertex();

protected:
    double r;
    Point3d center;

private:
    int N1, N2;
    std::unique_ptr<Point3d[]> P;
};

