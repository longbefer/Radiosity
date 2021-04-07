#include "pch.h"
#include "Radiation.h"
#include "Light.h"
#include "Paint.h"
#include "Projection.h"
#include "SmoothRectangle.h"
#include "Sampler.h"
extern volatile bool ENDPROGRAM;
#if !USERADIATIONSMOOTH
void Radiation::Init()
{
	size_t n = patchs.size();
	// ��ʼ����״����
#ifdef Southwell
	//factors = new double[n * (n + 1) / 2]{ 0 };
	increase = new Color[n]{ Black };
	// ��ʼ����Ƭ����Ȳ���������ΪE
	for (size_t i = 0; i < n; ++i) {
		increase[i] = patchs[i].obj->GetEmmision();
		patchs[i].excident = increase[i];

		// ���������
		double area = patchs[i].GetArea();
		sumArea += area;
		avgReflect += patchs[i].obj->GetReflectance() * area;
	}
	avgReflect /= sumArea;
#else
	factors = new double* [n];
	for (size_t i = 0; i < n; ++i)
		factors[i] = new double[n];
	// ��ʼ������
	for (auto& patch : patchs)
		patch.excident = patch.obj->GetEmmision();
	bFinish = false;
#endif
}

void Radiation::Rendered(size_t n)
{
#ifdef Southwell
	Calculate();
#else
	for (size_t i = n; i > 0; --i)
		Calculate();
#endif
	DeleteMatrix();
}

void Radiation::Draw(CDC* pDC)
{
	Paint paint;
	Projection project;
	
	P3 point3[3];
	P3 point4[4];
	paint.InitDeepBuffer(1000, 1000, 1000);
	
	for (auto& patch : patchs) {
		if (patch.vertices.size() == 3) {
			for (int loop = 0; loop < 3; ++loop) {
				point3[loop].position = project.PerspectiveProjection(patch.vertices[loop].position);
				point3[loop].excident = patch.excident;
				point3[loop].normal = patch.normal;
				point3[loop].texture = patch.vertices[loop].texture;
			}
			paint.SetPoint(point3[0], point3[1], point3[2]);
			paint.GouraudShading(pDC, patch);
		}
		else
		{
			for (int loop = 0; loop < static_cast<int>(patch.vertices.size()); ++loop) {
				point4[loop].position = project.PerspectiveProjection(patch.vertices[loop].position);
				point4[loop].excident = patch.excident;
				point4[loop].normal = patch.normal;
				point4[loop].texture = patch.vertices[loop].texture;
			}
			paint.SetPoint(point4[0], point4[1], point4[2]);
			paint.GouraudShading(pDC, patch);
			paint.SetPoint(point4[0], point4[2], point4[3]);
			paint.GouraudShading(pDC, patch);
		}
	}
}

void Radiation::Calculate()
{
	int n = static_cast<int>(patchs.size());
#ifdef Gauss_Siedel
#pragma omp parallel for schedule(dynamic, 1) // OpenMP
	for (int i = 0; i < n; ++i) {
		if (!bFinish) {
			for (int j = i; j < n; ++j) {
				// �洢��״����
				double fac = GetViewFactor(patchs[i], patchs[j]);
				factors[i][j] = patchs[j].GetArea() * fac;
				factors[j][i] = patchs[i].GetArea() * fac;
			}
		}
		// �ռ�������Ƭ�ķ����
		for (int j = 0; j < n; ++j)
			patchs[i].incident += patchs[j].excident * factors[i][j];
		// �������Ƭ�ķ����
		auto& I = patchs[i].incident;
		auto& R = patchs[i].obj->GetImage() ? patchs[i].GetReflectance() : patchs[i].obj->GetReflectance();
		auto& E = patchs[i].obj->GetEmmision();
		patchs[i].excident = ((I * R) + E).Clamp();
	}
	bFinish = true;
#endif
#ifdef Normal_Result
#pragma omp parallel for schedule(dynamic, 1) // OpenMP
	for (int i = 0; i < n; ++i) {
		if (!bFinish) {
			for (int j = i; j < n; ++j) {
				// �洢��״����
				double fac = GetViewFactor(patchs[i], patchs[j]);
				factors[i][j] = patchs[j].GetArea() * fac;
				factors[j][i] = patchs[i].GetArea() * fac;
			}
		}
		for (int j = 0; j < n; ++j)
			patchs[i].incident += patchs[j].excident * factors[i][j];
	}
	// Ȼ������������
	for (int i = 0; i < n; ++i) {
		auto& I = patchs[i].incident;
		auto& R = patchs[i].obj->GetImage() ? patchs[i].GetReflectance() : patchs[i].obj->GetReflectance();
		auto& E = patchs[i].obj->GetEmmision();
		patchs[i].excident = ((I * R) + E).Clamp();
		//patchs[i].excident = (patchs[i].incident * patchs[i].obj->GetReflectance() + patchs[i].obj->GetEmmision()).Clamp();
	}
	bFinish = true;
#endif 
#ifdef Southwell
	// ��ȡ����������Ƭ������
	int index = 0;
	// ��ȡ����������Ƭ�����ͷŵ�������ע���������������������
	Color maxColor = Black;
	// ����
	Color ambient = Black;
	// ���ⷴ����
	const Color invAvgReflect = 1.0 / (White - avgReflect);
	do {
		maxColor = Black; ambient = Black;
		// ��ÿһ����Ƭ�У�ѡ�����Ĵ���������
		for (int i = 0; i < n; ++i) {
			Color currentColor = increase[i] * patchs[i].GetArea();
			if (currentColor > maxColor) { // �ؼ�������ζ�����ɫ˭��˭��
				maxColor = currentColor;
				index = i;
			}
			ambient += currentColor;
		}
		ambient /= sumArea;
		ambient *= invAvgReflect;
		// ��ÿ����Ƭ��ӷ�������
		#pragma omp parallel for schedule(dynamic, 1) // OpenMP
		for (int i = 0; i < n; ++i) {
			Color refect = patchs[i].obj->GetImage() ? patchs[i].GetReflectance() : patchs[i].obj->GetReflectance();
			double factor = GetViewFactor(patchs[index], patchs[i]);
			Color rad = factor * refect * maxColor;
			increase[i] += rad;
			if (bFinish) patchs[i].excident += rad;
			else patchs[i].excident += (rad + ambient * refect);
			patchs[i].incident += factor * maxColor;
			patchs[i].excident.Clamped();
		}
		increase[index] = Black;
		bFinish = true;
		if (ENDPROGRAM) return;  // ����������ʱ���˳���Ⱦ
	} while (maxColor > Color(kEpsilon, kEpsilon, kEpsilon)); // ���0ʱ������ֹͣ
#endif
}

double Radiation::GetViewFactor(const Patch&in, const Patch&out)
{
	if (&in == &out) return 0.0;
#if USERADIATIONSMOOTH // �Ƿ�ʹ�ö�����������
	// ��������
	Point3d outCenter = (out.vertices[0].position + out.vertices[1].position + out.vertices[2].position) / 3.0;
	Point3d inCenter = (in.vertices[0].position + in.vertices[1].position + in.vertices[2].position) / 3.0;

	const Vector3d pointNormal = outCenter - inCenter;
	Vector3d vectorij = pointNormal.Normalized();
	double cosij = Dot(in.normal.Normalized(), vectorij);
	double cosji = Dot(out.normal.Normalized(), -vectorij);

	// ��Ƭ֮���໥�ɼ�
	if (cosij <= 0.0 || cosji <= 0.0)
		return 0.0;

	double t = 1.0E10;
	Ray ray(inCenter, vectorij);
	
	// ����Ƭ֮��ľ���
	double tHit = pointNormal.Distance();
	
	// �ж��Ƿ������Ƭ
	if (hitObject) t = hitObject(ray);
	else throw std::bad_function_call(); // need init hitObject function
	if (std::fabs(t - tHit) > kEpsilon)
		return 0.0;

	double invLength = 1.0 / pointNormal.Length_Square();

	return cosij * cosji * inv_pi * invLength;
#else 
#ifdef Southwell
	int samplerNumber = 16;   // ����������
#else 
	int samplerNumber = 64;   // ����������
#endif
	Sampler sampler[2];       // ������Ƭ�ֱ����
	double avg_factor = 0.0;  // ��ȡƽ������״����

	for (int i = 0; i < samplerNumber; ++i) {
		Point3d inPoint = (in.vertices.size() == 3ULL) ? // ����Ϊ��������ʹ�������β�����������β���
			sampler[0].TriangleSample(in.vertices[0].position, in.vertices[1].position, in.vertices[2].position) :
			sampler[0].RectangleSample(in.vertices[0].position, in.vertices[1].position - in.vertices[0].position,
				in.vertices[3].position - in.vertices[0].position);
		Point3d outPoint = (out.vertices.size() == 3ULL) ?
			sampler[1].TriangleSample(out.vertices[0].position, out.vertices[1].position, out.vertices[2].position) :
			sampler[1].RectangleSample(out.vertices[0].position, out.vertices[1].position - out.vertices[0].position,
				out.vertices[3].position - out.vertices[0].position);

		const Vector3d pointNormal = outPoint - inPoint;
		Vector3d vectorij = pointNormal.Normalized();
		double cosij = Dot(in.normal.Normalized(), vectorij);
		double cosji = Dot(out.normal.Normalized(), -vectorij);

		// ��Ƭ֮���໥�ɼ�
		if (cosij <= 0.0 || cosji <= 0.0)
			return 0.0;

		double t = 1.0E10;
		Ray ray(inPoint, vectorij);

		// ����Ƭ֮��ľ���
		double tHit = pointNormal.Distance();

		// �ж��Ƿ������Ƭ
		if (hitObject) t = hitObject(ray);
		else throw std::bad_function_call(); // need init hitObject function
		if (std::fabs(t - tHit) > kEpsilon)
			return 0.0;
		//if (t < tHit - kEpsilon) return 0.0; 
		// ע�⣺��䲢����ȷ������ȴ�����ܴ���û�л��и���Ƭ�Ŀ��ܣ�
		// �� �� t > tHitʱ��Ҳ��ʾ���и���Ƭ

		double invLength = 1.0 / pointNormal.Length_Square();
		avg_factor += cosij * cosji * inv_pi * invLength;
	}
	return avg_factor / static_cast<double>(samplerNumber);
#endif
}
#else
void Radiation::Init()
{
	// ��ʼ������
#ifdef Southwell
	size_t n = patchs.size();
	increase = new Color[n]{ Black };
	for (size_t i = 0; i < n; ++i) {
		patchs[i].excident = patchs[i].obj->GetEmmision();
		for (auto& vertex : patchs[i].vertices)
			vertex.excident = patchs[i].obj->GetEmmision();
		increase[i] = patchs[i].excident;
	}
#else
	for (auto& patch : patchs) {
		patch.excident = patch.obj->GetEmmision();
		for (auto& vertres : patch.vertices)
			vertres.excident = patch.excident;
	}
#endif
}

void Radiation::Rendered(size_t n)
{
	for (size_t i = n; i > 0; --i) {
		Calculate();
		// ����ȫ�����ڽ�����������������ڱ����ŵ�Calculate����Ӱ��
		// ����Ĳ��м��㡣����
		if (ENDPROGRAM) break;
	}
#ifdef Southwell
	DeleteMatrix();
#endif
}

void Radiation::Draw(CDC* pDC)
{
	Paint paint;
	Projection project;

	P3 point3[3];
	P3 point4[4];
	paint.InitDeepBuffer(1000, 1000, 1000);

	for (auto& patch : patchs) {
	//for (size_t i = 0; i < patchs.size(); ++i) {
	//	auto& patch = patchs[i];
		if (patch.vertices.size() == 3) {
			for (int loop = 0; loop < 3; ++loop) {
				point3[loop].position = project.PerspectiveProjection(patch.vertices[loop].position);
				point3[loop].excident = patch.vertices[loop].excident;
				point3[loop].normal = patch.vertices[loop].normal;
				point3[loop].texture = patch.vertices[loop].texture;
			}
			paint.SetPoint(point3[0], point3[1], point3[2]);
			paint.GouraudShading(pDC, patch);
		}
		else
		{
			for (int loop = 0; loop < static_cast<int>(patch.vertices.size()); ++loop) {
				point4[loop].position = project.PerspectiveProjection(patch.vertices[loop].position);
				point4[loop].excident = patch.vertices[loop].excident;
				point4[loop].normal = patch.vertices[loop].normal;
				point4[loop].texture = patch.vertices[loop].texture;
			}
			paint.SetPoint(point4[0], point4[1], point4[2]);
			paint.GouraudShading(pDC, patch);
			paint.SetPoint(point4[0], point4[2], point4[3]);
			paint.GouraudShading(pDC, patch);
		}
	}
}

void Radiation::Calculate()
{
	int n = static_cast<int>(patchs.size());
#ifdef Southwell
	// ��ȡ����������Ƭ������
	int index = 0;
	// ��ȡ����������Ƭ�����ͷŵ�������ע���������������������
	Color maxColor = Black;
	do {
		maxColor = Black;
		// ��ÿһ����Ƭ�У�ѡ�����Ĵ���������
		for (int i = 0; i < n; ++i) {
			Color currentColor = increase[i] * patchs[i].GetArea();
			if (currentColor > maxColor) {
				maxColor = currentColor;
				index = i;
			}
		}
		// ��ÿ����Ƭ��ӷ�������
#pragma omp parallel for schedule(dynamic, 1) // OpenMP
		for (int i = 0; i < n; ++i) {
			Color rad = Black, incid = Black; // rad Ϊ�������������� incid Ϊ��������
			for (int k = 0; k < patchs[i].vertices.size(); ++k) {
				Color signalRad = Black, signalIncident = Black;
				for (int s = 0; s < patchs[index].vertices.size(); ++s) {
					double factor = GetViewFactor(patchs[index], patchs[i], s, k);
					signalRad += factor * patchs[i].obj->GetReflectance() * maxColor;
					signalIncident += factor * maxColor;
				}
				signalRad /= static_cast<double>(patchs[index].vertices.size());
				signalIncident /= static_cast<double>(patchs[index].vertices.size());
				patchs[i].vertices[k].excident += signalRad;
				patchs[i].vertices[k].excident.Clamped();
				patchs[i].vertices[k].incident += signalIncident;
				rad += signalRad;
				incid += signalIncident;
			}
			Color avgRad = rad / static_cast<double>(patchs[i].vertices.size());
			incid /= static_cast<double>(patchs[i].vertices.size());
			increase[i] += avgRad;
			patchs[i].excident += avgRad;
			patchs[i].excident.Clamped();
			patchs[i].incident += incid; // ������������������������֮��ļ���
		}
		increase[index] = Black;
		if (ENDPROGRAM) return;  // ����������ʱ���˳���Ⱦ
	} while (maxColor > Color(kEpsilon, kEpsilon, kEpsilon)); // ���0ʱ������ֹͣ
#else
	//#pragma omp parallel for schedule(dynamic, 1) // OpenMP
	for (int i = 0; i < n; ++i) { // in ��Ƭ
		for (int j = i; j < n; ++j) { // out ��Ƭ
			double areai = patchs[i].GetArea(), areaj = patchs[j].GetArea();
			Color inPatchIncident = Black, outPatchIncident = Black;
			size_t inSize = patchs[i].vertices.size(),
				outSize = patchs[j].vertices.size();
			std::unique_ptr<Color[]> outVertexIncident(new Color[outSize]);
			for (size_t v = 0; v < outSize; ++v) outVertexIncident[v] = Black; // �����ʼ��������
			for (size_t k = 0; k < inSize; ++k) {
				Color inVertexIncident = Black;
				for (size_t s = 0; s < outSize; ++s) {
					double fac = GetViewFactor(patchs[i], patchs[j], k, s);
					// �ռ�������Ƭ�ϵ�ķ����
					inVertexIncident += (fac * areaj * patchs[j].vertices[s].excident);
					outVertexIncident[s] += (fac * areai * patchs[i].vertices[k].excident);
				}
				// ���ｫin��Ƭ������ƽ��
				auto sigalVertices = inVertexIncident / static_cast<double>(outSize); // ���������յ�������
				patchs[i].vertices[k].incident += sigalVertices;
				inPatchIncident += sigalVertices;
			}
			patchs[i].incident += (inPatchIncident / static_cast<double>(inSize));
			// ���ｫout��Ƭ(�����)������ƽ��
			for (size_t s = 0; s < outSize; ++s) {
				auto sigalVertices = (outVertexIncident[s] / static_cast<double>(inSize));
				patchs[j].vertices[s].incident += sigalVertices;
				outPatchIncident += sigalVertices;
			}
			patchs[j].incident += (outPatchIncident / static_cast<double>(outSize));
		}
		auto& I = patchs[i].incident;
		auto& R = patchs[i].obj->GetImage() ? patchs[i].GetReflectance() : patchs[i].obj->GetReflectance();
		auto& E = patchs[i].obj->GetEmmision();
		// ÿ����ķ����
		for (size_t k = 0; k < patchs[i].vertices.size(); ++k)
			patchs[i].vertices[k].excident = ((patchs[i].vertices[k].incident * R) + E).Clamp();
		// �����in��Ƭ�ķ����
		patchs[i].excident = ((I * R) + E).Clamp();
		//if (ENDPROGRAM) return; // ����������Ժܿ����������Ҳ�ƻ������������
	}
	bFinish = true;
#endif
}

double Radiation::GetViewFactor(const Patch& in, const Patch& out, const size_t& ip = 0, const size_t& op = 0)
{
	if (&in == &out) return 0.0;

	// ��������
	Point3d outCenter = out.vertices[op].position;
	Point3d inCenter = in.vertices[ip].position;

	//if (in.vertices[ip].normal.w != 0.0 || out.vertices[op].normal.w != 0.0) {
	//	outCenter = ZERO;
	//	inCenter = ZERO;
	//}

	const Vector3d pointNormal = outCenter - inCenter;
	Vector3d vectorij = pointNormal.Normalized();
	double cosij = Dot(in.vertices[ip].normal.Normalized(), vectorij);
	double cosji = Dot(out.vertices[op].normal.Normalized(), -vectorij);

	// ��Ƭ֮���໥�ɼ�
	if (cosij <= 0.0 || cosji <= 0.0)
		return 0.0;

	double t = 1.0E10;
	Ray ray(inCenter, vectorij);

	// ����Ƭ֮��ľ���
	double tHit = pointNormal.Distance();

	// �ж��Ƿ������Ƭ
	if (hitObject) t = hitObject(ray);
	else throw std::bad_function_call(); // need init hitObject function
	if (std::fabs(t - tHit) > kEpsilon)
		return 0.0;
	//if (t < tHit - kEpsilon) return 0.0;

	double invLength = 1.0 / pointNormal.Length_Square();

	return cosij * cosji * inv_pi * invLength;
}
#endif

