#ifndef _VECTOR2_H_
#define _VECTOR2_H_

#ifndef PI
#define PI 3.1415926f
#endif

#include <math.h>

class Vector2
{
public:
	float x = 0;
	float y = 0;

public:
	Vector2() = default;
	~Vector2() = default;

	Vector2(float x, float y)
		: x(x), y(y) { }

	Vector2 operator+(const Vector2& vec) const
	{
		return Vector2(x + vec.x, y + vec.y);
	}

	Vector2 operator-(const Vector2& vec) const
	{
		return Vector2(x - vec.x, y - vec.y);
	}

	void operator+=(const Vector2& vec)
	{
		x += vec.x, y += vec.y;
	}

	void operator-=(const Vector2& vec)
	{
		x -= vec.x, y -= vec.y;
	}

	float operator*(const Vector2& vec) const
	{
		return x * vec.x + y * vec.y;
	}

	Vector2 operator*(float val) const
	{
		return Vector2(x * val, y * val);
	}

	void operator*=(float val)
	{
		x *= val, y *= val;
	}

	bool operator==(const Vector2& vec) const
	{
		return (x == vec.x && y == vec.y);
	}

	float length() const
	{
		return sqrt(x * x + y * y);
	}

	Vector2 normalize() const
	{
		float len = length();

		if (len == 0)
			return Vector2(0, 0);

		return Vector2(x / len, y / len);
	}

	Vector2 rotate(float a) const
	{
		float b = a * PI / 180.0f;
		int b_i = (int)b;
		float b_f = b - b_i;
		b = b_i % 360 + b_f;

		float len = length();
		float fsin_b = sin(b), fcos_b = cos(b);

		return Vector2(x * fcos_b - y * fsin_b, y * fcos_b + fsin_b * x);
	}

	float angle_between(const Vector2& vec) const
	{
		float len_a = length();
		float len_b = vec.length();
		float alpha = acos((*this) * vec / (len_a * len_b)) * 180.0f / PI;

		return alpha;
	}

};

inline float distance(const Vector2& vec1, const Vector2& vec2)
{
	return (vec1 - vec2).length();
}

inline float dot(const Vector2& vec1, const Vector2& vec2)
{
	return vec1 * vec2;
}

#endif //!_VECTOR2_H_
