#pragma once
#include "Compound.h"
/*
* �������������ײ����
* �μ������߸����㷨������
* ��ע�����SetupCells() ����ŵ����ִ�У�����������ڱ任������ִ���
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
            // ���������������ɾ��Compound�ڵĶ���
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

    // nx, ny, nz�ֱ�ָ��x������cells��������y������cells��������z������cells������
    int nx, ny, nz;
};

