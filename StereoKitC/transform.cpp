#include "stereokit.h"

#include "math.h"

#include <directxmath.h> // Matrix math functions and objects
using namespace DirectX;

///////////////////////////////////////////

inline XMVECTOR to_fast3  (const vec3 &vec) { return XMLoadFloat3((XMFLOAT3 *)& vec); }
inline vec3     from_fast3(const XMVECTOR &a) { vec3 result; XMStoreFloat3((XMFLOAT3 *)& result, a); return result; }
inline quat     from_fastq(const XMVECTOR &a) { quat result; XMStoreFloat4((XMFLOAT4 *)& result, a); return result; }

///////////////////////////////////////////

void transform_initialize(transform_t &transform) {
	transform._position = vec3_zero;
	transform._rotation = quat_identity;
	transform._scale    = vec3_one;
	transform._dirty    = true;
}

///////////////////////////////////////////

void transform_set(transform_t &transform, const vec3 &position, const vec3 &scale, const quat &rotation) {
	transform._position = position;
	transform._scale    = scale;
	transform._rotation = rotation;
	transform._dirty    = true;
}

///////////////////////////////////////////

void transform_set_position(transform_t &transform, const vec3 &position) {
	transform._position = position;
	transform._dirty    = true;
}

///////////////////////////////////////////

vec3 transform_get_position(transform_t &transform) {
	return transform._position;
}

///////////////////////////////////////////

void transform_set_scale(transform_t &transform, const vec3 &scale) {
	transform._scale = scale;
	transform._dirty = true;
}

///////////////////////////////////////////

vec3 transform_get_scale(transform_t &transform) {
	return transform._scale;
}

///////////////////////////////////////////

void transform_set_rotation  (transform_t &transform, const quat &rotation) {
	transform._rotation = rotation;
	transform._dirty    = true;
}

///////////////////////////////////////////

quat transform_get_rotation(transform_t &transform) {
	return transform._rotation;
}

///////////////////////////////////////////

vec3 transform_forward  (transform_t &transform) {
	vec3 forward = { 0,0,-1 };
	return transform._rotation * forward;
}

///////////////////////////////////////////

void transform_lookat  (transform_t &transform, const vec3 &at) {
	XMMATRIX mat = XMMatrixLookAtRH(to_fast3(transform._position), to_fast3(at), XMVectorSet(0, 1, 0, 0));
	transform._rotation = from_fastq(XMQuaternionRotationMatrix(XMMatrixTranspose(mat)));
	transform._dirty    = true;
}

///////////////////////////////////////////

void transform_update(transform_t &transform) {
	if (transform._dirty) {
		transform._dirty = false;
		transform._transform = matrix_trs(
			transform._position, 
			transform._rotation, 
			transform._scale);
	}
}

///////////////////////////////////////////

void transform_matrix_out(transform_t &transform, matrix &result) {
	if (transform._dirty) {
		transform._dirty = false;
		transform._transform = matrix_trs(
			transform._position, 
			transform._rotation, 
			transform._scale);
	}
	result = transform._transform;
}

///////////////////////////////////////////

matrix transform_matrix(transform_t &transform) {
	if (transform._dirty) {
		transform._dirty = false;
		transform._transform = matrix_trs(
			transform._position, 
			transform._rotation, 
			transform._scale);
	}
	return transform._transform;
}

///////////////////////////////////////////

vec3 transform_world_to_local(transform_t &transform, const vec3 &world_coordinate) {
	matrix   tr;
	XMMATRIX fast;

	transform_matrix_out(transform, tr);
	math_matrix_to_fast(tr, &fast);
	fast = XMMatrixInverse(nullptr, fast);

	XMVECTOR resultXM = XMVector3Transform( XMLoadFloat3((XMFLOAT3 *)& world_coordinate),  fast);
	vec3     result;
	XMStoreFloat3((XMFLOAT3 *)& result, resultXM);
	return result;
}

///////////////////////////////////////////

vec3 transform_local_to_world(transform_t &transform, const vec3 &local_coordinate) {
	matrix   tr;
	XMMATRIX fast;
	transform_matrix_out(transform, tr);
	math_matrix_to_fast(tr, &fast);

	XMVECTOR resultXM = XMVector3Transform( XMLoadFloat3((XMFLOAT3 *)& local_coordinate),  fast);
	vec3     result;
	XMStoreFloat3((XMFLOAT3 *)& result, resultXM);
	return result;
}

///////////////////////////////////////////

vec3 transform_world_to_local_dir(transform_t &transform, const vec3 &world_direction) {
	matrix   tr;
	XMMATRIX fast;
	transform_matrix_out(transform, tr);
	math_matrix_to_fast(tr, &fast);
	fast = XMMatrixInverse(nullptr, fast);

	XMVECTOR resultXM = XMVector3TransformNormal( XMLoadFloat3((XMFLOAT3 *)& world_direction),  fast);
	vec3     result;
	XMStoreFloat3((XMFLOAT3 *)& result, resultXM);
	return result;
}

///////////////////////////////////////////

vec3 transform_local_to_world_dir(transform_t &transform, const vec3 &local_direction) {
	matrix   tr;
	XMMATRIX fast;
	transform_matrix_out(transform, tr);
	math_matrix_to_fast(tr, &fast);
	XMVECTOR resultXM = XMVector3TransformNormal( XMLoadFloat3((XMFLOAT3 *)& local_direction), fast);
	vec3     result;
	XMStoreFloat3((XMFLOAT3 *)& result, resultXM);
	return result;
}