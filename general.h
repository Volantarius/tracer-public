#ifndef GENERAL_H
#define GENERAL_H
#ifdef _WIN32
#pragma once
#endif

#pragma warning(disable:4244) // possible loss of data conversion warnings

#define _CRT_SECURE_NO_WARNINGS

#define PI 3.14159265358979323846

#define V_MAX_VERTICES 8192

#define V_DEBUG

// When disabled: Delays SDL by 1ms to stay within 60fps
#define V_GLVSYNC 1

typedef unsigned char u8;
typedef char s8;

typedef unsigned short u16;
typedef short s16;

// long
typedef unsigned long u32;
typedef long s32;

inline int read_until_from_right(unsigned char *buffer, int index, char char_term) {
	int count = 0;

	for (int i = 0; i < 4; i++) {
		if (char_term == ((char)buffer[index + i]) || '\x0A' == ((char)buffer[index + i])) {
			count = i;
			break;
		}
	}

	return count;
};

inline void v_power(int exp, int &out) {
	out = 1;

	for (int i = 0; i < exp; i++) {
		out *= 10;
	}
};

inline int read_as_number_from_right(unsigned char *buffer, int digits, unsigned int readpos)
{
	int final = 0;
	int temp = 1;

	// Read next bytes as ascii numbers, which we simply strip off the ascii part
	for (int i = 0; i < digits; i++)
	{
		v_power(digits - i - 1, temp);
		
		final += temp * (((int)buffer[readpos + i]) - 48);
	}

	return final;
};

inline void v_read_uint32(unsigned char *buffer, unsigned int &out, unsigned int readpos)
{
	out = 0;
	
	out += (unsigned char)buffer[readpos + 0];
	out += (unsigned char)buffer[readpos + 1] << 8;
	out += (unsigned char)buffer[readpos + 2] << 16;
	out += (unsigned char)buffer[readpos + 3] << 24;
};

inline void v_read_uint4(unsigned char *buffer, unsigned int &out, unsigned int readpos)
{
	out = 0;
	
	out += (unsigned char)buffer[readpos];
};

inline void v_read_int32(unsigned char *buffer, int &out, unsigned int readpos)
{
	out = 0;
	
	out |= (unsigned char)buffer[readpos + 0];
	out |= (unsigned char)buffer[readpos + 1] << 8;
	out |= (unsigned char)buffer[readpos + 2] << 16;
	out |= (unsigned char)buffer[readpos + 3] << 24;
};

inline void v_read_float(unsigned char *buffer, float &out, unsigned int readpos)
{
	// Fuuu slow
	unsigned int final = 0;
	
	final |= ((unsigned char)buffer[readpos + 0]);
	final |= ((unsigned char)buffer[readpos + 1] << 8);
	final |= ((unsigned char)buffer[readpos + 2] << 16);
	final |= ((unsigned char)buffer[readpos + 3] << 24);
	
	out = reinterpret_cast<float &>(final);
};

#endif