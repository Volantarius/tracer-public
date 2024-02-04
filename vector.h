#ifndef MATH_VECTOR_H
#define MATH_VECTOR_H

#ifdef _WIN32
#pragma once
#endif

//#include <assert.h>
#include <math.h>
#include <float.h>

typedef float vector_type;

struct v2 {
	vector_type x;
	vector_type y;

	inline v2(void) {
		x = 0.0f;
		y = 0.0f;
	};

	inline v2(vector_type xx, vector_type yy) {
		x = xx;
		y = yy;
	};
	
	inline bool operator==(v2& v) {
		return (v.x == x) && (v.y == y);
	};

	inline bool operator!=(v2& v) {
		return (v.x != x) || (v.y != y);
	};
	
	inline v2& operator+=(v2& v) {
		x += v.x;
		y += v.y;
		return *this;
	};

	inline v2 operator+(v2& v) {
		v2 result(x + v.x, y + v.y);

		return result;
	};
	
	inline v2& operator-=(v2& v) {
		x -= v.x;
		y -= v.y;
		return *this;
	};

	inline v2 operator-(v2& v) {
		v2 result(x - v.x, y - v.y);

		return result;
	};
	
	inline v2& operator*=(v2& v) {
		x *= v.x;
		y *= v.y;
		return *this;
	};

	inline v2 operator*(v2& v) {
		v2 result(x * v.x, y * v.y);

		return result;
	};
	
	// TODO ADD ASSERT BECAUSE ZERO DIVISION LOL
	inline v2& operator/=(v2& v) {
		x /= v.x;
		y /= v.y;
		return *this;
	};

	inline v2 operator/(v2& v) {
		v2 result(x / v.x, y / v.y);

		return result;
	};
	
	inline v2& operator+=(float f) {
		x += f;
		y += f;
		return *this;
	};
	
	inline v2& operator-=(float f) {
		x -= f;
		y -= f;
		return *this;
	};
	
	inline v2& operator*=(float f) {
		x *= f;
		y *= f;
		return *this;
	};
	
	// TODO ASSERT
	inline v2& operator/=(float f) {
		x /= f;
		y /= f;
		return *this;
	};
	
	inline vector_type operator[](int i) {
		// cast to a pointer then reference the thing
		// need assert lol
		return ((vector_type*) this)[i];
	};

	inline float length_sqr() {
		return (this->x * this->x) + (this->y * this->y);
	};

	inline float length() {
		return sqrtf((this->x * this->x) + (this->y * this->y));
	};

	inline float dot(v2& v) {
		return (this->x * v.x) + (this->y * v.y);
	};

	inline v2& normalize() {
		float dist = this->length_sqr();

		float idist = 1.0f / (dist + FLT_EPSILON);

		this->x = this->x * dist;
		this->y = this->y * dist;

		return *this;
	};

	inline v2 normalized() {
		v2 final = *this;
		final.normalize();
		return final;
	};
};

struct v3 {
	vector_type x;
	vector_type y;
	vector_type z;
	
	inline v3(void) {
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	};
	
	inline v3(vector_type xx, vector_type yy, vector_type zz) {
		x = xx;
		y = yy;
		z = zz;
	};

	inline v3(vector_type xx) {
		x = xx;
		y = xx;
		z = xx;
	};

	inline v3(v2 v, vector_type zz) {
		x = v.x;
		y = v.y;
		z = zz;
	};
	
	inline bool operator==(const v3 &v);
	inline bool operator!=(const v3 &v);

	inline v3& operator+=(const v3 &v);
	inline v3  operator+(const v3 &v);
	inline v3& operator-=(const v3 &v);
	inline v3  operator-(const v3 &v);
	inline v3& operator*=(const v3 &v);
	inline v3  operator*(const v3 &v);
	//inline v3& operator/=(v3 &v);
	//inline v3  operator/(v3 &v);

	inline v3& operator+=(const vector_type &f);
	inline v3& operator-=(const vector_type &f);
	inline v3& operator*=(const vector_type &f);

	inline v3 operator*(const vector_type &f);

	vector_type operator[](int i);

	inline vector_type length_sqr();
	inline vector_type length();
	inline vector_type dot(v3 &v);
	inline v3 cross(v3 &v);

	inline v3& normalize();
	inline v3 normalized();
};

inline void v3_add(const v3 &a, const v3 &b, v3 &c) {
	c.x = a.x + b.x;
	c.y = a.y + b.y;
	c.z = a.z + b.z;
};

inline void v3_subtract(const v3 &a, const v3 &b, v3 &c) {
	c.x = a.x - b.x;
	c.y = a.y - b.y;
	c.z = a.z - b.z;
};

inline void v3_multiply(const v3 &a, const v3 &b, v3 &c) {
	c.x = a.x * b.x;
	c.y = a.y * b.y;
	c.z = a.z * b.z;
};

inline void v3_divide(const v3 &a, const v3 &b, v3 &c) {
	c.x = a.x / b.x;
	c.y = a.y / b.y;
	c.z = a.z / b.z;
};

// Scalars
/*inline void v3_add(const v3 &a, vector_type &f, v3 &c) {
	c.x = a.x + f;
	c.y = a.y + f;
	c.z = a.z + f;
};*/

inline void v3_add(const v3 &a, const vector_type &f, v3 &c) {
	c.x = a.x + f;
	c.y = a.y + f;
	c.z = a.z + f;
};

/*inline void v3_subtract(const v3 &a, vector_type &f, v3 &c) {
	c.x = a.x - f;
	c.y = a.y - f;
	c.z = a.z - f;
};*/

inline void v3_subtract(const v3 &a, const vector_type &f, v3 &c) {
	c.x = a.x - f;
	c.y = a.y - f;
	c.z = a.z - f;
};

/*inline void v3_multiply(const v3 &a, vector_type &f, v3 &c) {
	c.x = a.x * f;
	c.y = a.y * f;
	c.z = a.z * f;
};*/

inline void v3_multiply(const v3 &a, const vector_type &f, v3 &c) {
	c.x = a.x * f;
	c.y = a.y * f;
	c.z = a.z * f;
};

/*inline void v3_divide(const v3 &a, vector_type &f, v3 &c) {
	c.x = a.x / f;
	c.y = a.y / f;
	c.z = a.z / f;
};*/

inline void v3_divide(const v3 &a, const vector_type &f, v3 &c) {
	c.x = a.x / f;
	c.y = a.y / f;
	c.z = a.z / f;
};

inline void v3_to_radians(const v3 &in, v3 &out) {
	out.x = (vector_type)(in.x) * (float)(PI / 180.0f);
	out.y = (vector_type)(in.y) * (float)(PI / 180.0f);
	out.z = (vector_type)(in.z) * (float)(PI / 180.0f);
};

// Create a vector from euler angle vector
// Does not consider roll
// TODO: Why is it all over the place.. ALSO ITS STILL ROLLING
inline void v3_deulerize(const v3 &in, v3 &out) {
	/*
	out.x = cosf(in.y) * cosf(in.x);
	out.y = sinf(in.y) * cosf(in.x);
	out.z = sinf(in.x);
	*/

	out.z = cosf(in.y) * cosf(in.x);
	out.x = - sinf(in.y) * cosf(in.x);
	out.y = sinf(in.x);
};

inline vector_type v3_dot(v3 &a, v3 &b) {
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
};

inline vector_type v3_dot(const v3 &a, const v3 &b) {
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
};

inline void v3_cross(const v3 &a, const v3 &b, v3 &c) {
	c.x = (a.y * b.z) - (a.z * b.y);
	c.y = (a.z * b.x) - (a.x * b.z);
	c.z = (a.x * b.y) - (a.y * b.x);
};

inline void v3_lerp(const v3 &s, const v3 &s2, const vector_type t, v3 &d) {
	d.x = s.x + (s2.x - s.x) * t;
	d.y = s.y + (s2.y - s.y) * t;
	d.z = s.z + (s2.z - s.z) * t;
};

inline vector_type v3_length_sqr(const v3 &v) {
	return (v.x * v.x) + (v.y * v.y) + (v.z * v.z);
};

inline vector_type v3_length(const v3 &v) {
	return sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
};

inline void v3_normalize(v3 &a, v3 &b) {
	float l = a.length();

	float il = 1.0f / (l + FLT_EPSILON);

	b.x = a.x * il;
	b.y = a.y * il;
	b.z = a.z * il;
};

// Calculate the normal of a face
inline void v3_face_normal(v3 &a, v3 &b, v3 &c, v3 &out) {
	//v3 cross = v3();
	v3_cross(b - a, c - a, out);
	v3_normalize(out, out);
};

// V3 STRUCT DEFINES

inline bool v3::operator==(const v3 &v) {
	return (v.x == x) && (v.y == y) && (v.z == z);
};

inline bool v3::operator!=(const v3 &v) {
	return (v.x != x) || (v.y != y) || (v.z != z);
};

inline v3& v3::operator+=(const v3 &v) {
	v3_add(*this, v, *this);

	return *this;
};

inline v3 v3::operator+(const v3 &v) {
	v3 result;

	v3_add(*this, v, result);

	return result;
};

inline v3& v3::operator-=(const v3 &v) {
	v3_subtract(*this, v, *this);

	return *this;
};

inline v3 v3::operator-(const v3 &v) {
	v3 result;

	v3_subtract(*this, v, result);

	return result;
};

inline v3& v3::operator*=(const v3 &v) {
	v3_multiply(*this, v, *this);

	return *this;
};

inline v3 v3::operator*(const v3 &v) {
	v3 result;

	v3_multiply(*this, v, result);

	return result;
};

// OTHER

inline v3& v3::operator+=(const vector_type &f) {
	x += f;
	y += f;
	z += f;
	return *this;
};

inline v3& v3::operator-=(const vector_type &f) {
	x -= f;
	y -= f;
	z -= f;
	return *this;
};

inline v3& v3::operator*=(const vector_type &f) {
	x *= f;
	y *= f;
	z *= f;
	return *this;
};

inline v3 v3::operator*(const vector_type &f) {
	v3 result = v3();
	v3_multiply(*this, f, result);
	return result;
};

inline vector_type v3::operator[](int i) {
	// cast to a pointer then reference the thing
	// need assert lol
	return ((vector_type*)this)[i];
};

inline vector_type v3::length_sqr() {
	return v3_length_sqr(*this);
};

inline vector_type v3::length() {
	return v3_length(*this);
};

inline vector_type v3::dot(v3& v) {
	return v3_dot(*this, v);
};

inline v3 v3::cross(v3& v) {
	v3 result;

	v3_cross(*this, v, result);

	return result;
};

inline v3& v3::normalize() {
	v3_normalize(*this, *this);

	return *this;
};

inline v3 v3::normalized() {
	v3 result;

	v3_normalize(*this, result);

	return result;
};

// IsZero
// min max clamp
// reflect
// clear can be v3 * 0.0f
#endif