#ifndef MATH_MATRIX_H
#define MATH_MATRIX_H

#ifdef _WIN32
#pragma once
#endif

#include <math.h>
#include <float.h>
#include "vector.h"
//#include "rotation.h"

// Need to setup a transposed matrix set, so that we don't have to tell Open GL to transpose

struct mat4 {
	vector_type m00;
	vector_type m01;
	vector_type m02;
	vector_type m03;
	
	vector_type m04;
	vector_type m05;
	vector_type m06;
	vector_type m07;
	
	vector_type m08;
	vector_type m09;
	vector_type m10;
	vector_type m11;
	
	vector_type m12;
	vector_type m13;
	vector_type m14;
	vector_type m15;
	
	inline mat4() {
		m00 = 1.0f;
		m01 = 0.0f;
		m02 = 0.0f;
		m03 = 0.0f;

		m04 = 0.0f;
		m05 = 1.0f;
		m06 = 0.0f;
		m07 = 0.0f;

		m08 = 0.0f;
		m09 = 0.0f;
		m10 = 1.0f;
		m11 = 0.0f;

		m12 = 0.0f;
		m13 = 0.0f;
		m14 = 0.0f;
		m15 = 1.0f;
	};

	/*inline v3 translation() {
		return v3(this->m12, this->m13, this->m14);
	};*/

	// Hmmm so yeah, when used for world translation, its backwards don't ya think? lol
	inline mat4& translate(v3& v) {
		this->m12 = -v.x;
		this->m13 = -v.y;
		this->m14 = -v.z;
		
		return *this;
	};
	
	/*inline mat4& from_pitchyaw(float &pitch, float &yaw) {
		float e = cosf(0.0f);
		float a = cosf(pitch);
		float c = cosf(yaw);

		float f = sinf(0.0f);
		float b = sinf(pitch);
		float d = sinf(yaw);

		float ae = a * e;
		float af = a * f;
		float be = b * e;
		float bf = b * f;

		this->m00 = c * e;
		this->m04 = - c * f;
		this->m08 = d;

		this->m01 = af + be * d;
		this->m05 = ae - bf * d;
		this->m09 = - b * c;

		this->m02 = bf - ae * d;
		this->m06 = be + af * d;
		this->m10 = a * c;

		this->m03 = 0.0f;
		this->m07 = 0.0f;
		this->m11 = 0.0f;

		this->m12 = 0.0f;
		this->m13 = 0.0f;
		this->m14 = 0.0f;
		this->m15 = 1.0f;

		return *this;
	};*/
	
	// Needs to be pointers
	/*inline mat4& look_at(v3& eye, v3& target, v3& up) {
		v3 forward = eye - target;
		forward.normalize();

		// side
		v3 vector2 = up.cross(forward);
		vector2.normalize();

		// new up
		v3 vector3 = forward.cross(vector2);
		
		this->m00 = vector2.x;
		this->m01 = vector3.x;
		this->m02 = forward.x;
		this->m03 = 0;
		
		this->m04 = vector2.y;
		this->m05 = vector3.y;
		this->m06 = forward.y;
		this->m07 = 0;
		
		this->m08 = vector2.z;
		this->m09 = vector3.z;
		this->m10 = forward.z;
		this->m11 = 0;

		this->m12 = -vector2.dot(eye);
		this->m13 = -vector3.dot(eye);
		this->m14 = -forward.dot(eye);
		this->m15 = 1;

		return *this;
	};*/

	inline mat4& look_at(v3& forward, v3& up) {
		v3 vector2 = v3();
		v3 vector3 = v3();

		// side
		v3_cross(up, forward, vector2);
		vector2.normalize();

		// new up
		v3_cross(forward, vector2, vector3);
		vector3.normalize();

		this->m00 = vector2.x;
		this->m01 = vector3.x;
		this->m02 = forward.x;
		this->m03 = 0.0f;
		
		this->m04 = vector2.y;
		this->m05 = vector3.y;
		this->m06 = forward.y;
		this->m07 = 0.0f;
		
		this->m08 = vector2.z;
		this->m09 = vector3.z;
		this->m10 = forward.z;
		this->m11 = 0.0f;

		this->m12 = 0.0f;
		this->m13 = 0.0f;
		this->m14 = 0.0f;
		this->m15 = 1.0f;

		return *this;
	};

	// What
	/*inline mat4& make_perspective(float fov, float aspect, float near, float far) {
		float yscale = 1 / tanf(((fov * PI) / 180.f) * 0.5f);
		float xscale = yscale / aspect;
		float negative_farrange = far / (near - far);

		this->m00 = xscale;
		this->m01 = this->m02 = this->m03 = 0;

		this->m05 = yscale;
		this->m04 = this->m06 = this->m07 = 0;

		this->m08 = this->m09 = 0;
		this->m10 = negative_farrange;
		this->m11 = -1;

		this->m12 = this->m13 = this->m15 = 0;
		this->m14 = near * negative_farrange;

		return *this;
	};*/

	inline mat4& make_infinte_perspective(float fov, float aspect, float near) {
		float yscale = 1 / tanf(((fov * PI) / 180.f) * 0.5f);
		float xscale = yscale / aspect;
		
		this->m00 = xscale;
		this->m01 = this->m02 = this->m03 = 0.0f;

		this->m05 = yscale;
		this->m04 = this->m06 = this->m07 = 0.0f;

		this->m08 = this->m09 = this->m10 = 0.0f;
		this->m11 = -1.0f;

		this->m12 = this->m13 = this->m15 = 0.0f;
		this->m14 = near;

		return *this;
	};

	/*inline mat4 operator*(mat4& m) {
		mat4 result = mat4();
		
		result.m00 = this->m00 * m.m00 + this->m01 * m.m04 + this->m02 * m.m08 + this->m03 * m.m12;
		
		return result;
	};*/

	// conversion function, lol are these fast??
	// I would hope its fast, its just a cast
	inline operator float*() {
		return (float *)this;
	};
};

#endif
