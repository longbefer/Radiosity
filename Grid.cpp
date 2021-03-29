#include "pch.h"
#include "Grid.h"
#include "Patch.h"

void Grid::SetupCells()
{
	// ��ȡ���ж��������Χ��
	this->box = Compound::GetBoundingBox();

	// ����ÿ����Ԫ��x,y,z�����ϵ�����
	size_t objNumber = objects.size();
	double wx = box.point[1].x - box.point[0].x;  // x�Ŀ��
	double wy = box.point[1].y - box.point[0].y;  // y�Ŀ��
	double wz = box.point[1].z - box.point[0].z;  // z�Ŀ��
	
	double multiplier = 2.0; 
	double s = std::pow(wx * wy * wz / objNumber, 1.0 / 3.0);
	
	nx = static_cast<int>(multiplier * wx / s) + 1;
	ny = static_cast<int>(multiplier * wy / s) + 1;
	nz = static_cast<int>(multiplier * wz / s) + 1;

	// ��cells��������Ϊ��ָ��
	int cellsNumber = nx * ny * nz;
	cells.reserve(cellsNumber);
	for (size_t i = 0; i < cellsNumber; ++i)
		cells.push_back(nullptr);

	// cells�����������
	std::vector<int> counts(cellsNumber, 0);

	BBox objBox; 
	int index;

	// ���������cells��
	for (int j = 0; j < objNumber; j++)
	{
		objBox = objects[j]->GetBoundingBox();

		// ��������Χ�нǵ�ĵ�Ԫ������
		int ixmin = Clamp((objBox.point[0].x - this->box.point[0].x) * nx / (this->box.point[1].x - this->box.point[0].x), 0, nx - 1);
		int iymin = Clamp((objBox.point[0].y - this->box.point[0].y) * ny / (this->box.point[1].y - this->box.point[0].y), 0, ny - 1);
		int izmin = Clamp((objBox.point[0].z - this->box.point[0].z) * nz / (this->box.point[1].z - this->box.point[0].z), 0, nz - 1);
		int ixmax = Clamp((objBox.point[1].x - this->box.point[0].x) * nx / (this->box.point[1].x - this->box.point[0].x), 0, nx - 1);
		int iymax = Clamp((objBox.point[1].y - this->box.point[0].y) * ny / (this->box.point[1].y - this->box.point[0].y), 0, ny - 1);
		int izmax = Clamp((objBox.point[1].z - this->box.point[0].z) * nz / (this->box.point[1].z - this->box.point[0].z), 0, nz - 1);

		// ��Ӷ��󵽵�Ԫ����
		for (int iz = izmin; iz <= izmax; iz++)// z����ĵ�Ԫ��
		{
			for (int iy = iymin; iy <= iymax; iy++)// y����ĵ�Ԫ��
			{
				for (int ix = ixmin; ix <= ixmax; ix++)// x����ĵ�Ԫ��
				{
					index = ix + nx * iy + nx * ny * iz;// ����դ��Ԫ������

					if (counts[index] == 0)
					{
						// ����ǰ��Ԫ�񲻴������壬ֱ�����
						cells[index] = objects[j];
						counts[index] += 1;
						index = 1;
					}
					else
					{
						if (counts[index] == 1)
						{
							// ����ǰ��Ԫ���Ѵ���һ�����壬���������
							// �Ա�����Ӷ������
							// ����һ����϶���
							Compound* compound_ptr = new Compound;
							// ��ӵ�Ԫ�������еĶ���
							compound_ptr->AddObject(cells[index]);
							// ����¶���
							compound_ptr->AddObject(objects[j]);
							// �ڵ�ǰ��Ԫ���д洢������
							cells[index] = compound_ptr;
							// ����=2
							counts[index] += 1;
						}
						else
						{
							// ���ڶ������ʱ��ȷ�������Ϊ�������󣬷��򱨴�
							auto obj = dynamic_cast<Compound*>(cells[index]);
							if (obj) obj->AddObject(objects[j]);
							else throw std::bad_cast();
							// ͳ��
							counts[index] += 1;
						}
					}
				}
			}
		}
	}
}


bool Grid::IsHit(const Ray&ray, double&tmin) const
{
	// �����ǰ�Χ���ж�
	auto &ox = ray.o.x, &oy = ray.o.y, &oz = ray.o.z;
	auto &dx = ray.d.x, &dy = ray.d.y, &dz = ray.d.z;

	auto &x0 = box.point[0].x, &y0 = box.point[0].y, &z0 = box.point[0].z;
	auto &x1 = box.point[1].x, &y1 = box.point[1].y, &z1 = box.point[1].z;

	double tx_min, ty_min, tz_min;
	double tx_max, ty_max, tz_max;

	double a = 1.0 / dx;
	tx_min = (x0 - ox) * a;
	tx_max = (x1 - ox) * a;
	if (a < 0) std::swap(tx_min, tx_max);

	double b = 1.0 / dy;
	ty_min = (y0 - oy) * b;
	ty_max = (y1 - oy) * b;
	if (b < 0) std::swap(ty_min, ty_max);

	double c = 1.0 / dz;
	tz_min = (z0 - oz) * c;
	tz_max = (z1 - oz) * c;
	if (c < 0) std::swap(tz_min, tz_max);

	double t0, t1;
	//�ҵ�����tֵ
	t0 = std::max({ tx_min, ty_min, tz_min });
	//�ҵ���С��tֵ
	t1 = std::min({ tx_max, ty_max, tz_max });

	if (t0 > t1) return false;

	// �����жϵ��Ƿ��ڰ�Χ���ڲ�
	int ix, iy, iz;
	
	Point3d p = ray.o + t0 * ray.d;
	if (box.Inside(ray.o)) p = ray.o;
	ix = Clamp((p.x - x0) * nx / (x1 - x0), 0, nx - 1);
	iy = Clamp((p.y - y0) * nx / (y1 - y0), 0, ny - 1);
	iz = Clamp((p.z - z0) * nx / (z1 - z0), 0, nz - 1);

	// �������û��в�ΰ�Χ���ڲ�����
	double dtx = (tx_max - tx_min) / nx;
	double dty = (ty_max - ty_min) / ny;
	double dtz = (tz_max - tz_min) / nz;

	double tx_next = tx_min + (ix + 1) * dtx;
	int ix_step = 1, ix_stop = nx;
	if (dx <= 0) {
		tx_next = ((dx == 0) ? 1.0E10 : (tx_min + (nx - ix) * dtx));
		ix_step = -1;
		ix_stop = -1;
	}

	double ty_next = ty_min + (iy + 1) * dty;
	int iy_step = 1, iy_stop = ny;
	if (dy <= 0) {
		ty_next = ((dy == 0) ? 1.0E10 : (ty_min + (ny - iy) * dty));
		iy_step = -1;
		iy_stop = -1;
	}

	double tz_next = tz_min + (iz + 1) * dtz;
	int iz_step = 1, iz_stop = nz;
	if (dz <= 0) {
		tz_next = ((dz == 0) ? 1.0E10 : (tz_min + (nz - iz) * dtz));
		iz_step = -1;
		iz_stop = -1;
	}

	//��������
	while (true) {
		GameObject* object_ptr = cells[ix + nx * iy + nx * ny * iz];

		if (tx_next < ty_next && tx_next < tz_next) {
			if (object_ptr && object_ptr->IsHit(ray, tmin) && tmin < tx_next)
				return (true);

			tx_next += dtx;
			ix += ix_step;

			if (ix == ix_stop) return (false);
		}
		else 
		{
			if (ty_next < tz_next) {
				if (object_ptr && object_ptr->IsHit(ray, tmin) && tmin < ty_next)
					return (true);

				ty_next += dty;
				iy += iy_step;

				if (iy == iy_stop) return (false);
			}
			else 
			{
				if (object_ptr && object_ptr->IsHit(ray, tmin) && tmin < tz_next)
					return (true);

				tz_next += dtz;
				iz += iz_step;

				if (iz == iz_stop) return (false);
			}
		}
	}
}