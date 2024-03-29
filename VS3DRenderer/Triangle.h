#pragma once

#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <stdint.h>
#include "Vector.h"
#include "Display.h"
#include "Texture.h"
#include "upng.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Declare a new type to hold 3D face vertex index
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct
{
	int a;
	int b;
	int	c;
	tex2_t aUV; //UV texture coordinates for vertex a
	tex2_t bUV; //UV texture coordinates for vertex b
	tex2_t cUV; //UV texture coordinates for vertex c
	color_t color;
} TriangleFace_t;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Declare a new type to hold 2D projected face vertex coordinates
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct
{
	vec4_t vertex[3]; // The 3 vertices of each triangle. If you don't want perspective correct interpolation, use a vec2 (just x and y)
	tex2_t uvCoordinates[3]; // The UV texture coordinates of each triangle's vertices
	color_t color;
	upng_t* texture;
} Triangle_t;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Return the barycentric weights (alpha, beta, gamma) for point P inside a triangle ABC
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
vec3_t Barycentric_Weights(vec2_t A, vec2_t B, vec2_t C, vec2_t P);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Draw a pixel with solid color at position (X,Y) of a triangle using interpolation
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Draw_Triangle_Pixel(
	int x, int y,
	vec4_t VertexA, vec4_t VertexB, vec4_t VertexC, // If you don't want perspective correct interpolation, use a vec2 (just x and y)
	color_t Color
);

///////////////////////////////////////////////////////////////////////////////
// Draw a filled triangle with the flat-top/flat-bottom method
///////////////////////////////////////////////////////////////////////////////
void Draw_Filled_Triangle(
	int x0, int y0, float z0, float w0,
	int x1, int y1, float z1, float w1,
	int x2, int y2, float z2, float w2,
	color_t Color
);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Draw texture pixel (texel) at position (X,Y) using interpolation
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Draw_Texel(
	int x, int y,
	vec4_t VertexA, vec4_t VertexB, vec4_t VertexC, // If you don't want perspective correct interpolation, use a vec2 (just x and y)
	tex2_t AUV, tex2_t BUV, tex2_t CUV,
	upng_t* Texture
);

///////////////////////////////////////////////////////////////////////////////
// Draw a textured triangle
///////////////////////////////////////////////////////////////////////////////

// If you don't want perspective correct interpolation, you don't need to pass the Z and W components
void Draw_Textured_Triangle(
	int x0, int y0, float z0, float w0,
	int x1, int y1, float z1, float w1,
	int x2, int y2, float z2, float w2,
	float u0, float v0, float u1, float v1, float u2, float v2,
	upng_t* texture
);

vec3_t Get_Triangle_Normal(vec4_t* TriangleVertices);

#endif
