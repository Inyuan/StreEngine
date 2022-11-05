#pragma once
#include <stdint.h>

/***
************************************************************
*
* Base Type
*
************************************************************
*/

///base type
struct s_uid
{
	char name[256];
};


struct s_float
{
	float x = 0;
	s_float(float in_x = 0) : x(in_x) {};
};
struct s_float2
{
	float x = 0;
	float y = 0;
	s_float2(float in_x = 0, float in_y = 0) : x(in_x), y(in_y) {};
};
struct s_float3
{
	float x = 0;
	float y = 0;
	float z = 0;
	s_float3(float in_x = 0, float in_y = 0, float in_z = 0) : x(in_x), y(in_y), z(in_z) {};

};

struct s_float4
{
	float x = 0;
	float y = 0;
	float z = 0;
	float w = 0;
	s_float4(float in_x = 0, float in_y = 0, float in_z = 0, float in_w = 0) : x(in_x), y(in_y), z(in_z), w(in_w) {};

};

struct s_float4x4
{
	union
	{
		struct
		{
			float _11, _12, _13, _14;
			float _21, _22, _23, _24;
			float _31, _32, _33, _34;
			float _41, _42, _43, _44;
		};
		float m[4][4];
	};

	static s_float4x4 identity4x4()
	{
		static s_float4x4 I(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);

		return I;
	}

	s_float4x4(float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33)
		: _11(m00), _12(m01), _13(m02), _14(m03),
		_21(m10), _22(m11), _23(m12), _24(m13),
		_31(m20), _32(m21), _33(m22), _34(m23),
		_41(m30), _42(m31), _43(m32), _44(m33) {}
};



struct s_vertex
{
	s_float3 position{ 0,0,0 };
	s_float3 normal{ 0,0,0 };
	s_float2 texC{ 0,0 };
	s_float3 tangentU{ 0,0,0 };

	s_vertex()
	{
		position = { 0,0,0 };
		normal = { 0,0,0 };
		texC = { 0,0 };
		tangentU = { 0,0,0 };
	}

	s_vertex(
		const s_float3& p,
		const s_float3& n,
		const s_float3& t,
		const s_float2& uv,
		const s_float2& mid) :
		position(p),
		normal(n),
		tangentU(t),
		texC(uv) {}
	s_vertex(
		float px, float py, float pz,
		float nx, float ny, float nz,
		float tx, float ty, float tz,
		float u, float v,
		int id) :
		position(px, py, pz),
		normal(nx, ny, nz),
		tangentU(tx, ty, tz),
		texC(u, v) {}

};


typedef uint32_t s_index;
typedef char s_texture;