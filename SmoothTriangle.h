#pragma once
#include "Triangle.h"
#include "Patch.h"

class SmoothTriangle :
    public Triangle
{
public:
    void SetTexture(const Texture&t1, const Texture&t2, const Texture&t3) {
        texture[0] = t1; texture[1] = t2; texture[2] = t3;
    }

    virtual std::vector<Patch> ObjectPatch() {
        std::vector<Patch> patchs;
        P3 point3[3];
        for (size_t i = 0; i < 3; ++i) {
            point3[i].position = this->vertices[i];
            point3[i].normal = normal[i];
            point3[i].texture = texture[i];
        }
        SmoothDividenPatch(patchs, point3, divide);
        return patchs;
    }

private:
    void SmoothDividenPatch(std::vector<Patch>&patchs, P3 point3[3], const int&n) {
        if (n == 0) {
            Patch patch;
            patch.obj = this;
            patch.normal = (normal[0] + normal[1] + normal[2]) / 3.0;
            P3 patchPoint[3];
            for (size_t i = 0; i < 3; ++i) {
                patchPoint[i].position = point3[i].position;
                patchPoint[i].normal = point3[i].normal; 
                patchPoint[i].texture = point3[i].texture;
                patch.vertices.push_back(patchPoint[i]);
            }
            patchs.push_back(patch);
            return;
        }
        P3 mid[3];
        mid[0].position = (point3[0].position + point3[1].position) / 2.0;
        mid[0].normal = (point3[0].normal + point3[1].normal) / 2.0;
        mid[0].texture = (point3[0].texture + point3[1].texture) / 2.0;

        mid[1].position = (point3[0].position + point3[2].position) / 2.0;
        mid[1].normal = (point3[0].normal + point3[2].normal) / 2.0;
        mid[1].texture = (point3[0].texture + point3[2].texture) / 2.0;

        mid[2].position = (point3[1].position + point3[2].position) / 2.0;
        mid[2].normal = (point3[1].normal + point3[2].normal) / 2.0;
        mid[2].texture = (point3[1].texture + point3[2].texture) / 2.0;

        P3 eachPoint[4][3];
        eachPoint[0][0] = point3[0];
        eachPoint[0][1] = mid[0];
        eachPoint[0][2] = mid[2];
        SmoothDividenPatch(patchs, eachPoint[0], n - 1);
        eachPoint[1][0] = mid[0];
        eachPoint[1][1] = point3[1];
        eachPoint[1][2] = mid[1];
        SmoothDividenPatch(patchs, eachPoint[1], n - 1);
        eachPoint[2][0] = mid[2];
        eachPoint[2][1] = mid[1];
        eachPoint[2][2] = point3[2];
        SmoothDividenPatch(patchs, eachPoint[2], n - 1);
        eachPoint[3][0] = mid[0];
        eachPoint[3][1] = mid[1];
        eachPoint[3][2] = mid[2];
        SmoothDividenPatch(patchs, eachPoint[3], n - 1);
    }

protected:
    Texture texture[3];
};

