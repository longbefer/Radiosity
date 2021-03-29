#pragma once
#include "Rectangle.h"
#include "Patch.h"
/*
* SmoothRectangle并非用来设置向量，而用来设置纹理，纹理
* 只能由Smooth物体来添加
*/
class SmoothRectangle :
    public User::Rectangle
{
public:
    void SetTexture(const Texture& t1, const Texture& t2, const Texture& t3, const Texture&t4) {
        texture[0] = t1; texture[1] = t2; texture[2] = t3; texture[3] = t4;
    }
    virtual std::vector<Patch> ObjectPatch() {
        std::vector<Patch> patchs;
        P3 point4[4];
        for (size_t i = 0; i < sizeof(point) / sizeof(Point3d); ++i) {
            point4[i].position = point[i];
            point4[i].normal = normal;
            point4[i].texture = texture[i];
        }
        SmoothDividenPatch(patchs, point4, divide);
        return patchs;
    }

private:
    void SmoothDividenPatch(std::vector<Patch>&patchs, const P3 point4[4], const int&t) {
        if (t == 0) {
            Patch patch;
            patch.obj = this;
            patch.normal = normal;
            for (int i = 0; i < 4; ++i) {
                P3 patchP;
                patchP.position = point4[i].position;
                patchP.normal = normal;
                patchP.texture = point4[i].texture;
                patch.vertices.push_back(patchP);
            }
            patchs.push_back(patch);
            return;
        }
        // 递归划分
        P3 mid[5];
        mid[0].position = (point4[0].position + point4[1].position) / 2.0;
        mid[0].normal = (point4[0].normal + point4[1].normal) / 2.0;
        mid[0].texture = (point4[0].texture + point4[1].texture) / 2.0;

        mid[1].position = (point4[0].position + point4[2].position) / 2.0;
        mid[1].normal = (point4[0].normal + point4[2].normal) / 2.0;
        mid[1].texture = (point4[0].texture + point4[2].texture) / 2.0;

        mid[2].position = (point4[0].position + point4[3].position) / 2.0;
        mid[2].normal = (point4[0].normal + point4[3].normal) / 2.0;
        mid[2].texture = (point4[0].texture + point4[3].texture) / 2.0;

        mid[3].position = (point4[2].position + point4[1].position) / 2.0;
        mid[3].normal = (point4[2].normal + point4[1].normal) / 2.0;
        mid[3].texture = (point4[2].texture + point4[1].texture) / 2.0;

        mid[4].position = (point4[2].position + point4[3].position) / 2.0;
        mid[4].normal = (point4[2].normal + point4[3].normal) / 2.0;
        mid[4].texture = (point4[2].texture + point4[3].texture) / 2.0;

        P3 div[4][4];
        div[0][0] = point4[0];
        div[0][1] = mid[0];
        div[0][2] = mid[1];
        div[0][3] = mid[2];
        SmoothDividenPatch(patchs, div[0], t - 1);

        div[1][0] = mid[0];
        div[1][1] = point4[1];
        div[1][2] = mid[3];
        div[1][3] = mid[1];
        SmoothDividenPatch(patchs, div[1], t - 1);

        div[2][0] = mid[1];
        div[2][1] = mid[3];
        div[2][2] = point4[2];
        div[2][3] = mid[4];
        SmoothDividenPatch(patchs, div[2], t - 1);

        div[3][0] = mid[2];
        div[3][1] = mid[1];
        div[3][2] = mid[4];
        div[3][3] = point4[3];
        SmoothDividenPatch(patchs, div[3], t - 1);
    }

protected:
    Texture texture[4];
};

