#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Array.h"
#include "Vector.h"
#include "Mesh.h"

#define MAX_NUM_MESHES 100
static mesh_t Meshes[MAX_NUM_MESHES]; // Array of meshes
static int MeshesCount = 0; // Counter of meshes in the array

mesh_t Mesh =
{
	.Vertices = NULL,
	.Faces = NULL,
	.Rotation = {0,0,0},
	.Scale = {1,1,1},
	.Position = {0,0,0}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Define the array of vertices/points that will make a cube mesh
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
			 (-1,1,1)----------------(1,1,1)								 7 ---------------- 5
				   /|                 /|									/|                 /|
				  / |                / |								   / |                / |
				 /  |               /  |								  /  |               /  |
				/   |              /   |								 /   |              /   |
		(-1,1,-1) ---------------(1,1,-1)    						2 ---------------- 3    |
			   |    |             |    |								|    |             |    |
			   |    |             |    |								|    |             |    |
			   | (-1,-1,1)--------|--(1,-1,1)						|   8 -------------|--- 6
			   |   /              |   /								|   /              |   /
			   |  /               |  /								|  /               |  /
			   | /                | /								| /                | /
			   |/                 |/									|/                 |/
	   (-1,-1,-1)---------------(1,-1,-1)							1 ---------------- 4
*/

/*
	( V ) ^
	     1|
	      |
	      |
	      |
	      |
	     0 -------------> 1 ( U )
*/

// The indexing starts at 1 and not 0 because OBJ files use that as convention

vec3_t CubeVertices[N_CUBE_VERTICES] =
{
	{.x = -1,.y = -1,.z = -1}, // 1
	{.x = -1,.y = 1,.z = -1}, // 2
	{.x = 1,.y = 1,.z = -1}, // 3
	{.x = 1,.y = -1,.z = -1}, // 4
	{.x = 1,.y = 1,.z = 1}, // 5
	{.x = 1,.y = -1,.z = 1}, // 6
	{.x = -1,.y = 1,.z = 1}, // 7
	{.x = -1,.y = -1,.z = 1}, // 8
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Define the array of cube triangle faces
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Going clockwise order
TriangleFace_t CubeFaces[N_CUBE_FACES] =
{
	// Front
	{.a = 1,.b = 2,.c = 3, .aUV = {0,1},.bUV = {0,0},.cUV = {1,0}, .color = 0xFFFF0000},
	{.a = 1,.b = 3,.c = 4, .aUV = {0,1},.bUV = {1,0},.cUV = {1,1}, .color = 0xFFFF0000},
	// Right
	{.a = 4,.b = 3,.c = 5, .aUV = {0,1},.bUV = {0,0},.cUV = {1,0}, .color = 0xFF00FF00},
	{.a = 4,.b = 5,.c = 6, .aUV = {0,1},.bUV = {1,0},.cUV = {1,1}, .color = 0xFF00FF00},
	// Back
	{.a = 6,.b = 5,.c = 7,.aUV = {0,1},.bUV = {0,0},.cUV = {1,0}, .color = 0xFF0000FF},
	{.a = 6,.b = 7,.c = 8,.aUV = {0,1},.bUV = {1,0},.cUV = {1,1}, .color = 0xFF0000FF},
	// Left
	{.a = 8,.b = 7,.c = 2,.aUV = {0,1},.bUV = {0,0},.cUV = {1,0}, .color = 0xFFFFFF00},
	{.a = 8,.b = 2,.c = 1,.aUV = {0,1},.bUV = {1,0},.cUV = {1,1}, .color = 0xFFFFFF00},
	// Top
	{.a = 2,.b = 7,.c = 5,.aUV = {0,1},.bUV = {0,0},.cUV = {1,0}, .color = 0xFFFF00FF},
	{.a = 2,.b = 5,.c = 3,.aUV = {0,1},.bUV = {1,0},.cUV = {1,1}, .color = 0xFFFF00FF},
	// Bottom
	{.a = 6,.b = 8,.c = 1,.aUV = {0,1},.bUV = {0,0},.cUV = {1,0}, .color = 0xFF00FFFF},
	{.a = 6,.b = 1,.c = 4,.aUV = {0,1},.bUV = {1,0},.cUV = {1,1}, .color = 0xFF00FFFF},
};

void Load_Cube_Mesh_Data(void)
{
	for (int idx = 0; idx < N_CUBE_VERTICES; idx++)
	{
		vec3_t CurrentCubeVertex = CubeVertices[idx];
		Array_Push(Mesh.Vertices, CurrentCubeVertex);
	}

	for (int idx = 0; idx < N_CUBE_FACES; idx++)
	{
		TriangleFace_t CurrentCubeFace = CubeFaces[idx];
		Array_Push(Mesh.Faces, CurrentCubeFace);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Load an OBJ file and save its data into our Mesh data structure
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Load_Mesh_OBJ_File_Data(mesh_t* Mesh, char* FileName)
{
	errno_t FileOpeningError;
	int FileScanError;
	FILE* OBJModel;
	FileOpeningError = fopen_s(&OBJModel, FileName, "r");
	
	if (FileOpeningError != 0)
	{
		printf("The file %s was not opened\n", FileName);
	}

	if (OBJModel == NULL)
	{
		printf("Error: could not open file %s", FileName);
		return 1;
	}
	else
	{
		printf("Opened: %s \n", FileName);

		char FileLine[512]; // Each line of the file will have a max. of 512 chars

		tex2_t* VertexTextureCoordinates = NULL; // Dynamic array of texture coordinates per found vertex

		 // Get a line from a file as string
		while (fgets(FileLine, 512, OBJModel))
		{
			/* Getting Vertex Data from file */

			// Compare 2 strings specifying the number of chars you want to look for
			if (strncmp(FileLine, "v ", 2) == 0) // if the first 2 characters of both strings are equal
			{
				vec3_t VertexFromFile;
				FileScanError = sscanf_s(&FileLine, "v %f %f %f", &VertexFromFile.x, &VertexFromFile.y, &VertexFromFile.z);
				if (FileScanError == EOF)
				{
					printf("Error getting vertex data from file");
					continue;
				}
				
				Array_Push(Mesh->Vertices, VertexFromFile);
			}

			/* Getting Texture Coordinates from file */

			if(strncmp(FileLine, "vt ", 3) == 0) // if the first 3 characters of both strings are equal
			{
				tex2_t TextureCoord;
				FileScanError = sscanf_s(&FileLine, "vt %f %f", &TextureCoord.u, &TextureCoord.v);
				if (FileScanError == EOF)
				{
					printf("Error getting texture coordinates from file");
					continue;
				}

				Array_Push(VertexTextureCoordinates, TextureCoord);
			}

			/* Getting Face Data from file */

			// Compare 2 strings specifying the number of chars you want to look for
			if (strncmp(FileLine, "f ", 2) == 0) // if the first 2 characters of both strings are equal
			{
				int VertexIndices[3];
				int TextureIndices[3];
				int NormalIndices[3];

				FileScanError = sscanf_s
				(
					&FileLine, "f %d/%d/%d %d/%d/%d %d/%d/%d",
					&VertexIndices[0], &TextureIndices[0], &NormalIndices[0],
					&VertexIndices[1], &TextureIndices[1], &NormalIndices[1],
					&VertexIndices[2], &TextureIndices[2], &NormalIndices[2]
				);

				if (FileScanError == EOF)
				{
					printf("Error getting face data from file");
					continue;
				}

				// Substract 1 from the indices because they start at 1 and not at 0
				TriangleFace_t FaceFromFile =
				{
					.a = VertexIndices[0] -1,
					.b = VertexIndices[1] -1,
					.c = VertexIndices[2] -1,
					.aUV = VertexTextureCoordinates[ TextureIndices[0] -1 ],
					.bUV = VertexTextureCoordinates[TextureIndices[1] -1 ],
					.cUV = VertexTextureCoordinates[TextureIndices[2] -1 ],
					.color = 0xFFE7E7E7 //808080
				};
				
				Array_Push(Mesh->Faces, FaceFromFile);
			}
		}

		Array_Free(VertexTextureCoordinates);

		if ( fclose(OBJModel) != 0)
		{
			printf("Error closing the file %s", FileName);
			return 1;
		}
	}	
}

void Load_Mesh_PNG_Data(mesh_t * Mesh, char * FileName)
{
	upng_t* PNGTexture = upng_new_from_file(FileName); // Load a texture from an especific file address
	if (PNGTexture != NULL)
	{
		upng_decode(PNGTexture); // Load all the data (width, height, buffer of colors in memory...)
		if (upng_get_error(PNGTexture) == UPNG_EOK) // If there're no errors from the decoding
		{
			//MeshTexture = (color_t*)upng_get_buffer(PNGTexture); // Get and save the texture's buffer of colors (casted as uint32_t)
			Mesh->Texture = PNGTexture; // Assign the loaded PNG texture to the current mesh
		}
	}
}

void Load_Mesh(char * OBJFileName, char* PNGFileName, vec3_t Scale, vec3_t Pos, vec3_t Rot)
{
	// Load the OBJ file to our mesh at MeshesCount position in the Meshes array
	Load_Mesh_OBJ_File_Data(&Meshes[MeshesCount], OBJFileName);
	// Load the PNG file data to our mesh texture at MeshesCount position in the Meshes array
	Load_Mesh_PNG_Data(&Meshes[MeshesCount], PNGFileName);
	// Initialize the current mesh's scale, position, and rotation
	Meshes[MeshesCount].Scale = Scale;
	Meshes[MeshesCount].Position = Pos;
	Meshes[MeshesCount].Rotation = Rot;
	// Add the new mesh to the meshes arary and update the counter
	MeshesCount++;
}

mesh_t* Get_Mesh(int Index)
{
	return &Meshes[Index];
}

int Get_Num_Meshes(void)
{
	return MeshesCount;
}

void Rotate_Mesh_Euler(int Index, vec3_t Rot)
{
	Meshes[Index].Rotation = Vec3_Add(Meshes[Index].Rotation, Rot);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Free the memory that was dynamically allocated by the program
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Free_Meshes(void)
{
	for (int idx = 0; idx < MeshesCount; idx++) {
		Array_Free(Meshes[idx].Faces);
		Array_Free(Meshes[idx].Vertices);
		upng_free(Meshes[idx].Texture);
	}
}
