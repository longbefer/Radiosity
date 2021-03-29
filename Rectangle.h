#pragma once
#include "GameObject.h"

namespace User {
    class Rectangle :
        public GameObject
    {
    public:
        Rectangle(const Point3d& point, const Vector3d& a, const Vector3d& b, const Normal& n) :normal(n) {
            this->point[0] = point;
            this->point[1] = point + a;
            this->point[2] = point + a + b;
            this->point[3] = point + b;

            // 获取最小、最大的值，用于包围盒
            Point3d minPoint(1.0E10, 1.0E10, 1.0E10), maxPoint(-1.0E10, -1.0E10, -1.0E10);
            for (int i = 0; i < sizeof(point)/sizeof(Point3d); ++i) {
                minPoint.x = std::min(minPoint.x, this->point[i].x);
                minPoint.y = std::min(minPoint.y, this->point[i].y);
                minPoint.z = std::min(minPoint.z, this->point[i].z);

                maxPoint.x = std::max(maxPoint.x, this->point[i].x);
                maxPoint.y = std::max(maxPoint.y, this->point[i].y);
                maxPoint.z = std::max(maxPoint.z, this->point[i].z);
            }
            this->box = BBox(minPoint - Point3d(1.0e-4, 1.0e-4, 1.0e-4),
                maxPoint + Point3d(1.0e-4, 1.0e-4, 1.0e-4));
            this->name = "矩形";
        }

        virtual bool IsHit(const Ray&, double&) const;
        virtual std::vector<Patch> ObjectPatch();

        virtual void SetTransformMatrix(const Matrix& matrix) {
            GameObject::SetTransformMatrix(matrix);
            for (size_t i = 0; i < sizeof(point) / sizeof(Point3d); ++i)
                point[i] = transformMatrix * point[i];
            this->normal = transformMatrix * normal;
        }

        virtual const BBox GetBoundingBox() {
            // 获取最小、最大的值，用于包围盒
            Point3d minPoint(1.0E10, 1.0E10, 1.0E10), maxPoint(-1.0E10, -1.0E10, -1.0E10);
            for (int i = 0; i < 4; ++i) {
                minPoint.x = std::min(minPoint.x, this->point[i].x);
                minPoint.y = std::min(minPoint.y, this->point[i].y);
                minPoint.z = std::min(minPoint.z, this->point[i].z);

                maxPoint.x = std::max(maxPoint.x, this->point[i].x);
                maxPoint.y = std::max(maxPoint.y, this->point[i].y);
                maxPoint.z = std::max(maxPoint.z, this->point[i].z);
            }
            this->box = BBox(minPoint - Point3d(1.0e-4, 1.0e-4, 1.0e-4),
                maxPoint + Point3d(1.0e-4, 1.0e-4, 1.0e-4));
            return this->box;
        }

    private:
        void DividenPatch(std::vector<Patch>&, const Point3d[4], const int&);

    protected:
        Point3d point[4];
        Normal normal = XNormal;
    };

}
