#ifndef MATH_ROTATION_H
#define MATH_ROTATION_H

#ifdef _WIN32
#pragma once
#endif

#include <math.h>
#include <float.h>
#include "vector.h"

// quaternion creation from vector is needed
// also getting a vector from a quaternion
// functions to get right, up, and forward from quaternion
// Use vectors for holding rotations

// math.h uses radians, so we gotta figure something out with radians and degrees
// 90 degrees is not definable in a radian float, which is silly that everything is in radians
// even with converting to radians there is loss

// matrices don't use radians, quaternions don't either
// Maybe try to setup a rotation conversion without any of this

// we can mod 180 or 90 for vector, but still we don't have cos or sin
// If all our rotations are strictly vector based we should be fine

// Quaternions also are really similiar to normalized vectors which are great!

struct quat {
	vector_type x;
	vector_type y;
	vector_type z;
	vector_type w;
	
	inline quat(void) {
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
		w = 1.0f;
	};
	
	inline quat& set_from_euler(v3& v) {
		float c1 = cosf(v.x * 0.5f);
		float c2 = cosf(v.y * 0.5f);
		float c3 = cosf(v.z * 0.5f);
		float s1 = sinf(v.x * 0.5f);
		float s2 = sinf(v.y * 0.5f);
		float s3 = sinf(v.z * 0.5f);
		
		this->x = (s1 * c2 * c3) + (c1 * s2 * s3);
		this->y = (c1 * s2 * c3) - (s1 * c2 * s3);
		this->z = (c1 * c2 * s3) + (s1 * s2 * c3);
		this->w = (c1 * c2 * c3) - (s1 * s2 * s3);
		
		return *this;
	};

	inline quat& set_from_ypr(float yaw, float pitch, float roll) {
		float c1 = cosf(yaw * 0.5f);
		float c2 = cosf(pitch * 0.5f);
		float c3 = cosf(roll * 0.5f);
		float s1 = sinf(yaw * 0.5f);
		float s2 = sinf(pitch * 0.5f);
		float s3 = sinf(roll * 0.5f);

		this->x = (s1 * c2 * c3) + (c1 * s2 * s3);
		this->y = (c1 * s2 * c3) - (s1 * c2 * s3);
		this->z = (c1 * c2 * s3) + (s1 * s2 * c3);
		this->w = (c1 * c2 * c3) - (s1 * s2 * s3);

		return *this;
	};

	// For simple creation of rotations
	inline quat& set_from_euler2(v3& v) {
		float c1 = cosf(v.x / 2);
		float c2 = cosf(v.y / 2);
		float s1 = sinf(v.x / 2);
		float s2 = sinf(v.y / 2);

		this->x = s1 * c2;
		this->y = c1 * s2;
		this->z = s1 * s2;
		this->w = c1 * c2;

		return *this;
	};

	inline quat& set_from_pitchyaw(float &pitch, float &yaw) {
		float c1 = cosf(pitch / 2);
		float c2 = cosf(yaw / 2);
		float s1 = sinf(pitch / 2);
		float s2 = sinf(yaw / 2);

		this->x = s1 * c2;
		this->y = c1 * s2;
		this->z = s1 * s2;
		this->w = c1 * c2;

		return *this;
	};
	
	inline quat& set_from_pitchyawroll(float pitch, float yaw, float roll) {
		float c_roll = cosf(0.0f);
		float c_pitch = cosf(pitch * 0.5f);
		float c_yaw = cosf(yaw * 0.5f);
		
		float s_roll = sinf(0.0f);
		float s_pitch = sinf(pitch * 0.5f);
		float s_yaw = sinf(yaw * 0.5f);

		this->x = (c_yaw * s_pitch * c_roll) + (s_yaw * c_pitch * s_roll);
		this->y = (s_yaw * c_pitch * c_roll) - (c_yaw * s_pitch * s_roll);
		this->z = (c_yaw * c_pitch * s_roll) - (s_yaw * s_pitch * c_roll);
		this->w = (c_yaw * c_pitch * c_roll) + (s_yaw * s_pitch * s_roll);

		return *this;
	};
	
	inline quat& set_from_axis_angle(v3& axis, float angle) {
		float half_angle = angle / 2.0f;
		float s = sinf(half_angle);
		
		this->x = axis.x * s;
		this->y = axis.y * s;
		this->z = axis.z * s;
		this->w = cosf(half_angle);
		
		return *this;
	};
	
	inline quat& conjugate() {
		this->x *= -1;
		this->y *= -1;
		this->z *= -1;
		
		return *this;
	};
	
	// Multiplying quaternions is how we rotate
	inline quat& operator*(quat& q) {
		auto result = quat();
		
		result.x = (this->x * q.w) + (this->w * q.x) + (this->y * q.z) - (this->z * q.y);
		result.y = (this->y * q.w) + (this->w * q.y) + (this->z * q.x) - (this->x * q.z);
		result.z = (this->z * q.w) + (this->w * q.z) + (this->x * q.y) - (this->y * q.x);
		result.w = (this->w * q.w) - (this->w * q.x) - (this->y * q.y) - (this->z * q.z);
		
		return result;
	};
	
	inline float length_sqr() {
		return (this->x * this->x) + (this->y * this->y) + (this->z * this->z) + (this->w * this->w);
	};

	inline float length() {
		return sqrtf((this->x * this->x) + (this->y * this->y) + (this->z * this->z) + (this->w * this->w));
	};
	
	inline quat& normalize() {
		float l = this->length();
		
		if (l == 0) {
			this->x = 0.0f;
			this->y = 0.0f;
			this->z = 0.0f;
			this->w = 0.0f;
		} else {
			float il = 1.0f / (l + FLT_EPSILON);
			
			this->x = this->x * il;
			this->y = this->y * il;
			this->z = this->z * il;
			this->w = this->w * il;
		};

		return *this;
	};
	
	inline quat& inverse() {
		this->conjugate();
		this->normalize();
		return *this;
	};
	
	// we need from matrix
	
	inline quat& set_from_vectors(v3& vector_from, v3& vector_to) {
		// Assumes vectors are normal
		v3 v = v3();
		float r = vector_from.dot(vector_to) + 1;
		
		// Need to find another example of these functions lol
		if (r < 0.000001)
		{
			r = 0.0f;
		} else {
			v = vector_from.cross(vector_to);
		};
		
		this->x = v.x;
		this->y = v.y;
		this->z = v.z;
		this->w = r;
		
		this->normalize();

		return *this;
	};
};

#endif
