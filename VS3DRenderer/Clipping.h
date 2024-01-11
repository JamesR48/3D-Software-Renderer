#pragma once

#ifndef CLIPPING_H
#define CLIPPING_H

#include "Vector.h"
#include "Triangle.h"

#define MAX_NUM_POLYGON_VERTICES 10
#define MAX_NUM_POLYGON_TRIANGLES 10

// View frustrum planes
enum
{
	LEFT_PLANE,
	RIGHT_PLANE,
	TOP_PLANE,
	BOTTOM_PLANE,
	NEAR_PLANE,
	FAR_PLANE
};

typedef struct
{
	vec3_t Point; // Plane's position/origin
	vec3_t Normal; // Plane's normal vector
}plane_t;

typedef struct
{
	vec3_t Vertices[MAX_NUM_POLYGON_VERTICES];
	tex2_t TextureCoords[MAX_NUM_POLYGON_VERTICES];
	int NumVertices;
}polygon_t;

///////////////////////////////////////////////////////////////////////////////
// Initialize the Frustum planes (defined by a point and a normal vector)
///////////////////////////////////////////////////////////////////////////////
// Near plane   :  P=(0, 0, znear), N=(0, 0,  1)
// Far plane    :  P=(0, 0, zfar),  N=(0, 0, -1)
// Top plane    :  P=(0, 0, 0),     N=(0, -cos(fovy/2), sin(fovy/2))
// Bottom plane :  P=(0, 0, 0),     N=(0, cos(fovy/2), sin(fovy/2))
// Left plane   :  P=(0, 0, 0),     N=(cos(fovx/2), 0, sin(fovx/2))
// Right plane  :  P=(0, 0, 0),     N=(-cos(fovx/2), 0, sin(fovx/2))
///////////////////////////////////////////////////////////////////////////////
void Initialize_Frustrum_Planes(float FOVx, float FOVy, float ZNear, float ZFar);

// A point Q will be "ON" the plane if Dot( Vector(Q-P), PlaneNormal ) = 0 (the subtracted vector is perpendicular to the normal)
// A point Q will be in the "OUTSIDE" space made by the plane if Dot( Vector(Q-P), PlaneNormal ) < 0
// A point Q will be in the "INSIDE" space made by the plane if Dot( Vector(Q-P), PlaneNormal ) > 0

// Linear Interpolation Equation ---> I = Q1 + t(Q2-Q1)
// With this formula we can define any point between Q1 and Q2 by choosing a value for "t" between 0 and 1 (like a percentage)
// t = 0 (0%) would be exactly at point Q1, t = 1 (100%) would be exactly at point Q2. Other percentages are values in between
// This could be used to find the point of intersection between a plane and a line segment defined by the points Q1,Q2

float Float_Lerp(float a, float b, float factor);

///////////////////////////////////////////////////////////////////////////////
// Create a polygon from the original triangle
///////////////////////////////////////////////////////////////////////////////
polygon_t Polygon_From_Triangle(
	vec4_t* TriangleVert0, vec4_t* TriangleVert1, vec4_t* TriangleVert2,
	tex2_t* TriangleUV0, tex2_t* TriangleUV1, tex2_t* TriangleUV2
);

///////////////////////////////////////////////////////////////////////////////
// Clip the polygon, returning a clipped polygon back
///////////////////////////////////////////////////////////////////////////////
void Clip_Polygon(polygon_t* Polygon );

void Clip_Polygon_Against_Plane(polygon_t* Polygon, int PlaneIndex);

void Triangles_From_Polygon(polygon_t* Polygon, Triangle_t Triangles[], int* NumTriangles);

#endif // !CLIPPING_H
