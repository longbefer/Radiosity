#include "pch.h"
#include "Grid.h"
#include "Patch.h"

void Grid::SetupCells()
{
	// 获取所有对象的最大包围盒
	this->box = Compound::GetBoundingBox();

	// 计算每个单元在x,y,z方向上的数量
	size_t objNumber = objects.size();
	double wx = box.point[1].x - box.point[0].x;  // x的宽度
	double wy = box.point[1].y - box.point[0].y;  // y的宽度
	double wz = box.point[1].z - box.point[0].z;  // z的宽度
	
	double multiplier = 2.0; 
	double s = std::pow(wx * wy * wz / objNumber, 1.0 / 3.0);
	
	nx = static_cast<int>(multiplier * wx / s) + 1;
	ny = static_cast<int>(multiplier * wy / s) + 1;
	nz = static_cast<int>(multiplier * wz / s) + 1;

	// 将cells的内容置为空指针
	int cellsNumber = nx * ny * nz;
	cells.reserve(cellsNumber);
	for (size_t i = 0; i < cellsNumber; ++i)
		cells.push_back(nullptr);

	// cells中物体计数器
	std::vector<int> counts(cellsNumber, 0);

	BBox objBox; 
	int index;

	// 将对象放入cells中
	for (int j = 0; j < objNumber; j++)
	{
		objBox = objects[j]->GetBoundingBox();

		// 计算对象包围盒角点的单元格索引
		int ixmin = Clamp((objBox.point[0].x - this->box.point[0].x) * nx / (this->box.point[1].x - this->box.point[0].x), 0, nx - 1);
		int iymin = Clamp((objBox.point[0].y - this->box.point[0].y) * ny / (this->box.point[1].y - this->box.point[0].y), 0, ny - 1);
		int izmin = Clamp((objBox.point[0].z - this->box.point[0].z) * nz / (this->box.point[1].z - this->box.point[0].z), 0, nz - 1);
		int ixmax = Clamp((objBox.point[1].x - this->box.point[0].x) * nx / (this->box.point[1].x - this->box.point[0].x), 0, nx - 1);
		int iymax = Clamp((objBox.point[1].y - this->box.point[0].y) * ny / (this->box.point[1].y - this->box.point[0].y), 0, ny - 1);
		int izmax = Clamp((objBox.point[1].z - this->box.point[0].z) * nz / (this->box.point[1].z - this->box.point[0].z), 0, nz - 1);

		// 添加对象到单元格中
		for (int iz = izmin; iz <= izmax; iz++)// z方向的单元格
		{
			for (int iy = iymin; iy <= iymax; iy++)// y方向的单元格
			{
				for (int ix = ixmin; ix <= ixmax; ix++)// x方向的单元格
				{
					index = ix + nx * iy + nx * ny * iz;// 计算栅格单元的索引

					if (counts[index] == 0)
					{
						// 若当前单元格不存在物体，直接添加
						cells[index] = objects[j];
						counts[index] += 1;
						index = 1;
					}
					else
					{
						if (counts[index] == 1)
						{
							// 若当前单元格已存在一个物体，设置组合体
							// 以便于添加多个物体
							// 构造一个组合对象
							Compound* compound_ptr = new Compound;
							// 添加单元格中已有的对象
							compound_ptr->AddObject(cells[index]);
							// 添加新对象
							compound_ptr->AddObject(objects[j]);
							// 在当前单元格中存储复合物
							cells[index] = compound_ptr;
							// 索引=2
							counts[index] += 1;
						}
						else
						{
							// 存在多个物体时，确保保存的为组合体对象，否则报错
							auto obj = dynamic_cast<Compound*>(cells[index]);
							if (obj) obj->AddObject(objects[j]);
							else throw std::bad_cast();
							// 统计
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
	// 以下是包围盒判断
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
	//找到最大的t值
	t0 = std::max({ tx_min, ty_min, tz_min });
	//找到最小的t值
	t1 = std::min({ tx_max, ty_max, tz_max });

	if (t0 > t1) return false;

	// 以下判断点是否在包围盒内部
	int ix, iy, iz;
	
	Point3d p = ray.o + t0 * ray.d;
	if (box.Inside(ray.o)) p = ray.o;
	ix = Clamp((p.x - x0) * nx / (x1 - x0), 0, nx - 1);
	iy = Clamp((p.y - y0) * nx / (y1 - y0), 0, ny - 1);
	iz = Clamp((p.z - z0) * nx / (z1 - z0), 0, nz - 1);

	// 以下设置击中层次包围盒内部增量
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

	//遍历网格
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