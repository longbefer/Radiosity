#pragma once
#include "Compound.h"
/*
* 网格对象，用于碰撞加速
* 参见《光线跟踪算法技术》
* （注意事项：SetupCells() 必须放到最后执行，否则可能由于变换问题出现错误）
*/
class Grid :
    public Compound
{
public:
    Grid() :nx(0), ny(0), nz(0) {}
    virtual ~Grid() {
        ResetCells();
        this->DeleteObjects();
    }

public:
    virtual bool IsHit(const Ray&, double&) const;
    virtual std::vector<Patch> ObjectPatch() {
        this->SetupCells();
        return Compound::ObjectPatch();
    }

    virtual void SetTransformMatrix(const Matrix& matrix) {
        Compound::SetTransformMatrix(matrix);
        // SetupCells();
    }

protected:
    void SetupCells();
    void ResetCells() {
        for (size_t i = 0; i < cells.size(); ++i) {
            // 先清除组合体对象，在删除Compound内的对象
            auto obj = dynamic_cast<Compound*>(cells[i]);
            if (obj) {
                delete obj;
                obj = nullptr;
            }
        }
        cells.erase(cells.begin(), cells.end());
        //cells.clear();
    }

private:
    std::vector<GameObject*> cells;

    // nx, ny, nz分别指代x方向上cells的数量，y方向上cells的数量，z方向上cells的数量
    int nx, ny, nz;
};

