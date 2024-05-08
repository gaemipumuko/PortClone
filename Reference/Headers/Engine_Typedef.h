#pragma once

namespace Engine
{
	/* ���α׷����� ���Ǵ� ���� Ÿ�Ե��� �������Ѵ�.  */
	typedef char						_char;
	typedef wchar_t						_tchar;

	typedef unsigned char				_ubyte;
	typedef signed char					_byte;

	typedef unsigned short				_ushort;
	typedef signed short				_short;

	typedef unsigned int				_uint;
	typedef signed int					_int;

	typedef unsigned long				_ulong;
	typedef signed long					_long;

	typedef float						_float;
	typedef double						_double;

	typedef bool						_bool;

	/* ����� Vector Type */
	typedef XMFLOAT2					_float2;
	typedef XMFLOAT3					_float3;
	typedef XMFLOAT4					_float4;

	typedef XMFLOAT4X4					_float4x4;

	/* ����� Vector Type */
	typedef XMVECTOR					_vector;
	typedef FXMVECTOR					_fvector;
	typedef GXMVECTOR					_gvector;
	
	typedef XMMATRIX					_matrix;
	typedef FXMMATRIX					_fmatrix;
	typedef CXMMATRIX					_cmatrix;
}