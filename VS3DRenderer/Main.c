#include <stdio.h>
#include <stdint.h> // For the new fixed types
#include <stdbool.h>
#include <math.h>
#include <SDL.h>
#include "upng.h"
#include "Array.h"
#include "Display.h"
#include "Vector.h"
#include "Matrix.h"
#include "Mesh.h"
#include "Light.h"
#include "Texture.h"
#include "Camera.h"
#include "Clipping.h"

// Left-handed coordinate system here (inside the monitor +Z outside -Z, o the right +X left -X, up +Y down -Y )

///////////////////////////////////////////////////////////////////////////////
// Array to store triangles that should be rendered each frame
///////////////////////////////////////////////////////////////////////////////

#define MAX_TRIANGLES_PER_MESH 50000
Triangle_t TrianglesToRender[MAX_TRIANGLES_PER_MESH];
int NumTrianglesToRender = 0;

Triangle_t ProjectedTriangles[N_CUBE_FACES];

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global variables for execution status and game loop
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool bIsRunning = false;
float DeltaTime = 0.0;
int PreviousFrameTime = 0; 	// How many miliseconds have passed since  last frame

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global variables for global transformations matrices
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

mat4_t WorldMatrix;
mat4_t ViewMatrix;
mat4_t PerspectiveProjectionMat;

int WindowWidth;
int WindowHeight;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Setup function to initialize variables and game objects
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Setup(void)
{
	// Initialize the render mode and the triangle culling method
	Set_Render_Mode(RENDER_TEXTURED);
	Set_Cull_Mode(CULL_BACKFACE);

	WindowWidth = Get_Window_Width();
	WindowHeight = Get_Window_Height();

	// Initialize the default Sun light
	Set_SunLight(Vec3_New(0, 0, 1));

	// Initialize the default camera
	Set_Camera_Position(Vec3_New(0, 0, 0));
	Set_Camera_LookDirection(Vec3_New(0, 0, 1));

	// Initialize the Perspective-Projection Matrix
	float AspectRatioX = (float)WindowWidth / (float)WindowHeight; // Horizontal aspect ratio
	float AspectRatioY = (float)WindowHeight / (float)WindowWidth; // Vertical aspect ratio
	// Field of view, vertical and horizontal
	float FOVy = M_PI / 2.0; // FOV angle in Rad (in Deg this is 90). [ Rad = (Deg*PI)/180 = (90*PI)/180 = PI/2 ]
	/*
	  The tan(Fovy/2.0) takes the FOVy angle to length space, so we multiply it by the horizontal aspect ratio
	  to calculate the horizontal FOV length. Then, the arctangent of the previous operation takes the resulting length
	  back into angle space, and finally, because initially the calculated tangent was the FOVy/2, we multiply the result
	  by 2 to get the full horizontal FOV angle
	*/
	float FOVx = atan(tan(FOVy / 2.0) * AspectRatioX) * 2.0;
	float ZNear = 0.1;
	float ZFar = 100.0;
	PerspectiveProjectionMat = Mat4_Make_PerspectiveProjection(FOVy, AspectRatioY, ZNear, ZFar);

	// Initialize the View Frustrum planes  with a point and normal vector
	Initialize_Frustrum_Planes(FOVx, FOVy, ZNear, ZFar);


	/*
		int PointCount = 0; // To ge the index of the new point that'll be added to the Cube Points array

		// Start loading the array of vectors
		// From -1 to 1, so each side of the cube will be of length 2
		for (float x = -1; x <= 1; x += 0.25)
		{
			for (float y = -1; y <= 1; y += 0.25)
			{
				for (float z = -1; z <= 1; z += 0.25)
				{
					vec3_t NewPoint = { x, y, z };
					CubePoints[PointCount] = NewPoint;
					PointCount++;
				}
			}
		}
	*/

	// Manually code the hard-coded texture data (for the cube) from the static array
	// The texture is 64x64 (so, 4096)
	//MeshTexture = (color_t*)REDBRICK_TEXTURE;

	// Loads the cube hard-coded values in the mesh data structure
	//Load_Cube_Mesh_Data();	

	// Loads an OBJ model file (mesh) and a PNG file (texture) to save their data in the mesh data structure
	// [ Rad = (Deg*PI)/180 = (90*PI)/180 = PI/2 ]
	Load_Mesh("Assets/galaxy.obj", "Assets/galaxy4.png", Vec3_New(2, 2, 2), Vec3_New(0, 0, 8), Vec3_New(0, 0, 0));
	//Load_Mesh("Assets/monkey.obj", "Assets/Monkey.png", Vec3_New(1, 1, 1), Vec3_New(-3, 0, 8), Vec3_New(0, 0, 0));
	Load_Mesh("Assets/astronaut.obj", "Assets/astronaut2.png", Vec3_New(1, 1, 1), Vec3_New(0, 0, 8), Vec3_New((45 * M_PI) / 180, 0, 0));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Poll system events and handle keyboard input
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Process_Input(void)
{
	SDL_Event Event;

	while (SDL_PollEvent(&Event))
	{
		switch (Event.type)
		{
			//When user clicks the "red X" in the application window's top right corner
		case SDL_QUIT:
			bIsRunning = false;
			break;

		case SDL_KEYDOWN:
			//If a keyboard key was pressed, and it was the ESC key
			if (Event.key.keysym.sym == SDLK_ESCAPE)
			{
				bIsRunning = false;
				break;
			}
			//If a keyboard key was pressed, and it was the 1 Key
			// RenderMode displays the wireframe and a small blue dot for each triangle vertex
			if (Event.key.keysym.sym == SDLK_1)
			{
				Set_Render_Mode(RENDER_WIRE_VERTEX);
				break;
			}
			//If a keyboard key was pressed, and it was the 2 Key
			// RenderMode displays only the wireframe lines
			if (Event.key.keysym.sym == SDLK_2)
			{
				Set_Render_Mode(RENDER_WIRE);
				break;
			}
			//If a keyboard key was pressed, and it was the 3 key
			// RenderMode displays filled triangles with a solid color
			if (Event.key.keysym.sym == SDLK_3)
			{
				Set_Render_Mode(RENDER_FILL_TRIANGLE);
				break;
			}
			//If a keyboard key was pressed, and it was the 4 key
			// RenderMode displays both filled triangles and wireframe lines
			if (Event.key.keysym.sym == SDLK_4)
			{
				Set_Render_Mode(RENDER_FILL_TRIANGLE_WIRE);
				break;
			}
			//If a keyboard key was pressed, and it was the 5 key
			// RenderMode displays textured triangles
			if (Event.key.keysym.sym == SDLK_5)
			{
				Set_Render_Mode(RENDER_TEXTURED);
				break;
			}
			//If a keyboard key was pressed, and it was the 6 key
			// RenderMode displays both textured triangles and wireframe lines
			if (Event.key.keysym.sym == SDLK_6)
			{
				Set_Render_Mode(RENDER_TEXTURED_WIRE);
				break;
			}
			//If a keyboard key was pressed, and it was the O key
			if (Event.key.keysym.sym == SDLK_o)
			{
				Set_Cull_Mode(CULL_BACKFACE);
				break;
			}
			//If a keyboard key was pressed, and it was the I key
			if (Event.key.keysym.sym == SDLK_i)
			{
				Set_Cull_Mode(CULL_NONE);
				break;
			}
			// If a keyboard key was pressed, and it was the W key
			// Move forward (where the camera is currently looking)
			if (Event.key.keysym.sym == SDLK_w)
			{
				Set_Camera_LookDirection(Vec3_New(0, 0, 1));
				Set_Camera_Velocity(5.0*DeltaTime);
				Set_Camera_Position(Vec3_Add(Get_Camera_Position(), Get_Camera_Velocity())); // Move forward
				break;
			}
			// If a keyboard key was pressed, and it was the S key
			// Move backward (opposite of where the camera is currently looking)
			if (Event.key.keysym.sym == SDLK_s)
			{
				Set_Camera_LookDirection(Vec3_New(0, 0, 1));
				Set_Camera_Velocity(-5.0*DeltaTime);
				Set_Camera_Position(Vec3_Add(Get_Camera_Position(), Get_Camera_Velocity())); // Move backward
				break;
			}
			// If a keyboard key was pressed, and it was the D key
			// Move right or rotate right (tank controls)
			if (Event.key.keysym.sym == SDLK_d)
			{
				Set_Camera_LookDirection(Vec3_New(1,0,0));
				Set_Camera_Velocity(5.0*DeltaTime);
				Set_Camera_Position(Vec3_Add(Get_Camera_Position(), Get_Camera_Velocity())); // Move Right
				break;
			}
			// If a keyboard key was pressed, and it was the A key
			// Move left or rotate left (tank controls)
			if (Event.key.keysym.sym == SDLK_a)
			{
				Set_Camera_LookDirection(Vec3_New(1, 0, 0));
				Set_Camera_Velocity(-5.0*DeltaTime);
				Set_Camera_Position(Vec3_Add(Get_Camera_Position(), Get_Camera_Velocity())); // Move Left
				break;
			}
			// If a keyboard key was pressed, and it was the Right Arrow key
			// Move right 
			if (Event.key.keysym.sym == SDLK_RIGHT)
			{
				Rotate_Camera_Euler(Vec3_New(0, 1.0*DeltaTime, 0)); // rotate right (around Y axis, yaw)
				break;
			}
			// If a keyboard key was pressed, and it was the Left Arrow key
			// Move left
			if (Event.key.keysym.sym == SDLK_LEFT)
			{
				Rotate_Camera_Euler(Vec3_New(0, -1.0*DeltaTime, 0)); // rotate left (around Y axis, yaw)
				break;
			}
			// If a keyboard key was pressed, and it was the Up Arrow key
			// Rotate up (tank controls)
			if (Event.key.keysym.sym == SDLK_UP)
			{
				Rotate_Camera_Euler(Vec3_New(-1.0*DeltaTime, 0, 0)); // rotate up (around X axis, pitch)
				break;
			}
			// If a keyboard key was pressed, and it was the Down Arrow key
			// Rotate down (tank controls)
			if (Event.key.keysym.sym == SDLK_DOWN)
			{
				Rotate_Camera_Euler(Vec3_New(1.0*DeltaTime, 0, 0)); // rotate down (around X axis, pitch)
				break;
			}
			// If a keyboard key was pressed, and it was the Q key
			// Move up
			if (Event.key.keysym.sym == SDLK_q)
			{
				vec3_t NewPos = Vec3_New(Get_Camera_Position().x, Get_Camera_Position().y + 3.0 * DeltaTime, Get_Camera_Position().z);
				Set_Camera_Position(NewPos); // Move up
				break;
			}
			// If a keyboard key was pressed, and it was the E key
			// Move down
			if (Event.key.keysym.sym == SDLK_e)
			{
				vec3_t NewPos = Vec3_New(Get_Camera_Position().x, Get_Camera_Position().y - 3.0 * DeltaTime, Get_Camera_Position().z);
				Set_Camera_Position(NewPos); // Move down
				break;
			}
			break;

		default:
			break;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// Process the graphics pipeline stages for all the mesh triangles
///////////////////////////////////////////////////////////////////////////////
// +-------------+
// | Model space |  <-- original mesh vertices
// +-------------+
// |   +-------------+
// `-> | World space |  <-- multiply by world matrix
//     +-------------+
//     |   +--------------+
//     `-> | Camera space |  <-- multiply by view matrix
//         +--------------+
//         |    +------------+
//         `--> |  Clipping  |  <-- clip against the six frustum planes
//              +------------+
//              |    +------------+
//              `--> | Projection |  <-- multiply by projection matrix
//                   +------------+
//                   |    +-------------+
//                   `--> | Image space |  <-- apply perspective divide (NDC - Normalize Device Coordinates)
//                        +-------------+
//                        |    +--------------+
//                        `--> | Screen space |  <-- ready to render
//                             +--------------+
///////////////////////////////////////////////////////////////////////////////

void Process_Graphics_Pipeline_Stages(mesh_t* CurrentMesh)
{
	// Update camera look at target to create view matrix
	vec3_t LookTarget = Get_Camera_LookAt_Target();
	vec3_t WorldUpVector = { 0,1,0 };
	// Create the view matrix
	ViewMatrix = Mat4_Look_At(Get_Camera_Position(), LookTarget, WorldUpVector);

	// Create rotation, scale and translation matrices to multiply the mesh vertices 1 by 1
	mat4_t ScaleMat = Mat4_MakeScale(CurrentMesh->Scale.x, CurrentMesh->Scale.y, CurrentMesh->Scale.z);
	mat4_t RotationXMat = Mat4_MakeRotationX(CurrentMesh->Rotation.x);
	mat4_t RotationYMat = Mat4_MakeRotationY(CurrentMesh->Rotation.y);
	mat4_t RotationZMat = Mat4_MakeRotationZ(CurrentMesh->Rotation.z);
	mat4_t TranslationMat = Mat4_MakeTranslation(CurrentMesh->Position.x, CurrentMesh->Position.y, CurrentMesh->Position.z);

	// Create a World Matrix combining scale, rotation and translation matrices (in that order!)
	WorldMatrix = Mat4_MakeWorldMatrix(ScaleMat, RotationXMat, RotationYMat, RotationZMat, TranslationMat);

	// Multiply the View Matrix by the world matrix to transform the scene to camera space
	WorldMatrix = Mat4_Multiply_Mat4(ViewMatrix, WorldMatrix);

	int NumBerOfFaces = Array_Length(CurrentMesh->Faces);

	// Loop all triangle faces and for each one we render each vertex
	for (int idx = 0; idx < NumBerOfFaces; idx++)
	{
		TriangleFace_t CurrentFace = CurrentMesh->Faces[idx]; // Current face in the loop

		vec3_t FaceVertices[3]; // The 3 vertices of the current looped face
		FaceVertices[0] = CurrentMesh->Vertices[CurrentFace.a]; // Substract 1 from the index cause they start at 1 and not at 0
		FaceVertices[1] = CurrentMesh->Vertices[CurrentFace.b];
		FaceVertices[2] = CurrentMesh->Vertices[CurrentFace.c];

		vec4_t TransformedVertices[3];

		// Loop all 3 vertices of the current face and apply transformations
		for (int ind = 0; ind < 3; ind++)
		{
			vec4_t TransformedVertex = Vec3_To_Vec4(FaceVertices[ind]);

			// Multiply the World Matrix by our original vertices to scale, rotate and translate them (in that order!)
			TransformedVertex = Mat4_Multiply_Vec4(WorldMatrix, TransformedVertex);

			TransformedVertices[ind] = TransformedVertex;
		}

		// Calculate the triangle face normal
		vec3_t FaceNormal = Get_Triangle_Normal(TransformedVertices);

		///////////////  BACK FACE CULLING  ///////////////

		if (Is_Cull_Backface())
		{
			// Find the vector between vertex A in the current triangle face and the camera origin
			vec3_t VertexA = Vec4_To_Vec3(TransformedVertices[0]);
			vec3_t Origin = { 0,0,0 }; // We use the origin because the actual camera position might change while running
			vec3_t CameraRay = Vec3_Subtract(Origin, VertexA);

			// Calculate how aligned the camera ray is with the current face's normal (using the dot product)
			float CameraFaceAlignment = Vec3_Dot(CameraRay, FaceNormal);

			// Bypass the triangles that are looking away from the camera
			if (CameraFaceAlignment < 0)
			{
				continue;
			}
		}

		///////////////  END BACK FACE CULLING  ///////////////

		///////////////  CLIPPING  ///////////////

		// Create a polygon from the original transformed triangle to be clipped
		polygon_t Polygon = Polygon_From_Triangle(
			&TransformedVertices[0], &TransformedVertices[1], &TransformedVertices[2],
			&CurrentFace.aUV, &CurrentFace.bUV, &CurrentFace.cUV
		);
		// Clip the polygon and return a new one with potential new vertices
		Clip_Polygon(&Polygon);
		// Array of triangles returned by the clipped polygon
		Triangle_t TrianglesAfterClipping[MAX_NUM_POLYGON_TRIANGLES];
		int NumTrianglesAfterClipping = 0; // Counter of triangles returned by the clipped polygon
		// Break the polygon back into triangles after clipping
		Triangles_From_Polygon(&Polygon, TrianglesAfterClipping, &NumTrianglesAfterClipping);

		///////////////  END CLIPPING  ///////////////

		// Loop all the assembled triangles after clipping
		for (int tri = 0; tri < NumTrianglesAfterClipping; tri++)
		{
			Triangle_t TriangleAfterClipping = TrianglesAfterClipping[tri];

			vec4_t ProjectedVertex[3];

			// Loop all 3 vertices of the current face and project them into the screen
			for (int ind = 0; ind < 3; ind++)
			{
				// Project the current vertex (multiply the projection mat by the vertices to transform the scene to screen space)
				//ProjectedVertex[ind] = Mat4_Multiply_Vec4_Projection(PerspectiveProjectionMat, TransformedVertices[ind]);
				ProjectedVertex[ind] = Mat4_Multiply_Vec4_Projection(PerspectiveProjectionMat, TriangleAfterClipping.vertex[ind]);

				// Scale the projected vertex into the view
				ProjectedVertex[ind].x *= (WindowWidth / 2.0);
				ProjectedVertex[ind].y *= (WindowHeight / 2.0);

				// Invert the Y value to account for flipped screen Y coordinate
				ProjectedVertex[ind].y *= -1;

				// Move the projected vertex to he middle of the screen
				ProjectedVertex[ind].x += (WindowWidth / 2.0);
				ProjectedVertex[ind].y += (WindowHeight / 2.0);
			}

			/* SIMPLE FLAT (PER TRIANGLE FACE) SHADING */

			// Calculate the shade intensity based on how aligned is the normal of a face with the inverse of the light ray
			//Vec3_Normalize(&SunLight.LightDirection);
			float LightIntensityFactor = -Vec3_Dot(FaceNormal, Get_SunLight().LightDirection);

			// Calculate the resulting triangle color based on the light angle
			CurrentFace.color = Light_Apply_Intensity(CurrentFace.color, LightIntensityFactor);

			Triangle_t CurrentTriToRender =
			{
				.vertex =
				{
					{ProjectedVertex[0].x, ProjectedVertex[0].y, ProjectedVertex[0].z, ProjectedVertex[0].w},
					{ProjectedVertex[1].x, ProjectedVertex[1].y, ProjectedVertex[1].z, ProjectedVertex[1].w},
					{ProjectedVertex[2].x, ProjectedVertex[2].y, ProjectedVertex[2].z, ProjectedVertex[2].w}
				},
				.uvCoordinates =
				{
					{TriangleAfterClipping.uvCoordinates[0].u, TriangleAfterClipping.uvCoordinates[0].v},
					{TriangleAfterClipping.uvCoordinates[1].u, TriangleAfterClipping.uvCoordinates[1].v},
					{TriangleAfterClipping.uvCoordinates[2].u, TriangleAfterClipping.uvCoordinates[2].v}
				},
				.color = CurrentFace.color,
				.texture = CurrentMesh->Texture
			};

			if (NumTrianglesToRender < MAX_TRIANGLES_PER_MESH)
			{
				// Save the projection in the triangles array
				TrianglesToRender[NumTrianglesToRender] = CurrentTriToRender;
				NumTrianglesToRender++;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Update function frame by frame with a fixed time step
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Update(void)
{
	// Wait a couple of miliseconds until target time is reached
	int TimeToWait = FRAME_TARGET_TIME - (SDL_GetTicks() - PreviousFrameTime);
	// Only delay execution if we're going too fast
	if (TimeToWait > 0 && TimeToWait <= FRAME_TARGET_TIME)
	{
		// If The amount of miliseconds passed since last frame (SDL_GetTicks() - PreviousFrameTime) is bigger than
		// the amount of miliseconds required to complete the frame-rate FPS (FRAME_TARGET_TIME - (SDL_GetTicks() - PreviousFrameTime))
		SDL_Delay(TimeToWait);
	}

	/*
		// Doing the delay manually but slower, because while loops are processor instructions
		// so, whenever the OS executes that task, it doesn't know if it should delegate the attention of the processor
		// to other tasks or what it should you, using 100% of the CPU core for this
		while( !SDL_TICKS_PASSED(SDL_GetTicks(), PreviousFrameTime + FRAME_TARGET_TIME) );
	*/

	// Difference between one frame to the next (how long has it been since last frame)
	// This will be a delta time factor converted to seconds to be used to update game objects
	DeltaTime = (SDL_GetTicks() - PreviousFrameTime) / 1000.0; // (miliseconds - miliseconds)/1000 = seconds

	// SDL_GetTicks() = Get how many miliseconds has passed since SDL_Init was called
	PreviousFrameTime = SDL_GetTicks();

	// Initialize the counter of triangles to render for the current frame
	NumTrianglesToRender = 0;

	// Loop all the meshes in the scene
	for (int MeshIdx = 0; MeshIdx < Get_Num_Meshes(); MeshIdx++)
	{
		mesh_t* CurrentMesh = Get_Mesh(MeshIdx);

		// changing (rotating, scaling, translating...etc) the mesh X amount of units per second
		//CurrentMesh->Rotation.x += 0.5 * DeltaTime; // Roll
		//CurrentMesh->Rotation.y += 0.5 * DeltaTime; // Yaw
		//CurrentMesh->Rotation.z += 0.5 * DeltaTime; // Pitch
		//CurrentMesh->Scale.x += 0.1 * DeltaTime;
		//CurrentMesh->Position.z = 5; // Move the vertices away from the camera (point of view) and "inside the monitor"

		// Process graphics pipeline stages for each mesh
		Process_Graphics_Pipeline_Stages(CurrentMesh);
	}

}

void Render_Mode_Selector(Triangle_t CurrentTriangle)
{
	// Draw filled triangles for each face 
	if (Should_Render_Fill_Triangles())
	{
		// Parameter "DrawingMethod" = 0 is DDA Line Rasterization Algorithm, = 1 is Bresenham's
		Draw_Filled_Triangle
		(
			// Vertex A
			CurrentTriangle.vertex[0].x, CurrentTriangle.vertex[0].y, CurrentTriangle.vertex[0].z, CurrentTriangle.vertex[0].w,
			// Vertex B
			CurrentTriangle.vertex[1].x, CurrentTriangle.vertex[1].y, CurrentTriangle.vertex[1].z, CurrentTriangle.vertex[1].w,
			// Vertex C
			CurrentTriangle.vertex[2].x, CurrentTriangle.vertex[2].y, CurrentTriangle.vertex[2].z, CurrentTriangle.vertex[2].w,
			CurrentTriangle.color // Color
		);
	}

	// Draw wireframe triangles for each face 
	if (Should_Render_Wireframe_Triangles())
	{
		// Parameter "DrawingMethod" = 0 is DDA Line Rasterization Algorithm, = 1 is Bresenham's
		Draw_Triangle
		(
			CurrentTriangle.vertex[0].x, CurrentTriangle.vertex[0].y, // Vertex A
			CurrentTriangle.vertex[1].x, CurrentTriangle.vertex[1].y, // Vertex B
			CurrentTriangle.vertex[2].x, CurrentTriangle.vertex[2].y, // Vertex C
			0xFFFFFFFF, // Color
			1 // DrawingMethod
		);
	}

	// Draw textured triangles for each face 
	if (Should_Render_Textured_Triangles())
	{
		// If you don't want perspective correct interpolation, you don't need to pass the Z and W components
		Draw_Textured_Triangle
		(
			// Vertex A
			CurrentTriangle.vertex[0].x, CurrentTriangle.vertex[0].y, CurrentTriangle.vertex[0].z, CurrentTriangle.vertex[0].w,
			// Vertex B
			CurrentTriangle.vertex[1].x, CurrentTriangle.vertex[1].y, CurrentTriangle.vertex[1].z, CurrentTriangle.vertex[1].w,
			// Vertex C
			CurrentTriangle.vertex[2].x, CurrentTriangle.vertex[2].y, CurrentTriangle.vertex[2].z, CurrentTriangle.vertex[2].w,
			CurrentTriangle.uvCoordinates[0].u, CurrentTriangle.uvCoordinates[0].v, // UV Vertex A
			CurrentTriangle.uvCoordinates[1].u, CurrentTriangle.uvCoordinates[1].v, // UV Vertex B
			CurrentTriangle.uvCoordinates[2].u, CurrentTriangle.uvCoordinates[2].v, // UV Vertex C
			CurrentTriangle.texture
		);
	}

	// Draw triangle vertex points for each face
	if (Should_Render_Triangle_Vertices())
	{
		// Draw rectangles for each projected triangle vertex, translated to the middle of the screen
		Draw_Rectangle(CurrentTriangle.vertex[0].x, CurrentTriangle.vertex[0].y, 6, 6, 0xFFFF0000);
		Draw_Rectangle(CurrentTriangle.vertex[1].x, CurrentTriangle.vertex[1].y, 6, 6, 0xFFFF0000);
		Draw_Rectangle(CurrentTriangle.vertex[2].x, CurrentTriangle.vertex[2].y, 6, 6, 0xFFFF0000);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Render function to draw objects on the display
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Render(void)
{
	// Set the color of the renderer that you want to paint the screen with
	// RGBA format, 0-255
	//SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);

	// Clear the renderer with the choosed color 
	//SDL_RenderClear(Renderer);

	Clear_ColorBuffer(0x0000000);
	Clear_ZBuffer();

	// Loop all the projected triangles and render them
	for (int idx = 0; idx < NumTrianglesToRender; idx++)
	{
		Triangle_t CurrentTriangle = TrianglesToRender[idx];

		Render_Mode_Selector(CurrentTriangle);
	}

	//Draw_Grid(10, 0xFF333333);
	// Update the screen, presenting the backbuffer that contains the stuff you want to draw
	Render_ColorBuffer();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Main function
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* args[])
{
	bIsRunning = Initialize_Window();
	Setup();

	while (bIsRunning)
	{
		Process_Input();
		Update();
		Render();
	}

	Destroy_Window();
	Free_Meshes();

	return 0;
}