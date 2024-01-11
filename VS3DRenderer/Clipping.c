#include <math.h>
#include "Clipping.h"

#define NUM_FRUSTRUM_PLANES 6
plane_t FrustrumPlanes[NUM_FRUSTRUM_PLANES];

///////////////////////////////////////////////////////////////////////////////
// Frustum planes are defined by a point and a normal vector
///////////////////////////////////////////////////////////////////////////////
// Near plane   :  P=(0, 0, znear), N=(0, 0,  1)
// Far plane    :  P=(0, 0, zfar),  N=(0, 0, -1)
// Top plane    :  P=(0, 0, 0),     N=(0, -cos(fovy/2), sin(fovy/2))
// Bottom plane :  P=(0, 0, 0),     N=(0, cos(fovy/2), sin(fovy/2))
// Left plane   :  P=(0, 0, 0),     N=(cos(fovx/2), 0, sin(fovx/2))
// Right plane  :  P=(0, 0, 0),     N=(-cos(fovx/2), 0, sin(fovx/2))
///////////////////////////////////////////////////////////////////////////////
//
//           /|\
//         /  | | 
//       /\   | |
//     /      | |
//  P*|-->  <-|*|   ----> +z-axis
//     \      | |
//       \/   | |
//         \  | | 
//           \|/
//
///////////////////////////////////////////////////////////////////////////////
void Initialize_Frustrum_Planes(float FOVx, float FOVy, float ZNear, float ZFar)
{
	float Cos_Half_FOVx = cos(FOVx/2);
	float Sin_Half_FOVx = sin(FOVx/2);

	float Cos_Half_FOVy = cos(FOVy / 2);
	float Sin_Half_FOVy = sin(FOVy / 2);

	FrustrumPlanes[LEFT_PLANE].Point = Vec3_New(0,0,0);
	FrustrumPlanes[LEFT_PLANE].Normal = Vec3_New(Cos_Half_FOVx, 0, Sin_Half_FOVx );

	FrustrumPlanes[RIGHT_PLANE].Point = Vec3_New(0, 0, 0);
	FrustrumPlanes[RIGHT_PLANE].Normal = Vec3_New( -Cos_Half_FOVx, 0, Sin_Half_FOVx);

	FrustrumPlanes[TOP_PLANE].Point = Vec3_New(0, 0, 0);
	FrustrumPlanes[TOP_PLANE].Normal = Vec3_New(0, -Cos_Half_FOVy, Sin_Half_FOVy);

	FrustrumPlanes[BOTTOM_PLANE].Point = Vec3_New(0, 0, 0);
	FrustrumPlanes[BOTTOM_PLANE].Normal = Vec3_New(0, Cos_Half_FOVy, Sin_Half_FOVy);

	FrustrumPlanes[NEAR_PLANE].Point = Vec3_New(0, 0, ZNear);
	FrustrumPlanes[NEAR_PLANE].Normal = Vec3_New(0, 0, 1);

	FrustrumPlanes[FAR_PLANE].Point = Vec3_New(0, 0, ZFar);
	FrustrumPlanes[FAR_PLANE].Normal = Vec3_New(0, 0, -1);
}

float Float_Lerp(float a, float b, float factor)
{
	return (a + (factor * (b-a) ) );
}

///////////////////////////////////////////////////////////////////////////////
// Create a polygon from the original triangle
///////////////////////////////////////////////////////////////////////////////
polygon_t Polygon_From_Triangle(
	vec4_t* TriangleVert0, vec4_t* TriangleVert1, vec4_t* TriangleVert2,
	tex2_t* TriangleUV0, tex2_t* TriangleUV1, tex2_t* TriangleUV2
)
{
	vec3_t Vert0 = Vec4_To_Vec3(*TriangleVert0);
	vec3_t Vert1 = Vec4_To_Vec3(*TriangleVert1);
	vec3_t Vert2 = Vec4_To_Vec3(*TriangleVert2);

	tex2_t TexCoord0 = *TriangleUV0;
	tex2_t TexCoord1 = *TriangleUV1;
	tex2_t TexCoord2 = *TriangleUV2;

	polygon_t NewPolygon = 
	{
		.Vertices = {Vert0, Vert1, Vert2},
		.TextureCoords = {TexCoord0, TexCoord1, TexCoord2},
		.NumVertices = 3
	};
	
	return NewPolygon;
}

///////////////////////////////////////////////////////////////////////////////
// Clip the polygon, returning a clipped polygon back
///////////////////////////////////////////////////////////////////////////////
void Clip_Polygon(polygon_t * Polygon)
{
	// The resulting output polygon of one clipping stage is used as the input for the next one
	Clip_Polygon_Against_Plane(Polygon, LEFT_PLANE);
	Clip_Polygon_Against_Plane(Polygon, RIGHT_PLANE);
	Clip_Polygon_Against_Plane(Polygon, TOP_PLANE);
	Clip_Polygon_Against_Plane(Polygon, BOTTOM_PLANE);
	Clip_Polygon_Against_Plane(Polygon, NEAR_PLANE);
	Clip_Polygon_Against_Plane(Polygon, FAR_PLANE);
}

void Clip_Polygon_Against_Plane(polygon_t * Polygon, int PlaneIndex)
{
	vec3_t PlanePoint = FrustrumPlanes[PlaneIndex].Point;
	vec3_t PlaneNormal = FrustrumPlanes[PlaneIndex].Normal;
	
	// Declare the static array of Inside vertices and texture coords that will be part of the final clipped polygon returned
	vec3_t InsideVerts[MAX_NUM_POLYGON_VERTICES];
	tex2_t InsideTextCoords[MAX_NUM_POLYGON_VERTICES];
	int NumInsideVerts = 0; // Counter of Inside vertices added to the array

	// Initialize the current vertex and texture coordinte with the first polygon vertex and texture coordinate
	vec3_t* CurrentVert = &Polygon->Vertices[0];
	tex2_t* CurrentTextCoord = &Polygon->TextureCoords[0];

	// Calculate the previous vertex and texture coordunate with the last polygon vertex and texture coords
	vec3_t* PreviousVert = &Polygon->Vertices[Polygon->NumVertices - 1];
	tex2_t* PreviousTextCoord = &Polygon->TextureCoords[Polygon->NumVertices - 1];

	// Initialize the current polygon vertex dot product
	float CurrentDot = 0.0;
	// Calculate the previous vertex dot product with the plane normal
	// DotVert2 = Dot( Vector(Q2-PlanePoint), PlaneNormal )
	float PreviousDot = Vec3_Dot( Vec3_Subtract(*PreviousVert, PlanePoint), PlaneNormal);

	// Loop while the current vertex is different than the last vertex
	while ( CurrentVert != &Polygon->Vertices[Polygon->NumVertices] )
	{
		// Calculate the current vertex dot product with the plane normal
		// DotVert1 = Dot( Vector(Q1-PlanePoint), PlaneNormal )
		CurrentDot = Vec3_Dot( Vec3_Subtract(*CurrentVert, PlanePoint), PlaneNormal );

		// If the current vertex is Outside and the previous was Inside or vice-versa...
		// ...(the dot products changed from negative to positive or vice-versa basically)
		if ( (CurrentDot * PreviousDot) < 0.0)
		{
			// Calculate the interpolation factor, t = DotVert1 / (DotVert1 - DotVert2)
			float t = PreviousDot / (PreviousDot - CurrentDot);
			
			// Calculate the intersection point, I = Vert1 + t(Vert2 - Vert1)
			vec3_t IntersectionPoint = 
			{
				.x = Float_Lerp(PreviousVert->x, CurrentVert->x, t),
				.y = Float_Lerp(PreviousVert->y, CurrentVert->y, t),
				.z = Float_Lerp(PreviousVert->z, CurrentVert->z, t)
			};

			// Use the lerp (linear interpolation) formula to get the interpolated U and V texture coordinate
			tex2_t InterpolatedTextCoord = 
			{
				.u = Float_Lerp(PreviousTextCoord->u, CurrentTextCoord->u, t),
				.v = Float_Lerp(PreviousTextCoord->v, CurrentTextCoord->v, t)
			};

			// Add the new intersection point in the list of Inside vertices
			InsideVerts[NumInsideVerts] = Vec3_From_Pointer(&IntersectionPoint);

			// Add the current texture coordiante to the list of Inside textcoords
			InsideTextCoords[NumInsideVerts] = Tex2_From_Pointer(&InterpolatedTextCoord);

			NumInsideVerts++; // Add one to the counter of Inside vertices added
		}

		// If the current vertex is inside the plane (its dot product is positive)
		if (CurrentDot > 0.0)
		{
			// Add the current vertex to the list of Inside vertices
			InsideVerts[NumInsideVerts] = Vec3_From_Pointer(CurrentVert);

			// Add the current texture coordiante to the list of Inside textcoords
			InsideTextCoords[NumInsideVerts] = Tex2_From_Pointer(CurrentTextCoord);

			NumInsideVerts++; // Add one to the counter of Inside vertices added
		}

		// Move to the next vertex and texture coordinate
		PreviousDot = CurrentDot;
		PreviousVert = CurrentVert;
		PreviousTextCoord = CurrentTextCoord;
		// Pointer arithmetic. Since CurrentVert and CurrentTextCoord are pointers to an array's 1st element...
		// ...the ++ means that we move to the next array element and so on
		CurrentVert++; 
		CurrentTextCoord++;
	}

	// Copy all the vertices and texture coordinates from the Inside vertices/textcoords array into the destination polygon
	for (int idx = 0; idx < NumInsideVerts; idx++)
	{
		Polygon->Vertices[idx] = Vec3_From_Pointer(&InsideVerts[idx]);
		Polygon->TextureCoords[idx] = Tex2_From_Pointer(&InsideTextCoords[idx]);
	}
	Polygon->NumVertices = NumInsideVerts;
}

void Triangles_From_Polygon(polygon_t * Polygon, Triangle_t Triangles[], int * NumTriangles)
{
	for(int idx = 0; idx < Polygon->NumVertices - 2; idx++)
	{
		int IndexVert0 = 0;
		int IndexVert1 = idx + 1;
		int IndexVert2 = idx + 2;

		Triangles[idx].vertex[0] = Vec3_To_Vec4( Polygon->Vertices[IndexVert0] );
		Triangles[idx].vertex[1] = Vec3_To_Vec4( Polygon->Vertices[IndexVert1] );
		Triangles[idx].vertex[2] = Vec3_To_Vec4( Polygon->Vertices[IndexVert2] );

		Triangles[idx].uvCoordinates[0] = Polygon->TextureCoords[IndexVert0];
		Triangles[idx].uvCoordinates[1] = Polygon->TextureCoords[IndexVert1];
		Triangles[idx].uvCoordinates[2] = Polygon->TextureCoords[IndexVert2];
	}

	*NumTriangles = Polygon->NumVertices - 2;
}
