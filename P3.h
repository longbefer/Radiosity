#pragma once
#include <iostream>
#include <iomanip> /* 输出格式化字符 */
#include <type_traits> /* is_arithmetic */
#include <cmath> /* sqrt fabs 等函数*/
#include <exception> /* 错误异常抛出 */
#include <future> /* 异步 */
#include <array> /* 存储 */
#include <vector> /* 存储 */
#include <memory> /* 智能指针 */
#include <string> /* 字符操作 */


#if defined(__clang__) || defined(__GNUC__)
// English: c++ complier is clang or gcc
// 中文：判断c++编译器是否为 clang 或 gcc
#define CPP_STANDARD __cplusplus
#elif defined(_MSC_VER)
// English: c++ complier is msvc 
// 中文：微软vs编译器
#define CPP_STANDARD _MSVC_LANG
#endif // define clang or gun or msvc

#if CPP_STANDARD < 201704L // c++ version
#define IsNumber typename
#define pi 3.141592653589793      // pi = 3.1415926
#define inv_pi 0.3183098861837907 // 1.0 / pi = 0.31830988
#elif CPP_STANDARD >= 201704L // c++ 20 latest
#if _HAS_CXX20
#include <concepts> // c++ 20
#include <numbers> /* PI 等数学符号集 */
// English: evaluate the template is value type
// 中文：判断模板类型是否为数值类型（非对象类型）
template <typename T>
concept IsNumber = std::is_arithmetic<T>::value;
#elif // _HAS_CXX20
static_assert(_HAS_CXX20, "Yours C++ version is too low.\n");
#error Yours C++ version is too low.
#endif
#endif // CPP_STANDARD

// 删除MFC自带的min和max
#undef min
#undef max

#define PI pi

#define Round(d) static_cast<int>(std::floor(static_cast<double>(d) + 0.5))
inline double Clamp(const double& area, const double& min, const double& max) {
	return (area < min ? min : (area > max) ? max : area);
}
inline int Clamp(const double& area, const int& min, const int& max) {
	int val = static_cast<int>(area);
	return (val < min ? min : (val > max) ? max : val);
}


/*
* 矩阵，用于平移等运算
*/
class Matrix
{
public:
	enum class AXIS { X, Y, Z };
public:
	Matrix(void) { SetIdentity(); }
	Matrix(const Matrix& mat) {
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				m[i][j] = mat.m[i][j];
	}
	~Matrix(void) {};//析构函数
public:
	Matrix& operator=(const Matrix& rhs) {//赋值运算符重载
		if (this == &rhs)
			return(*this);
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				m[i][j] = rhs.m[i][j];
		return(*this);
	}
	Matrix operator*(const Matrix& mat)const {//乘法运算符重载(矩阵乘法)
		Matrix product;
		for (int j = 0; j < 4; j++)
			for (int i = 0; i < 4; i++)
			{
				double sum = 0.0;
				for (int k = 0; k < 4; k++)
					sum += m[i][k] * mat.m[k][j];
				product.m[i][j] = sum;
			}
		return(product);
	}
	Matrix operator/(const double d) {
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				m[i][j] = m[i][j] / d;
		return(*this);
	}
	void SetIdentity(void) {
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				if (i == j) m[i][j] = 1.0;
				else m[i][j] = 0.0;
	};
	Matrix SetTranslate(double tx, double ty, double tz) {
		SetIdentity();
		m[0][3] = tx;
		m[1][3] = ty;
		m[2][3] = tz;
		return *this;
	}
	Matrix SetScale(double sx, double sy, double sz) {
		SetIdentity();
		m[0][0] = sx;
		m[1][1] = sx;
		m[2][2] = sx;
		return *this;
	}
	Matrix SetRotate(double beta, AXIS axis) {
		SetIdentity();
		double rad = beta * pi / 180;
		switch (axis) {
		case AXIS::X:
			m[1][1] = cos(rad); m[1][2] = -sin(rad);
			m[2][1] = sin(rad); m[2][2] = cos(rad);
			break;
		case AXIS::Y:
			m[0][0] = cos(rad); m[0][2] = sin(rad);
			m[2][0] = -sin(rad); m[2][2] = cos(rad);
			break;
		case AXIS::Z:
			m[0][0] = cos(rad); m[0][1] = -sin(rad);
			m[1][0] = sin(rad); m[1][1] = cos(rad);
			break;
		}
		return *this;
	}

	Matrix Translate(double tx, double ty, double tz) const {
		Matrix mat;
		mat.SetTranslate(tx, ty, tz);
		return mat.operator*(*this);
	}
	Matrix Scale(double sx, double sy, double sz) const {
		Matrix mat;
		mat.SetScale(sx, sy, sz);
		return mat.operator*(*this);
	}
	Matrix Rotate(double beta, AXIS axis) const {
		Matrix mat;
		mat.SetRotate(beta, axis);
		return mat.operator*(*this);
	}
public:
	double m[4][4];
};

template <IsNumber T>
class Vec2 {
public:
	Vec2() : x(0.0), y(0.0) {}
	Vec2(const T& x, const T& y) :x(x), y(y) {};

public:
	// 向量相加
	Vec2<T> operator+(const Vec2<T>& rhs) const {
		Vec2<T> sum(this->x + rhs.x, this->y + rhs.y);
		return sum;
	}
	template<IsNumber T1>
	auto operator+(const Vec2<T1>& rhs) const {
		Vec2<decltype(rhs.x + this->x)> sum(this->x + rhs.x, this->y + rhs.y);
		return sum;
	}
	void operator+=(const Vec2<T>& rhs) {
		this->x += rhs.x;
		this->y += rhs.y;
	}
	template<IsNumber T1>
	void operator+=(const Vec2<T1>& rhs) {
		(*this) = this->operator+(rhs);
	}

	// 两向量相减
	template<IsNumber T1>
	auto operator-(const Vec2<T1>& rhs) const {
		Vec2<decltype(rhs.x - this->x)> sum(this->x - rhs.x, this->y - rhs.y);
		return sum;
	}
	template<IsNumber T1>
	void operator-=(const Vec2<T1>& rhs) {
		(*this) = this->operator-(rhs);
	}
	// 取负数
	Vec2<T> operator-() const {
		if (std::is_same<unsigned, T>) throw std::exception("unsigned can't");
		return Vec2<T>(-this->x, -this->y);
	}

	// 乘法
#if _HAS_CXX20
	template<IsNumber T1>
	auto operator*(const T1& rhs) const {
		typedef decltype(this->x* rhs->x) T2;
		return Vec2<T2>(rhs * this->x, rhs * this->y);
	}
	template<IsNumber T1>
	void operator*=(const T1& rhs) {
		(*this) = this->operator*(rhs);
	}
	template<IsNumber T1>
	friend auto operator*(const T1& lhs, const Vec2<T>& rhs) {
		typedef decltype(lhs* rhs->x) T2;
		return Vec2<T2>(rhs.x * lhs, rhs.y * lhs);
	}
#else
	auto operator*(const double& rhs) const {
		return Vec2<T>(static_cast<T>(rhs * this->x), static_cast<T>(rhs * this->y));
	}

	void operator*=(const double& rhs) {
		(*this) = this->operator*(rhs);
	}

	friend auto operator*(const double& lhs, const Vec2<T>& rhs) {
		return Vec2<T>(static_cast<T>(rhs.x * lhs), static_cast<T>(rhs.y * lhs));
	}
#endif

	// 除法
#if _HAS_CXX20
	template<IsNumber T1>
	auto operator/(const T1& rhs) const {
		typedef decltype(rhs* this->x) T2;
		if (static_cast<double>(std::fabs(rhs)) < eps)
			return Vec2<T2>(static_cast<T2>(this->x), static_cast<T2>(this->y));
		return Vec2<T2>(this->x / rhs, this->y / rhs);
	}
	template<IsNumber T1>
	void operator/=(const T1& rhs) {
		if (static_cast<double>(std::fabs(rhs)) < eps)
			return;
		(*this) = this->operator/(rhs);
	}
#else
	auto operator/(const double& rhs) const {
		if (rhs == 0.0) throw std::logic_error("除数为零, Divided is zero");
		return Vec2<T>(static_cast<T>(this->x / rhs), static_cast<T>(this->y / rhs));
	}

	void operator/=(const double& rhs) {
		if (rhs == 0.0) throw std::logic_error("除数为零, Divided is zero");
		(*this) = this->operator/(rhs);
	}
#endif

	// 相等判断
	bool operator==(const Vec2<T>& rhs) const {
		return (this->x == rhs.x && this->y == rhs.y);
	}
	bool operator!=(const Vec2<T>& rhs) const {
		return !this->operator==(rhs);
	}


public:
	T x, y;
};

template <IsNumber T>
class Vec3 : public Vec2<T> {
public:
	Vec3() : Vec2<T>(static_cast<T>(0.0), static_cast<T>(0.0)), z(static_cast<T>(0.0)), w(1.0) {}
	Vec3(const T& x, const T& y, const T& z, const double& w = 1.0)
		: Vec2<T>(x, y), z(z), w(w) {}

	// 类型转换
	explicit operator Vec3<int>() {
		return Vec3<int>(Round(this->x), Round(this->y), Round(this->z), w);
	}
	template <IsNumber T1>
	explicit operator Vec2<T1>() {
		return Vec2<T1>(this->x, this->y);
	}
	explicit operator Vec2<int>() {
		return Vec2<int>(Round(this->x), Round(this->y));
	}

public:
	// 中文：两向量相加
	Vec3<T> operator+(const Vec3<T>& rhs) const {
		Vec3<T> sum(this->x + rhs.x, this->y + rhs.y, this->z + rhs.z, this->w + rhs.w);
		return sum;
	}
	template<IsNumber T1>
	auto operator+(const Vec3<T1>& rhs) const {
		Vec3<decltype(rhs.x + this->x)> sum(this->x + rhs.x, this->y + rhs.y, this->z + rhs.z, this->w + rhs.w);
		return sum;
	}
	void operator+=(const Vec3<T>& rhs) {
		this->x += rhs.x;
		this->y += rhs.y;
		this->z += rhs.z;
		this->w += rhs.w;
	}
	template<IsNumber T1>
	void operator+=(const Vec3<T1>& rhs) {
		(*this) = this->operator+(rhs);
	}

	// English: two vector sub
	// 两向量相减
	template<IsNumber T1>
	auto operator-(const Vec3<T1>& rhs) const {
		Vec3<decltype(rhs.x - this->x)> sum(this->x - rhs.x, this->y - rhs.y, this->z - rhs.z, this->w - rhs.w);
		return sum;
	}
	template<IsNumber T1>
	void operator-=(const Vec3<T1>& rhs) {
		(*this) = this->operator-(rhs);
	}

	Vec3<T> operator-() {
		if (std::is_unsigned<T>()) throw std::bad_exception();
		return Vec3<T>(-this->x, -this->y, -this->z, -this->w);
	}

	// 乘法
#if _HAS_CXX20
	template<IsNumber T1>
	auto operator*(const T1& rhs) const {
		typedef decltype(rhs* this->x) T2;
		return Vec3<T2>(rhs * this->x, rhs * this->y, rhs * this->z, rhs * this->w);
	}
	template<IsNumber T1>
	void operator*=(const T1& rhs) {
		(*this) = this->operator*(rhs);
	}
	template<IsNumber T1>
	friend auto operator*(const T1& lhs, const Vec3<T>& rhs) {
		typedef decltype(lhs* rhs.x) T2;
		return Vec3<T2>(rhs.x * lhs, rhs.y * lhs, rhs.z * lhs, rhs.w * lhs);
	}
#else
	auto operator*(const double& rhs) const {
		return Vec3<T>(static_cast<T>(rhs * this->x), static_cast<T>(rhs * this->y), static_cast<T>(rhs * this->z), rhs * this->w);
	}

	void operator*=(const double& rhs) {
		(*this) = this->operator*(rhs);
	}

	friend auto operator*(const double& lhs, const Vec3<T>& rhs) {
		return rhs.operator*(lhs);
	}
#endif

	// 除法
#if _HAS_CXX20
	template<IsNumber T1>
	auto operator/(const T1& rhs) const {
		typedef decltype(rhs* this->x) T2;
		if (rhs == 0.0)
			return Vec3<T2>(static_cast<T2>(this->x), static_cast<T2>(this->y), static_cast<T2>(this->z), this->w);
		return Vec3<T2>(this->x / rhs, this->y / rhs, this->z / rhs, this->w / rhs);
	}
	template<IsNumber T1>
	void operator/=(const T1& rhs) {
		if (static_cast<double>(std::fabs(rhs)) < eps)
			return;
		(*this) = this->operator/(rhs);
	}
#else
	auto operator/(const double& rhs) const {
		if (rhs == 0.0) return *this;
		return Vec3<T>(static_cast<T>(this->x / rhs), static_cast<T>(this->y / rhs), static_cast<T>(this->z / rhs), this->w / rhs);
	}

	void operator/=(const double& rhs) {
		if (rhs == 0.0) return;
		(*this) = this->operator/(rhs);
	}
#endif

	bool operator==(const Vec3<T>& rhs) const {
		return (this->x == rhs.x && this->y == rhs.y && this->z == rhs.z);
	}

	bool operator!=(const Vec3<T>& rhs) const {
		return !this->operator==(rhs);
	}

	// [x / distance, y / distance, z / distance]
	// 归一化，将方向变为单位向量
	void Normalize() {
		double distance = this->Distance();
		if (distance == 0.0) return; // deal if distance is zero
		(*this) /= distance;
	}
	// 归一化，不改变自身
	Vec3<T> Normalized() {
		double distance = this->Distance();
		if (distance == 0.0) return *this;
		return (*this) / distance;
	}
	// 归一化，不改变自身
	Vec3<T> Normalized() const {
		double distance = this->Distance();
		if (distance == 0.0) return (*this);
		return (*this) / distance;
	}

	// √(x^2 + y^2 + z^2)
	// 距离
	double Distance() const {
		return std::sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
	}
	// x^2 + y^2 + z^2
	// 距离的平方，减少由于根号导致的精度问题
	double Length_Square() const {
		return (this->x * this->x + this->y * this->y + this->z * this->z);
	}

	// English: Dot product
	// x0 * x1 + y0 * y1 + z0 * z1
	// 点乘
	template<IsNumber T1, IsNumber T2>
	friend double Dot(const Vec3<T1>& lhs, const Vec3<T2>& rhs);

	// English: Cross product
	// [y1 * z2 - z0 * y1, z0 * x1 - x0 * z1, x0 * y1 - y0 * x1]
	// 叉乘 由lhs 指向 rhs
	template<IsNumber T1, IsNumber T2>
	friend auto Cross(const Vec3<T1>& lhs, const Vec3<T2>& rhs);

	// 矩阵运算
	friend Vec3<T> operator*(const Matrix& mat, const Vec3<T>& p) {
		return(Vec3<T>(mat.m[0][0] * p.x + mat.m[0][1] * p.y + mat.m[0][2] * p.z + mat.m[0][3]
			, mat.m[1][0] * p.x + mat.m[1][1] * p.y + mat.m[1][2] * p.z + mat.m[1][3]
			, mat.m[2][0] * p.x + mat.m[2][1] * p.y + mat.m[2][2] * p.z + mat.m[2][3]));
	}
public:
	T z;
	double w;
};

template<IsNumber T1, IsNumber T2>
inline double Dot(const Vec3<T1>& lhs, const Vec3<T2>& rhs)
{
	return static_cast<double>(lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z);
}

template<IsNumber T1, IsNumber T2>
inline auto Cross(const Vec3<T1>& lhs, const Vec3<T2>& rhs)
{
	typedef decltype(lhs.x* lhs.y) T3;
	Vec3<T3> sum(static_cast<T3>(lhs.y) * static_cast<T3>(rhs.z) -
		static_cast<T3>(lhs.z) * static_cast<T3>(rhs.y),
		static_cast<T3>(lhs.z) * static_cast<T3>(rhs.x) -
		static_cast<T3>(lhs.x) * static_cast<T3>(rhs.z),
		static_cast<T3>(lhs.x) * static_cast<T3>(rhs.y) -
		static_cast<T3>(lhs.y) * static_cast<T3>(rhs.x));
	return sum;
}

typedef Vec2<double> Point2d;
typedef Vec2<int> Point2i;
typedef Vec3<double> Point3d, Normal3d, Vector3d, Normal;
typedef Vec3<int> Point3i;

#define ZERO Point3d(0.0, 0.0, 0.0, 1.0)

#define XNormal Normal3d(1.0, 0.0, 0.0, 0.0)
#define YNormal Normal3d(0.0, 1.0, 0.0, 0.0)
#define ZNormal Normal3d(0.0, 0.0, 1.0, 0.0)

/*
* 颜色类，包含r,g,b,alpha
*/
class Color {
public:
	Color() { Color(0.6, 0.6, 0.6, 1.0); }
	Color(const double& r, const double& g, const double& b, const double& a = 1.0)
		:r(r), g(g), b(b), alpha(a) {};
public:
	auto operator+(const Color& c) const {
		return Color(this->r + c.r, this->g + c.g, this->b + c.b, this->alpha + c.alpha);
	}

	auto operator-(const Color& c) const {
		return Color(this->r - c.r, this->g - c.g, this->b - c.b, this->alpha - c.alpha);
	}

	friend auto operator*(const double& ratio, const Color& rhs)->Color {
		return Color(ratio * rhs.r, ratio * rhs.g, ratio * rhs.b, ratio * rhs.alpha);
	}

	Color operator*(const double& ratio) const {
		return Color(ratio * this->r, ratio * this->g, ratio * this->b, ratio * this->alpha);
	}

	Color operator*(const Color& rhs) const {
		return Color(this->r * rhs.r, this->g * rhs.g, this->b * rhs.b, this->alpha * rhs.alpha);
	}

	auto operator/(const double& ratio) const {
		if (ratio == 0.0) return (*this);
		return Color(this->r / ratio, this->g / ratio, this->b / ratio, this->alpha / ratio);
	}

	friend auto operator/(const double& ratio, const Color& rhs)->Color {
		if (rhs.r == 0.0 || rhs.g == 0.0 || rhs.b == 0.0 || rhs.alpha == .0)
			throw std::error_condition();
		return Color(ratio / rhs.r, ratio / rhs.g, ratio / rhs.b, ratio / rhs.alpha);
	}

	auto operator/(const Color& rhs) const {
		if (rhs.r == 0.0 || rhs.g == 0.0 || rhs.b == 0.0 || rhs.alpha == .0)
			throw std::error_condition();
		return Color(this->r / rhs.r, this->g / rhs.g, this->b / rhs.b, this->alpha / rhs.alpha);
	}

	// 改变内容
	// alert the content

	// add 加
	void operator+=(const Color& rhs) {
		this->r += rhs.r;
		this->g += rhs.g;
		this->b += rhs.b;
		this->alpha += rhs.alpha;
	}

	void operator-=(const Color& rhs) {
		this->r -= rhs.r;
		this->g -= rhs.g;
		this->b -= rhs.b;
		this->alpha -= rhs.alpha;
	}

	void operator*=(const double& ratio) {
		this->r *= ratio;
		this->g *= ratio;
		this->b *= ratio;
		this->alpha *= ratio;
	}

	void operator*=(const Color& c) {
		this->r *= c.r;
		this->g *= c.g;
		this->b *= c.b;
		this->alpha *= c.alpha;
	}

	void operator/=(const double& ratio) {
		if (ratio == 0.0) return;
		this->r /= ratio;
		this->g /= ratio;
		this->b /= ratio;
		this->alpha /= ratio;
	}

	bool operator<(const Color& c) {
		return ((this->r < c.r) && (this->g < c.g) && (this->b < c.b));
	}

	bool operator>(const Color& c) {
		return ((this->r > c.r) && (this->g > c.g) && (this->b > c.b));
	}

	bool operator<=(const Color& c) {
		return ((this->r <= c.r) && (this->g <= c.g) && (this->b <= c.b));
	}

	void Clamped() {
		this->r = this->r > 1.0 ? 1.0 : this->r < 0.0 ? 0.0 : this->r;
		this->g = this->g > 1.0 ? 1.0 : this->g < 0.0 ? 0.0 : this->g;
		this->b = this->b > 1.0 ? 1.0 : this->b < 0.0 ? 0.0 : this->b;
	}

	Color Clamp() {
		Color temp = *this;
		temp.Clamped();
		return temp;
	}

	Color Clamp() const {
		Color temp = *this;
		temp.Clamped();
		return temp;
	}
public:
	double r, g, b, alpha;
};

#define Black Color(0.0, 0.0, 0.0, 1.0)
#define Red Color(1.0, 0.0, 0.0, 1.0)
#define Green Color(0.0, 1.0, 0.0, 1.0)
#define Blue Color(0.0, 0.0, 1.0, 1.0)
#define White Color(1.0, 1.0, 1.0, 1.0)

typedef Color CRGB;

/*
* 纹理类，描述四个角的纹理坐标
*/
class Texture {
public:
	Texture() { Texture(0.0, 0.0); };
	Texture(const double& u, const double& v) :u(u), v(v) {};
public:
	Texture operator+(const Texture& rhs) const {
		return Texture(this->u + rhs.u, this->v + rhs.v);
	}
	Texture operator-(const Texture& rhs) const {
		return Texture(this->u - rhs.u, this->v - rhs.v);
	}
	Texture operator*(const double& ratio) const {
		return Texture(this->u * ratio, this->v * ratio);
	}
	friend auto operator*(const double& ratio, const Texture& t) {
		return t.operator*(ratio);
	}
	Texture operator/(const double& ratio) const {
		if (ratio == 0) throw std::logic_error("zero");
		return Texture(this->u / ratio, this->v / ratio);
	}
public:
	double u, v;
};

#define BL Texture(0.0, 0.0)
#define BR Texture(1.0, 0.0)
#define TL Texture(0.0, 1.0)
#define TR Texture(1.0, 1.0)

template <IsNumber T>
class Point {
public:
	Point() { Point(static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(0.0)); }
	Point(const T& x, const T& y, const T& z, const T& w = 1.0) {
		this->position.x = x;
		this->position.y = y;
		this->position.z = z;
	}
	Point(const T& x, const T& y,const Color& c) {
		this->position.x = x;
		this->position.y = y;
		this->excident = c;
	}
	Point(const T& x, const T& y, const T& z, const Color& c) {
		this->position.x = x;
		this->position.y = y;
		this->position.z = z;
		this->excident = c;
	}
public:
	// 赋值运算符
	Point<T>& operator=(const Point<T>& rhs) {
		if (this == &rhs) return *this;
		this->position = rhs.position;
		this->normal = rhs.normal;
		this->excident = rhs.excident;
		this->texture = rhs.texture;
		return *this;
	}
	// 复制点的信息（不进行运算符重载了）
	void Copy(const Point<T>& rhs) {
		this->position = rhs.position;
		this->excident = rhs.excident;
		this->normal = rhs.normal;
		this->texture = rhs.texture;
	}
public:
	Vec3<T> position;           // 坐标
	Normal normal = XNormal;    // 点向量
	Texture texture = BL;       // 纹理坐标
	Color incident = Black;     // 颜色
	Color excident = Black;     // 颜色
};
typedef Point<double> CP3, CP2, P3, P2;
typedef Point<int> CPoint2, CPoint3, Point2, Point3;

/*
* ray 光线类， o + t * d 可以描述从o出发，方向为d 距离为t的一个点
*/
class Ray {
public:
	Ray() {};
	Ray(const Point3d& o, const Vector3d& d) :o(o), d(d) {};
public:
	Point3d o = ZERO;
	Vector3d d = ZNormal;
};

class Image {
public:
	Image() {}
	void SetResource(UINT nIDResource) {
		CBitmap NewBitmap;
		NewBitmap.LoadBitmapW(nIDResource);
		NewBitmap.GetBitmap(&bmp);
		int nbytesize = bmp.bmWidthBytes * bmp.bmHeight;
		image = std::make_unique<BYTE[]>(nbytesize);
	}
public:
	std::unique_ptr<BYTE[]> image{ 0 };   // 图片内容，以颜色保存
	BITMAP bmp{ 0 };                      // 保存图片信息，宽高
};


// 1. 声明 可变模板类
template <class... T> class Expand {};
// 2. 偏特化定义（递归开始）
template <class T, class... Args>
class Expand<T, Args...> : public Expand<Args...> {
public:

	typedef Expand<Args...> NextType; // 表示下一个类型

	/* 展开参数包 */
	Expand() = delete;
	// 注意：这个无参数构造一般会出现问题（对于传递的
	// 类型没有默认构造函数时），建议不使用

	/* 默认的复制构造函数 */
	explicit Expand(const T& val, const Args&... next) {
		this->data = val;
		// printf("type: %s\n", typeid(T).name());
	}

	/* 获取下一个类型的变量 */
	NextType& next() { return *this; }

	T data;
};
// 3. 终止递归调用
template<> class Expand<> {}; // 边界条件


/**
*
* 对于上述Expand<T, Args>的使用方法
* 可以见tuple的使用方法。
*
* 使用方法如下：
* Expand<float, int, double> point;
* auto number1 = point.data;               // 此时number1为float
* auto number2 = point.next().data;        // 此时number2为int
* auto number3 = point.next().next().data; // 此时number3为double
*
* 如，带颜色的点可以使用
* Expand<Point3d, Color> point;
* 如，带矢量和颜色的点可以使用
* Expand<Point3d, Color, Vector3d> point;
* point.data; // 此时的data为点的坐标
* point.data.x, point.data.y, point.data.z;
*
* point.next().data; // 此时的data为颜色
* point.next().data.R, point.next().data.G, point.next().data.B
* ...
*
* reference: https://blog.csdn.net/qq_38410730/article/details/105247065
*
**/