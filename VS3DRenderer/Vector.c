#include <math.h>
#include "Vector.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation of Vector 2 Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

vec2_t Vec2_New(float x, float y)
{
	vec2_t NewVector = { x,y };
	return NewVector;
}

// Calculate the magnitude of a vector2
float Vec2_Length(vec2_t Vector2)
{
	float VecLength = sqrtf((Vector2.x * Vector2.x) + (Vector2.y * Vector2.y));
	return VecLength;
}

// Calculate the addition of 2 vector2
vec2_t Vec2_Add(vec2_t VectorA, vec2_t VectorB)
{
	vec2_t Result = 
	{
		.x = VectorA.x + VectorB.x,
		.y = VectorA.y + VectorB.y
	};
	return Result;
}

// Calculate the subtraction of 2 vector2
vec2_t Vec2_Subtract(vec2_t VectorA, vec2_t VectorB)
{
	vec2_t Result =
	{
		.x = VectorA.x - VectorB.x,
		.y = VectorA.y - VectorB.y
	};
	return Result;
}

// Calculate the multiplication of a vector2 and a scalar number
vec2_t Vec2_ScalarMultiply(vec2_t Vector, float Factor)
{
	vec2_t Result =
	{
		.x = Vector.x * Factor,
		.y = Vector.y * Factor
	};
	return Result;
}

// Calculate the division of a vector2 and a scalar number
vec2_t Vec2_ScalarDivide(vec2_t Vector, float Factor)
{
	vec2_t Result =
	{
		.x = Vector.x / Factor,
		.y = Vector.y / Factor
	};
	return Result;
}

// Calculate the dot product of 2 vector2 (tell how aligned are 2 vectors)
float Vec2_Dot(vec2_t VectorA, vec2_t VectorB)
{
	// Dot product between vectorA and vetorB, is the projection of B onto A
	// If both vectors are pointing in the same direction, the dot product will be exactly 1
	// inversely, the less aligned they're, the dot product will be less
	// If both vectors are perpendicular (forming a 90° between them), the dot product will be exactly 0
	// If both are completely opposite to each other, the dot product will be exactly -1

	// So we can say the dot product between 2 vectors tell us how aligned they're

	float Result = (VectorA.x * VectorB.x) + (VectorA.y * VectorB.y);
	return Result;
}

// Normalize (transform a vector of any length into an unit vector, length = 1) a vector2
void Vec2_Normalize(vec2_t *Vector)
{
	float VectorLength = Vec2_Length(*Vector);
	Vector->x /= VectorLength;
	Vector->y /= VectorLength;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation of Vector 3 Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Make new vector3 with the especified values
vec3_t Vec3_New(float x, float y, float z)
{
	vec3_t NewVector = { x,y,z };
	return NewVector;
}

// Roll
vec3_t Vec3_Rotate_X(vec3_t Vector, float angle)
{
	vec3_t RotatedVector =
	{
		.x = Vector.x,
		.y = Vector.y * cos(angle) - Vector.z * sin(angle),
		.z = Vector.y * sin(angle) + Vector.z * cos(angle)
	};

	return RotatedVector;
}

// Yaw
vec3_t Vec3_Rotate_Y(vec3_t Vector, float angle)
{
	vec3_t RotatedVector =
	{
		.x = Vector.x * cos(angle) - Vector.z * sin(angle),
		.y = Vector.y,
		.z = Vector.x * sin(angle) + Vector.z * cos(angle)
	};

	return RotatedVector;
}

// Pitch
vec3_t Vec3_Rotate_Z(vec3_t Vector, float angle)
{
	vec3_t RotatedVector =
	{
		.x = Vector.x * cos(angle) - Vector.y * sin(angle),
		.y = Vector.x * sin(angle) + Vector.y * cos(angle),
		.z = Vector.z
	};

	return RotatedVector;
}

// Calculate the magnitude of a vector3
float Vec3_Length(vec3_t Vector3)
{
	float VecLength = sqrtf( (Vector3.x * Vector3.x) + (Vector3.y * Vector3.y) + (Vector3.z * Vector3.z) );
	return VecLength;
}

// Calculate the addition of 2 vector3
vec3_t Vec3_Add(vec3_t VectorA, vec3_t VectorB)
{
	vec3_t Result =
	{
		.x = VectorA.x + VectorB.x,
		.y = VectorA.y + VectorB.y,
		.z = VectorA.z + VectorB.z
	};
	return Result;
}

// Calculate the subtraction of 2 vector3
vec3_t Vec3_Subtract(vec3_t VectorA, vec3_t VectorB)
{
	// First invert the second vector, then is the same as an addition [ basically: a-b = a + (-b) ]

	// If you subtract the position of a vertex of a mesh's face minus the camera position vector, that subtraction
	// will give us the camera ray!

	vec3_t Result =
	{
		.x = VectorA.x - VectorB.x,
		.y = VectorA.y - VectorB.y,
		.z = VectorA.z - VectorB.z
	};
	return Result;
}

// Calculate the multiplication of a vector3 and a scalar number
vec3_t Vec3_ScalarMultiply(vec3_t Vector, float Factor)
{
	vec3_t Result =
	{
		.x = Vector.x * Factor,
		.y = Vector.y * Factor,
		.z = Vector.z * Factor
	};
	return Result;
}

// Calculate the division of a vector3 and a scalar number
vec3_t Vec3_ScalarDivide(vec3_t Vector, float Factor)
{
	vec3_t Result =
	{
		.x = Vector.x / Factor,
		.y = Vector.y / Factor,
		.z = Vector.z / Factor
	};
	return Result;
}

// Calculate the cross product of 2 vector3 (find the perpendicular vector between 2 vectors)
vec3_t Vec3_Cross(vec3_t VectorA, vec3_t VectorB)
{
	// PD: cross product is only define for 3D vectors, so, not 2D this time

	// The resulting vector is always perpendicular (forms a 90° angle) to both VectorA and VectorB
	
	// The direction of the resulting vector depends on the coordinate system (right-hand, left-hand)
	// and specially the order of the vectors (clockwise or counter)
	
	// The magnitude of the resulting vector is equal to the area of the parallelogram between both vectors
	// so, the more aligned both vectors are, the shorter is the cross product. If they point to the same direction,
	// the cross product is 0
	// Formally, its magnitude is: ||A x B|| = ||A|| * ||B|| * Sin(angle between both vectors)

	vec3_t Result =
	{
		.x = (VectorA.y*VectorB.z) - (VectorA.z*VectorB.y),
		.y = (VectorA.z*VectorB.x) - (VectorA.x*VectorB.z),
		.z = (VectorA.x*VectorB.y) - (VectorA.y*VectorB.x)
	};
	return Result;
}

// Calculate the dot product of 2 vector3 (tell how aligned are 2 vectors)
float Vec3_Dot(vec3_t VectorA, vec3_t VectorB)
{
	// Dot product between vectorA and vetorB, is the projection of B onto A
	// If both vectors are perfectly parallel to each and point at the same direction, the dot product'll be exactly 1
	// inversely, the less aligned they're, the dot product will be less
	// If both vectors are perpendicular (forming a 90° between them), the dot product will be exactly 0
	// The more opposite both vectors are, the dot product will be negative

	// So we can say the dot product between 2 vectors tell us how aligned they're

	float Result = (VectorA.x * VectorB.x) + (VectorA.y * VectorB.y) + (VectorA.z * VectorB.z);
	return Result;
}

// Normalize (transform a vector of any length into an unit vector, length = 1) a vector3
void Vec3_Normalize(vec3_t* Vector)
{
	float VectorLength = Vec3_Length(*Vector);
	Vector->x /= VectorLength;
	Vector->y /= VectorLength;
	Vector->z /= VectorLength;
}

// Create a Vector3 from a Vector3 pointer
vec3_t Vec3_From_Pointer(vec3_t * vp)
{
	vec3_t NewVec = { vp->x, vp->y, vp->z };
	return NewVec;
}

/*
	For back face culling (how aligned is a face to the camera ray):
	1- Find vectors B-A and C-A
	2- Take the cross product of the subtraction to find the perpendicular normal vector N
	3- Find the camera ray vector by subtracting the camera position from point A
	4- Take the dot product between the normal vector and the camera ray vector
	5- If dot product is positive, draw that face
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation of Vector Convertion Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

vec2_t Vec4_To_Vec2(vec4_t Vector4)
{
	vec2_t Vector2 =
	{
		Vector4.x,
		Vector4.y,
	};
	return Vector2;
}

vec3_t Vec4_To_Vec3(vec4_t Vector4)
{
	vec3_t Vector3 =
	{
		Vector4.x,
		Vector4.y,
		Vector4.z,
	};

	return Vector3;
}

vec4_t Vec3_To_Vec4(vec3_t Vector3)
{
	vec4_t Vector4 = 
	{
		Vector3.x,
		Vector3.y,
		Vector3.z,
		.w = 1
	};

	return Vector4;
}