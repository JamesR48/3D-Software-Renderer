#include <stdio.h>
#include <stdint.h> // For the new fixed types
#include <stdbool.h>
#include <SDL.h>
#include "Display.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global Variables
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// static means: the value of this variables is going to persist across function calls,
// and they're only visible in the file where they were declared  
static SDL_Window* Window = NULL; 
static SDL_Renderer* Renderer = NULL;

//Color Buffer: sequence/array of hex numbers (colors) allocated in memory that'll be rendered/painted by SDL
// Color Buffer = ScreenWidth x ScreenHeight (all the screen's pixels)
//	0 x (R)(G)(B)(A) = 0xFF0000FF [this is the Red color in hex] 
//  0 x  FF 00 00 FF	 [32bits number = 1st byte is Red, 2nd is Green, 3rd is Blue, 4th is Alpha]
// So we use a uint32_t (fixed-size unsigned 32bits integer new type) to represent the hex colors
// that because the size of an int depends on the architecture and compiler, but, the colors must be 32bits
static color_t* ColorBuffer = NULL; // Declare a pointer to the 1st position of an array of uint32 elements

static SDL_Texture* ColorBufferTexture = NULL; // SDL Texture used to display the Color Buffer

static float* ZBuffer = NULL; // Array containing the depth value of each pixel that'll be rendered

static int WindowWidth = 320; //800;
static int WindowHeight = 200; //600;

static int CullMode = CULL_BACKFACE;

static int RenderMode = RENDER_WIRE;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Getters/Setters
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int Get_Window_Width(void)
{
	return WindowWidth;
}

int Get_Window_Height(void)
{
	return WindowHeight;
}

float Get_ZBuffer_At(int x, int y)
{
	if (x < 0 || x >= WindowWidth || y < 0 || y >= WindowHeight)
	{
		return 1.0;
	}
	return ZBuffer[(WindowWidth*y)+x];
}

void Update_ZBuffer_At(int x, int y, float value)
{
	if (x < 0 || x >= WindowWidth || y < 0 || y >= WindowHeight)
	{
		return;
	}
	ZBuffer[(WindowWidth*y) + x] = value;
}

void Set_Render_Mode(int Mode)
{
	RenderMode = Mode;
}

void Set_Cull_Mode(int Mode)
{
	CullMode = Mode;
}

bool Is_Cull_Backface(void)
{
	if (CullMode == CULL_BACKFACE)
	{
		return true;
	}
	return false;
}

bool Should_Render_Fill_Triangles(void)
{
	return (RenderMode == RENDER_FILL_TRIANGLE || RenderMode == RENDER_FILL_TRIANGLE_WIRE);
}

bool Should_Render_Wireframe_Triangles(void)
{
	return (RenderMode == RENDER_WIRE ||
		RenderMode == RENDER_WIRE_VERTEX ||
		RenderMode == RENDER_FILL_TRIANGLE_WIRE ||
		RenderMode == RENDER_TEXTURED_WIRE
		);
}

bool Should_Render_Textured_Triangles(void)
{
	return (RenderMode == RENDER_TEXTURED || RenderMode == RENDER_TEXTURED_WIRE);
}

bool Should_Render_Triangle_Vertices(void)
{
	return (RenderMode == RENDER_WIRE_VERTEX);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Creating a SDL Window
bool Initialize_Window(void)
{
	//If there're no errors while initializing SDL's input, graphics, and stuff
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		fprintf(stderr, "Error initializig SDL! \n");
		return false;
	}

	// Use SDL to query what's te fullscreen max. width and height
	SDL_DisplayMode DisplayMode;
	SDL_GetCurrentDisplayMode(0, &DisplayMode); // DisplayMode is an Out Param
	int FullScreenWidth = DisplayMode.w;
	int FullScreenHeight = DisplayMode.h;

	WindowWidth = FullScreenWidth / 4;
	WindowHeight = FullScreenHeight / 4;

	// Create SDL window 
	Window = SDL_CreateWindow
	(
		NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, FullScreenWidth, FullScreenHeight, SDL_WINDOW_MAXIMIZED
	);

	if (Window == NULL)
	{
		fprintf(stderr, "Error creating SDL window! \n");
		return false;
	}

	// Create SDL renderer
	Renderer = SDL_CreateRenderer(Window, -1, 0);
	if (Renderer == NULL)
	{
		fprintf(stderr, "Error creating SDL renderer! \n");
		return false;
	}

	// SDL_WINDOW_FULLSCREEN, for "real" fullscreen with a videomode change
	// SDL_WINDOW_FULLSCREEN_DESKTOP for "fake" fullscreen that takes the size of the desktop
	//SDL_SetWindowFullscreen(Window, SDL_WINDOW_FULLSCREEN);

	// Dynamically allocate a certain number of bytes in the heap for the Color Buffer (casting the allocation to uint32_t*)
	// (the size that each pixel needs to store a color * window width * window height)
	// sizeof is a language operator, not a function. It's provided by the compiler and it's implementation specific.
	ColorBuffer = (color_t*)malloc(sizeof(color_t) * WindowWidth * WindowHeight);
	if (ColorBuffer == NULL)
	{
		// If malloc returns a NULL pointer, the allocation wasn't successful (maybe the machine doesn't have enough free memory)
		return;
	}
	//ColorBuffer[0] = 0xFF0000FF; // Set the 1st pixel to Blue
	// Since this is a linear array, to get any specific pixel we do (window width * its row) + its column
	//ColorBuffer[(WindowWidth * 10) + 20] = 0xFFFF0000; // Set the pixel at row 10, column 20 to Red

	// The format for each pixel in the texture is RGBA (in that order) each of 8bits, hence RGBA32 (8bits x 4 channels)
	// The texture will be continuosly streamed since it will be updated frame by frame
	ColorBufferTexture = SDL_CreateTexture
	(
		Renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, WindowWidth, WindowHeight
	);

	// Dynamically allocate a certain number of bytes in the heap for the Z/depth Buffer (casting the allocation to float*)
	ZBuffer = (float*)malloc(sizeof(float) * WindowWidth * WindowHeight);

	return true;
}

// Clear the Color Buffer (it's like animating in a white board: you erase the previous frame and draw the new one on top)
void Clear_ColorBuffer(color_t ClearColor)
{
	//for (int Row = 0; Row < WindowHeight; Row++) // Rows
	//{
	//	for (int Col = 0; Col < WindowWidth; Col++) // Columns
	//	{
	//		ColorBuffer[( (WindowWidth * Row) + Col)] = ClearColor;
	//	}
	//}

	//memset(ColorBuffer, 0, WindowWidth * WindowHeight * 4);

	int WindowSize = WindowWidth * WindowHeight;
	for (int idx = 0; idx < WindowSize; idx++)
	{
		ColorBuffer[idx] = ClearColor;
	}

	/*for (int idx = 0; idx < WindowSize; idx += 4)
	{
		ColorBuffer[idx] = ClearColor;
		ColorBuffer[idx + 1] = ClearColor;
		ColorBuffer[idx + 2] = ClearColor;
		ColorBuffer[idx + 3] = ClearColor;
	}*/
}

// Clear the ZBuffer (restart all its values with one)
void Clear_ZBuffer(void)
{
	int WindowSize = WindowWidth * WindowHeight;
	for (int idx = 0; idx < WindowSize; idx++)
	{
		ZBuffer[idx] = 1.0;
	}
}

// Copy all the Color Buffer's pixels in a texture and displays it
void Render_ColorBuffer(void)
{
	// Texture pitch = size of each row (how many bytes for each row of the texture)
	SDL_UpdateTexture(ColorBufferTexture, NULL, ColorBuffer, (int)(WindowWidth * sizeof(color_t)));
	SDL_RenderCopy(Renderer, ColorBufferTexture, NULL, NULL);

	// Update the screen, presenting the backbuffer that contains the stuff you want to draw
	SDL_RenderPresent(Renderer);
}

void Draw_Pixel(int x, int y, color_t Color)
{
	if ( (x < 0) || (x >= WindowWidth) || (y < 0) || (y >= WindowHeight) )
	{
		return;
	}

	// (Total rows in the screen * how many rows) + how many columns
	ColorBuffer[(WindowWidth * y) + x] = Color;
}

void DrawLine_Bresenham(int x0, int y0, int x1, int y1, color_t LineColor)
{

	int DeltaX = x1 - x0;
	int DeltaY = y1 - y0;
	int AbsDeltaX = abs(DeltaX);
	int AbsDeltaY = abs(DeltaY);

	int CurrentX = x0;
	int CurrentY = y0;

	Draw_Pixel(CurrentX, CurrentY, LineColor);

	if ((x0 == x1) && (y0 == y1)) return;

	// Slope < 1
	if (AbsDeltaX > AbsDeltaY)
	{
		int Step = 2*AbsDeltaY - AbsDeltaX;

		for(int idx = 0; idx <= AbsDeltaX; idx++)
		{
			CurrentX = (DeltaX < 0) ? CurrentX-1 : CurrentX+1;
			if (Step < 0)
			{
				Step += 2*AbsDeltaY;
			}
			else
			{
				CurrentY = (DeltaY < 0) ? CurrentY - 1 : CurrentY + 1;
				Step += (2 * AbsDeltaY - 2*AbsDeltaX);
			}

			Draw_Pixel(CurrentX, CurrentY, LineColor);
		}
	}
	// Slope >= 1
	else
	{
		int Step = 2 * AbsDeltaX - AbsDeltaY;

		for (int idx = 0; idx <= AbsDeltaY; idx++)
		{
			CurrentY = (DeltaY < 0) ? CurrentY - 1 : CurrentY + 1;
			if (Step < 0)
			{
				Step += 2 * AbsDeltaX;
			}
			else
			{
				CurrentX = (DeltaX < 0) ? CurrentX - 1 : CurrentX + 1;
				Step += (2 * AbsDeltaX - 2 * AbsDeltaY);
			}

			Draw_Pixel(CurrentX, CurrentY, LineColor);
		}
	}
}

// Line Drawing with the DDA algorithm
void DrawLine_DDA(int x0, int y0, int x1, int y1, color_t LineColor)
{
	// The heart of this algorithm lies in the fact that once we find the X and Y increments, these values will always
	// be the same for each pass of the loop

	int DeltaX = x1 - x0;
	int DeltaY = y1 - y0;

	// How much the algorithm needs to run to draw the whole line, aka. step
	// Sometimes DeltaY is greater than DeltaX (really steep line), meaning we need to run the total DeltaY side length
	// instead of DeltaX	
	int LongestSideLength = ( abs(DeltaX) >= abs(DeltaY) ) ? abs(DeltaX) : abs(DeltaY);

	// Find how much we should increment in both x and Y each step
	float Xincrement = DeltaX / (float)LongestSideLength;
	float Yincrement = DeltaY / (float)LongestSideLength; // Rise / Run , aka. how steep is the inclination of the line

	// Keep track of current X and Y in each loop iteration, starting from X0 , Y0
	float CurrentX = (float)x0;
	float CurrentY = (float)y0;

	// Loop from origin to end of the line, drawing a pixel for each current point and incrementing them respectively
	for (int idx = 0; idx <= LongestSideLength; idx++)
	{
		// Get the closer integer value to the current float point and draw a pixel there
		Draw_Pixel(round(CurrentX), round(CurrentY), LineColor);
		CurrentX += Xincrement;
		CurrentY += Yincrement;
	}
}

void Draw_Triangle(int x0, int y0, int x1, int y1, int x2, int y2, color_t Color, int DrawingMethod)
{
	// Draw line with DDA method
	if (DrawingMethod == 0)
	{
		DrawLine_DDA(x0, y0, x1, y1, Color);
		DrawLine_DDA(x1, y1, x2, y2, Color);
		DrawLine_DDA(x0, y0, x2, y2, Color);
	}
	// Draw line with Bresenham method
	else if (DrawingMethod == 1)
	{
		DrawLine_Bresenham(x0, y0, x1, y1, Color);
		DrawLine_Bresenham(x1, y1, x2, y2, Color);
		DrawLine_Bresenham(x0, y0, x2, y2, Color);
	}
}

void Draw_Grid(int CellSpacing, color_t LineColor)
{

	// Horizontal Grid Lines
	for (int Row = 0; Row < WindowHeight; Row += CellSpacing) // Rows
	{
		for (int Col = 0; Col < WindowWidth; Col++) // Columns
		{
			ColorBuffer[((WindowWidth * Row) + Col)] = LineColor;
		}
	}

	// Vertical Grid Lines
	for (int Row = 0; Row < WindowHeight; Row++) // Rows
	{
		for (int Col = 0; Col < WindowWidth; Col += CellSpacing) // Columns
		{
			ColorBuffer[((WindowWidth * Row) + Col)] = LineColor;
		}
	}
}

void Draw_Rectangle(int x, int y, int Width, int Height, color_t Color)
{
	for (int Row = 0; Row < Width; Row++) // Rows
	{
		for (int Col = 0; Col < Height; Col++) // Columns
		{
			int CurrentX = Row + x;
			int CurrentY = Col + y;
			Draw_Pixel(CurrentX, CurrentY, Color);
		}
	}
}

// Release anything that was allocated
void Destroy_Window(void)
{
	// Free the memory in the reverse order that it was allocated
	free(ZBuffer);
	free(ColorBuffer);
	SDL_DestroyTexture(ColorBufferTexture);
	SDL_DestroyRenderer(Renderer);
	SDL_DestroyWindow(Window);
	SDL_Quit();
}