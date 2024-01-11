#pragma once

#ifndef VECTOR_H
#define VECTOR_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Declare a new type to hold vectors 2
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// The memory allocated for a struct depends on the size of its components
// e.g. A Vec2_t will allocate 2 floats, so depending on the machine's arch. it would allocate 8bytes (4bytes each)

typedef struct
{
	float x;
	float y;
} vec2_t;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Declare a new type to hold vectors 3
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct
{
	float x;
	float y;
	float z;
} vec3_t;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Declare a new type to hold vectors 4
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct
{
	float x;
	float y;
	float z;
	float w;
} vec4_t;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Vector 2 Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Make new vector2 with the especified values
vec2_t Vec2_New(float x, float y);

// Calculate the magnitude of a vector2
float Vec2_Length(vec2_t Vector2);

// Calculate the addition of 2 vector2
vec2_t Vec2_Add(vec2_t VectorA, vec2_t VectorB);

// Calculate the subtraction of 2 vector2
vec2_t Vec2_Subtract(vec2_t VectorA, vec2_t VectorB);

// Calculate the multiplication of a vector2 and a scalar number
vec2_t Vec2_ScalarMultiply(vec2_t Vector, float Factor);

// Calculate the division of a vector2 and a scalar number
vec2_t Vec2_ScalarDivide(vec2_t Vector, float Factor);

// Calculate the dot product of 2 vector2 (tell how aligned are 2 vectors)
float Vec2_Dot(vec2_t VectorA, vec2_t VectorB);

// Normalize (transform a vector of any length into an unit vector, length = 1) a vector2
void Vec2_Normalize(vec2_t* Vector);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Vector 3 Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Make new vector3 with the especified values
vec3_t Vec3_New(float x, float y, float z);

// Roll
vec3_t Vec3_Rotate_X( vec3_t Vector, float angle);
// Yaw
vec3_t Vec3_Rotate_Y(vec3_t Vector, float angle);
//Pitch
vec3_t Vec3_Rotate_Z(vec3_t Vector, float angle);

// Calculate the magnitude of a vector3
float Vec3_Length(vec3_t Vector3);

// Calculate the addition of 2 vector3
vec3_t Vec3_Add(vec3_t VectorA, vec3_t VectorB);

// Calculate the subtraction of 2 vector3
vec3_t Vec3_Subtract(vec3_t VectorA, vec3_t VectorB);

// Calculate the multiplication of a vector3 and a scalar number
vec3_t Vec3_ScalarMultiply(vec3_t Vector, float Factor);

// Calculate the division of a vector3 and a scalar number
vec3_t Vec3_ScalarDivide(vec3_t Vector, float Factor);

// Calculate the cross product of 2 vector3 (find the perpendicular vector between 2 vectors)
vec3_t Vec3_Cross(vec3_t VectorA, vec3_t VectorB);

// Calculate the dot product of 2 vector3 (tell how aligned are 2 vectors)
float Vec3_Dot(vec3_t VectorA, vec3_t VectorB);

// Normalize (transform a vector of any length into an unit vector, length = 1) a vector3
void Vec3_Normalize(vec3_t* Vector);

// Create a Vector3 from a Vector3 pointer
vec3_t Vec3_From_Pointer(vec3_t* vp);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Vector Convertion Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

vec2_t Vec4_To_Vec2(vec4_t Vector4);

vec3_t Vec4_To_Vec3(vec4_t Vector4);

vec4_t Vec3_To_Vec4(vec3_t Vector3);

#endif // !VECTOR_H
