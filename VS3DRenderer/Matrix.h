#pragma once

#ifndef MATRIX_H
#define MATRIX_H

#include "Vector.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Declare a new type to hold 4x4 matrices
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct
{
	float matrix[4][4];
}mat4_t;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

mat4_t Mat4_Identity(void);

mat4_t Mat4_MakeRotationX(float Angle);
mat4_t Mat4_MakeRotationY(float Angle);
mat4_t Mat4_MakeRotationZ(float Angle);

mat4_t Mat4_MakeScale(float Sx, float Sy, float Sz);

mat4_t Mat4_MakeTranslation(float Tx, float Ty, float Tz);

mat4_t Mat4_Multiply_Mat4(mat4_t MatrixA, mat4_t MatrixB);

vec4_t Mat4_Multiply_Vec4(mat4_t Matrix, vec4_t Vector);

/*
	World Matrix = combination/multiplication of all the matrices that transform
	the original vectors and place the overall 3D mesh in its correct place in 
	the world/scene
*/
mat4_t Mat4_MakeWorldMatrix(mat4_t Scale, mat4_t RotX, mat4_t RotY, mat4_t RotZ, mat4_t Translation);

mat4_t Mat4_Make_PerspectiveProjection(float FOV, float AspectRatio, float ZNear, float ZFar);

vec4_t Mat4_Multiply_Vec4_Projection	(mat4_t ProjectionMatrix, vec4_t Vector);

// Compute the Look At function
mat4_t Mat4_Look_At(vec3_t CameraEye, vec3_t LookTarget, vec3_t UpVector);

#endif // !MATRIX_H
