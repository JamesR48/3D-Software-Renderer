#pragma once

#ifndef DISPLAY_H // Preprocessor directive: ensures that this is only included once
#define DISPLAY_H

#include <stdint.h> // For the new fixed types
#include <stdbool.h>
#include <SDL.h>

#define FPS 30 // Update loop Frames Per Second
#define FRAME_TARGET_TIME (1000/FPS) // How many miliseconds each frame should take in order to complete 30FPS

// Declare a new typedef to hold unsigned 32bit color values
typedef uint32_t color_t;

enum ECull_Mode {
	CULL_NONE,
	CULL_BACKFACE
};

enum ERender_Mode {
	RENDER_WIRE,
	RENDER_WIRE_VERTEX,
	RENDER_FILL_TRIANGLE,
	RENDER_FILL_TRIANGLE_WIRE,
	RENDER_TEXTURED,
	RENDER_TEXTURED_WIRE
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Getters/Setters
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int Get_Window_Width(void);
int Get_Window_Height(void);
float Get_ZBuffer_At(int x, int y);
void Update_ZBuffer_At(int x, int y, float value);
void Set_Render_Mode(int Mode);
void Set_Cull_Mode(int Mode);

bool Is_Cull_Backface(void);
bool Should_Render_Fill_Triangles(void);
bool Should_Render_Wireframe_Triangles(void);
bool Should_Render_Textured_Triangles(void);
bool Should_Render_Triangle_Vertices(void);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Creating a SDL Window
bool Initialize_Window(void);
// Clear the Color Buffer (it's like animating in a white board: you erase the previous frame and draw the new one on top)
void Clear_ColorBuffer(color_t ClearColor);
// Clear the ZBuffer (restart all its values with one)
void Clear_ZBuffer(void);
// Copy all the Color Buffer's pixels in a texture and displays it
void Render_ColorBuffer(void);
void Draw_Pixel(int x, int y, color_t Color);
void DrawLine_Bresenham(int x0, int y0, int x1, int y1, color_t LineColor);
void DrawLine_DDA(int x0, int y0, int x1, int y1, color_t LineColor);
void Draw_Triangle(int x0, int y0, int x1, int y1, int x2, int y2, color_t Color, int DrawingMethod);
void Draw_Grid(int CellSpacing, color_t LineColor);
void Draw_Rectangle(int x, int y, int Width, int Height, color_t Color);
// Release anything that was allocated
void Destroy_Window(void);

#endif // !DISPLAY_H

