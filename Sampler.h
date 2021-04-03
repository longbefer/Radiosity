#pragma once
#include "P3.h"
#include <random>

class Sampler
{
public:
	const double invRandomMax = 1.0 / RAND_MAX;
#if 0
	Sampler(int n) { this->numSample = n; this->numSet = 1ULL * n; this->count = 0ULL; }
	void SetSampleNumber(int num) { this->numSample = num; }
	int GetSampleNumber() { return numSample; }

	void TriangleSample() {
		samples.clear();
		srand(std::time(nullptr));
		size_t n = static_cast<size_t>(std::sqrt(numSample));
		samples.reserve(numSample * numSet);
		for (size_t i = 0ULL; i < numSet; ++i)
			for (size_t p = 0ULL; p < n; ++p)
				for (size_t q = 0ULL; q < n; ++q)
				{
					double x = rand() * invRandomMax;
					double y = rand() * invRandomMax;
					if ((x + y) > 1.0) {
						x = 1.0 - x;
						y = 1.0 - y;
					}
					size_t index = i * numSample + p * n + q;
					samples[index].x = x;
					samples[index].y = y;
				}
	}

	void RectangleSample() {
		samples.clear();
		srand(std::time(nullptr));
		size_t n = static_cast<size_t>(std::sqrt(numSample));
		samples.reserve(numSample * numSet);
		for (size_t i = 0ULL; i < numSet; ++i)
			for (size_t p = 0ULL; p < n; ++p)
				for (size_t q = 0ULL; q < n; ++q)
				{
					double x = rand() * invRandomMax;
					double y = rand() * invRandomMax;
					size_t index = i * numSample + p * n + q;
					samples[index].x = x;
					samples[index].y = y;
				}
	}

	Point2d GetSamplerPoint() {
		return samples[count++ % (numSample * numSet)];
	}

protected:
	size_t numSample;  // 采样点数量
	size_t numSet;     // 样本集
	std::vector<Point2d> samples;
	size_t count;  // 当前使用的采样点
#endif
public:
	Point3d TriangleSample(const Point3d&v0, const Point3d&v1, const Point3d&v2) {
		// 随机取点
		Point2d samples;
		double x = rand() * invRandomMax;
		double y = rand() * invRandomMax;
		if ((x + y) > 1.0) {
			x = 1.0 - x;
			y = 1.0 - y;
		}
		samples.x = x;
		samples.y = y;

		assert(samples < Point2d(0.0, 0.0), "random is error");
		assert(samples > Point2d(1.0, 1.0), "random is error");

		Vector3d a = v1 - v0;
		Vector3d b = v2 - v0;

		return v0 + a * samples.x + b * samples.y;
	}

	Point3d RectangleSample(const Point3d& v0, const Vector3d& a, const Vector3d& b) {
		// 随机取点
		Point2d samples;
		samples.x = rand() * invRandomMax;
		samples.y = rand() * invRandomMax;

		assert(samples < Point2d(0.0, 0.0), "random is error");
		assert(samples > Point2d(1.0, 1.0), "random is error");

		return v0 + a * samples.x + b * samples.y;
	}
};

