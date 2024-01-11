#include <stdio.h>
#include "Swap.h"
#include "Triangle.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Return the barycentric weights (alpha, beta, gamma) for point P inside a triangle ABC
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
		          A
		         /|\
		        / | \
		       /  |  \
			  /   |   \
		  Gamma   p   Beta
		    /   /   \   \
		   /  /       \  \
		  / /   Alpha   \ \
		 B --------------- C
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

vec3_t Barycentric_Weights(vec2_t A, vec2_t B, vec2_t C, vec2_t P)
{
	// Find the vectors between the vertices ABC and point P
	vec2_t SubBA = Vec2_Subtract(B, A);
	vec2_t SubCP = Vec2_Subtract(C, P);
	vec2_t SubCA = Vec2_Subtract(C, A);
	vec2_t SubAP = Vec2_Subtract(A, P);
	vec2_t SubBP = Vec2_Subtract(B, P);

	// Calculate the area of the full triangle ABC using cross product (area of parallelogram / 2)
	float AreaTriABC = (SubBA.x * SubCA.y) - (SubBA.y * SubCA.x);

	// Weight alpha is the area of the sub-triangle PBC divided by the area of the full triangle ABC
	float Alpha = ( (SubBP.x * SubCP.y) - (SubBP.y * SubCP.x) ) / AreaTriABC;

	// Weight beta is the area of the sub-triangle PCA divided by the area of the full triangle ABC
	float Beta = ((SubCP.x * SubAP.y) - (SubCP.y * SubAP.x)) / AreaTriABC;

	//printf("PB.X: %f, CB.X: %f, PB.Y: %f, CB.Y: %F \n", SubPB.x, SubCB.x, SubPB.y, SubCB.y);

	// Weight gamma is found by the property that the 3 barycentric coordinates weights (alpha, beta, gamma) always add up to 1
	// (alhpa + beta + gamma) = 1 ----> gamma = (1 - alpha - beta)
	float Gamma = 1 - Alpha - Beta;

	//printf("Area: %f, Alpha: %f, Beta: %f, Gamma: %f \n", AreaTriABC, Alpha, Beta, Gamma);

	vec3_t Weights = {Alpha, Beta, Gamma};
	return Weights;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Draw a pixel witth solid color at position (X,Y) of a triangle using interpolation
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Draw_Triangle_Pixel(
	int x, int y,
	vec4_t VertexA, vec4_t VertexB, vec4_t VertexC, // If you don't want perspective correct interpolation, use a vec2 (just x and y)
	color_t Color
	)
{
	// The vertex P of the barycentric coordinates is going to be the current pixel in position (X,Y) of the triangle
	vec2_t CurrentPixel = { x, y };
	vec2_t VertA = Vec4_To_Vec2(VertexA);
	vec2_t VertB = Vec4_To_Vec2(VertexB);
	vec2_t VertC = Vec4_To_Vec2(VertexC);

	vec3_t BarycentricWeights = Barycentric_Weights(VertA, VertB, VertC, CurrentPixel);
	// Extract the weights alpha, beta and gamma from the resulting barycentric coordinates weights calculation
	float Alpha = BarycentricWeights.x;
	float Beta = BarycentricWeights.y;
	float Gamma = BarycentricWeights.z;

	// Perform the interpolation of 1/W for the current pixel
	float Simplified0 = Alpha * VertexB.w * VertexC.w;
	float Simplified1 = Beta * VertexA.w * VertexC.w;
	float Simplified2 = Gamma * VertexA.w * VertexB.w;
	float SimplifiedAddition = Simplified0 + Simplified1 + Simplified2;
	float WMultiplication = VertexA.w * VertexB.w * VertexC.w;
	float ReciprocalW = SimplifiedAddition / WMultiplication;
	// Adjust the reciprocal W so the pixels that are closer to the camera have smaller values
	ReciprocalW = 1.0 - ReciprocalW;

	// Only draw the current pixel ih its depth value is less than what was already previously stored in the ZBuffer
	// Basically, if the current pixel that is going to be drawn is in front of was already there
	// Make sure the current pixel index is inside the screen! (that's achived with the % operation)
	//int PixelIndex = ((Get_Window_Width() * y) + x) % (Get_Window_Width()*Get_Window_Height()); // (windowWith * current row) + current column
	//if (ReciprocalW < ZBuffer[PixelIndex])
	if (ReciprocalW < Get_ZBuffer_At(x, y))
	{
		// Draw a pixel at position X,Y with the especified solid color
		Draw_Pixel(x, y, Color);

		// Update the Z/Depth Buffer value with the calculated reciprocal W (1/W) of this current pixel
		// ZBuffer[PixelIndex] = InterpolatedReciprocalW; // This is with the "less performant" way
		//ZBuffer[PixelIndex] = ReciprocalW;
		Update_ZBuffer_At(x, y, ReciprocalW);
	}
}

///////////////////////////////////////////////////////////////////////////////
// Draw a filled triangle with the flat-top/flat-bottom method
// We split the original triangle in 2, half flat-bottom and half flat-top
///////////////////////////////////////////////////////////////////////////////
/*
			  (x0,y0) A
				/   \
			   /     \
			  /       \
			 /         \
			/           \
	 B (x1,y1)-----------\
		   \_             \
			  \_           \
				 \ _        \
					 \_      \
						\     \
						  \_   \
							 \_ \
								\
							 (x2,y2) C
*/
///////////////////////////////////////////////////////////////////////////////
void Draw_Filled_Triangle(
	int x0, int y0, float z0, float w0,
	int x1, int y1, float z1, float w1,
	int x2, int y2, float z2, float w2,
	color_t Color
	)
{
	// We need to sort the vertices by Y-coordinate ascending (y0 < y1 < y2)

	if (y0 > y1) // If y0 is greater than y1, swap their values (X and Y)
	{
		Integer_Swap(&y0, &y1);
		Integer_Swap(&x0, &x1);
		Float_Swap(&z0, &z1); // If you don't want perspective correct interpolation, you don't need to pass the Z and W components
		Float_Swap(&w0, &w1);
	}
	if (y1 > y2) // If y1 is greater than y2, swap their values (X and Y)
	{
		Integer_Swap(&y1, &y2);
		Integer_Swap(&x1, &x2);
		Float_Swap(&z1, &z2);
		Float_Swap(&w1, &w2);
	}
	if (y0 > y1) // If after swapping y1 and y2, y0 is greater than y1, then swap their values(X and Y)
	{
		Integer_Swap(&y0, &y1);
		Integer_Swap(&x0, &x1);
		Float_Swap(&z0, &z1);
		Float_Swap(&w0, &w1);
	}

	// Create vectors containing the sorted vertices and texture coords with the sorted UVs
	vec4_t VertexA = { x0, y0, z0, w0 }; // If you don't want perspective correct interpolation, use a vec2 (just x and y)
	vec4_t VertexB = { x1, y1, z1, w1 };
	vec4_t VertexC = { x2, y2, z2, w2 };

	/////////////////////  DRAW THE UPPER PART OF THE TRIANGLE (FLAT-BOTTOM) ///////////////////////////////////

	// Calculate the 2 slopes from each triangle "leg"
	// Because Y is always going to change 1 by 1, then we calculate the inverse of the slope
	// So, instead of doing slope = Dy/Dx as usual, we do slope = Dx/Dy
	// Meaning we're searching the change in X per Y, not Y per X
	float DY1 = abs(y1 - y0);
	float DY2 = abs(y2 - y0);
	float InverseSlope1 = 0;
	float InverseSlope2 = 0;

	// Initialize XStart and XEnd
	int XStart = 0;
	int XEnd = 0;

	if (DY1 != 0 && DY2 != 0)
	{
		InverseSlope1 = (float)(x1 - x0) / DY1;
		InverseSlope2 = (float)(x2 - x0) / DY2;

		// Loop all scanlines from top to middle
		for (int y = y0; y <= y1; y++)
		{
			// XStart is the left X value (x1) + the difference between current Y in the loop and the left Y (y-y1)
				// multiplied by the left inverted slope
			XStart = x1 + ((y - y1)*InverseSlope1);
			// XEnd is the right X value (x2) + the difference between current Y in the loop and the right Y (y-y2)
			// multiplied by the right inverted slope
			XEnd = x2 + ((y - y2)*InverseSlope2);

			// Ensure that XStart is always to the left of XEnd (this could be inverted because of the vertices order of some faces)
			if (XStart > XEnd)
			{
				Integer_Swap(&XStart, &XEnd);
			}

			// For each looped row, draw a texture pixel in every column, from XStart to XEnd
			for (int x = XStart; x < XEnd; x++)
			{
				// Draw a pixel with the especified solid color and drawing method (line drawing algorithm)
				Draw_Triangle_Pixel(x, y, VertexA, VertexB, VertexC, Color);
			}
		}
	}

	/////////////////////  DRAW THE Bottom PART OF THE TRIANGLE (FLAT-TOP) ///////////////////////////////////

	// Calculate the inverted slope of each leg, now for the bottom half of the triangle
	DY1 = abs(y2 - y1);
	DY2 = abs(y2 - y0);
	if (DY1 != 0 && DY2 != 0)
	{
		InverseSlope1 = (float)(x2 - x1) / DY1;
		InverseSlope2 = (float)(x2 - x0) / DY2;

		// Initialize XStart and XEnd
		XStart = 0;
		XEnd = 0;

		// Loop all scanlines from middle to bottom
		for (int y = y1; y <= y2; y++)
		{
			// XStart is the left X value (x1) + the difference between current Y in the loop and the left Y (y-y1)
			// multiplied by the left inverted slope
			XStart = x1 + ((y - y1)*InverseSlope1);
			// XEnd is the right X value (x2) + the difference between current Y in the loop and the right Y (y-y2)
			// multiplied by the right inverted slope
			XEnd = x2 + ((y - y2)*InverseSlope2);

			// Ensue that XStart is always to the left of XEnd (this could be inverted because of the vertices order of some faces)
			if (XStart > XEnd)
			{
				Integer_Swap(&XStart, &XEnd);
			}

			// For each looped row, draw a texture pixel in every column, from XStart to XEnd
			for (int x = XStart; x < XEnd; x++)
			{
				// Draw a pixel with the especified solid color and drawing method (line drawing algorithm)
				Draw_Triangle_Pixel(x, y, VertexA, VertexB, VertexC, Color);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Draw texture pixel (texel) at position (X,Y) using interpolation
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Draw_Texel(
	int x, int y,
	vec4_t VertexA, vec4_t VertexB, vec4_t VertexC, // If you don't want perspective correct interpolation, use a vec2 (just x and y)
	tex2_t AUV, tex2_t BUV, tex2_t CUV,
	upng_t* Texture
	)
{
	// The vertex P of the barycentric coordinates is going to be the current pixel in position (X,Y) of the triangle
	vec2_t CurrentPixel = { x, y };
	vec2_t VertA = Vec4_To_Vec2(VertexA);
	vec2_t VertB = Vec4_To_Vec2(VertexB);
	vec2_t VertC = Vec4_To_Vec2(VertexC);

	vec3_t BarycentricWeights = Barycentric_Weights(VertA, VertB, VertC, CurrentPixel);
	// Extract the weights alpha, beta and gamma from the resulting barycentric coordinates weights calculation
	float Alpha = BarycentricWeights.x;
	float Beta = BarycentricWeights.y;
	float Gamma = BarycentricWeights.z;

	// Variables to store the interpolated values of U, V and reciprocal W (1/W) for the current pixel
	// This for correcting the perspective interpolation by using a linear equation accounting the depth of each vertex 
	// We use the reciprocal W because the regular one doesn't give us a linear equation, sp we can't linear interpolate it
	float InterpolatedU;
	float InterpolatedV;
	// float InterpolatedReciprocalW; // reciprocal of W = 1/W

	/*
		// Less Performant way (too many divisions):

		// Perform the interpolation of all U/W , V/W values using barycentric weights and a factor of 1/W
		InterpolatedU = (u0 / VertexA.w) * Alpha + (u1 / VertexB.w) * Beta + (u2 / VertexC.w) * Gamma;
		InterpolatedV = (v0 / VertexA.w) * Alpha + (v1 / VertexB.w) * Beta + (v2 / VertexC.w) * Gamma;

		// Perform the interpolation of 1/W for the current pixel
		InterpolatedReciprocalW = (1 / VertexA.w) * Alpha + (1 / VertexB.w) * Beta + (1 / VertexC.w) * Gamma;

		// Divide the interpolated U and V by the interpolated 1/W (undo the perspective transform)
		InterpolatedU /= InterpolatedReciprocalW;
		InterpolatedV /= InterpolatedReciprocalW;

	*/

	// More Performant way (just 1 division for U's and V's)...
	// ...this can be achieved by doing algebra and simplifications with the established operations

	// Perform the interpolation of all U/W , V/W values using barycentric weights and a factor of 1/W
	// Perform the interpolation of 1/W for the current pixel
	// Divide the interpolated U and V by the interpolated 1/W (undo the perspective transform)
	float Simplified0 = Alpha * VertexB.w * VertexC.w;
	float Simplified1 = Beta * VertexA.w * VertexC.w;
	float Simplified2 = Gamma * VertexA.w * VertexB.w;
	float SimplifiedAddition = Simplified0 + Simplified1 + Simplified2;
	float SimplifiedDivision = 1 / SimplifiedAddition;
	InterpolatedU = ( (AUV.u * Simplified0) + (BUV.u*Simplified1) + (CUV.u*Simplified2) ) * SimplifiedDivision;
	InterpolatedV = ((AUV.v * Simplified0) + (BUV.v*Simplified1) + (CUV.v*Simplified2)) * SimplifiedDivision;

	// Get the current mesh's texture width and height dimensions
	int TextureWidth = upng_get_width(Texture);
	int TextureHeight = upng_get_height(Texture);

	// Map the UV coordinate to the full texture width and height
	// At the end apply a modulus (%) operation to truncate the result so it never goes beyond TextureWidth/Height
	// This will avoid crashes (overflowing the texture buffer) maintaining the values between 0 and width/height
	int TextureX = abs( (int)(InterpolatedU * TextureWidth) ) % TextureWidth;
	int TextureY = abs( (int)(InterpolatedV * TextureHeight) ) % TextureHeight;

	// Only draw the current pixel ih its depth value is less than what was already previously stored in the ZBuffer
	// Basically, if the current pixel that is going to be drawn is in front of was already there
	float WMultiplication = VertexA.w * VertexB.w * VertexC.w;
	float ReciprocalW = SimplifiedAddition / WMultiplication;
	// Adjust the reciprocal W so the pixels that are closer to the camera have smaller values
	ReciprocalW = 1.0 - ReciprocalW;	
	// Make sure the current pixel index is inside the screen! (that's achived with the % operation)
	//int PixelIndex = ((Get_Window_Width() * y) + x)%(Get_Window_Width() * Get_Window_Height()); // (windowWith * current row) + current column
	//if ( ReciprocalW < ZBuffer[PixelIndex])
	if(ReciprocalW < Get_ZBuffer_At(x, y))
	{
		// Get the buffer of colors from the current mesh's texture
		color_t* TextureBuffer = (color_t*)upng_get_buffer(Texture);

		// Draw a pixel at position X,Y with the color that comes from the mapped texture
		int TextureIndex = ((TextureWidth * TextureY) + TextureX) % (Get_Window_Width() * Get_Window_Height());
		Draw_Pixel(x, y, TextureBuffer[TextureIndex]);

		// Update the Z/Depth Buffer value with the calculated reciprocal W (1/W) of this current pixel
		// ZBuffer[PixelIndex] = InterpolatedReciprocalW; // This is with the "less performant" way
		//ZBuffer[PixelIndex] = ReciprocalW;
		Update_ZBuffer_At(x, y, ReciprocalW);
	}	
}

///////////////////////////////////////////////////////////////////////////////
// Draw a textured triangle with the flat-top/flat-bottom method
// We split the original triangle in 2, half flat-bottom and half flat-top
///////////////////////////////////////////////////////////////////////////////
/*
			  (u0,v0) A
				/   \
			   /     \
			  /       \
			 /         \
			/           \
	 B (u1,v1)-----------\
		   \_             \
			  \_           \
				 \ _        \
					 \_      \
						\     \
						  \_   \
							 \_ \
								\
							 (u2,v2) C

*/
///////////////////////////////////////////////////////////////////////////////

// If you don't want perspective correct interpolation, you don't need to pass the Z and W components
void Draw_Textured_Triangle(
	int x0, int y0, float z0, float w0,
	int x1, int y1, float z1, float w1,
	int x2, int y2, float z2, float w2,
	float u0, float v0, float u1, float v1, float u2, float v2,
	upng_t* texture
	)
{
	// Loop all the pixels of the triangle based on the texture's color
	
	// We need to sort the vertices by Y-coordinate ascending (y0 < y1 < y2)

	if (y0 > y1) // If y0 is greater than y1, swap their values (X and Y)
	{
		Integer_Swap(&y0, &y1);
		Integer_Swap(&x0, &x1);
		Float_Swap(&z0, &z1); // If you don't want perspective correct interpolation, you don't need to pass the Z and W components
		Float_Swap(&w0, &w1);

		Float_Swap(&v0, &v1);
		Float_Swap(&u0, &u1);
	}
	if (y1 > y2) // If y1 is greater than y2, swap their values (X and Y)
	{
		Integer_Swap(&y1, &y2);
		Integer_Swap(&x1, &x2);
		Float_Swap(&z1, &z2);
		Float_Swap(&w1, &w2);

		Float_Swap(&v1, &v2);
		Float_Swap(&u1, &u2);
	}
	if (y0 > y1) // If after swapping y1 and y2, y0 is greater than y1, then swap their values(X and Y)
	{
		Integer_Swap(&y0, &y1);
		Integer_Swap(&x0, &x1);
		Float_Swap(&z0, &z1);
		Float_Swap(&w0, &w1);

		Float_Swap(&v0, &v1);
		Float_Swap(&u0, &u1);
	}

	// Flip the V component to account for innverted V-Coordinates (V is growing downwards by default)
	v0 = 1.0 - v0;
	v1 = 1.0 - v1;
	v2 = 1.0 - v2;

	// Create vectors containing the sorted vertices and texture coords with the sorted UVs
	vec4_t VertexA = {x0, y0, z0, w0 }; // If you don't want perspective correct interpolation, use a vec2 (just x and y)
	vec4_t VertexB = { x1, y1, z1, w1 };
	vec4_t VertexC = { x2, y2, z2, w2 };

	tex2_t AUV = {u0, v0};
	tex2_t BUV = { u1, v1 };
	tex2_t CUV = { u2, v2 };

	/////////////////////  DRAW THE UPPER PART OF THE TRIANGLE (FLAT-BOTTOM) ///////////////////////////////////

	// Calculate the 2 slopes from each triangle "leg"
	// Because Y is always going to change 1 by 1, then we calculate the inverse of the slope
	// So, instead of doing slope = Dy/Dx as usual, we do slope = Dx/Dy
	// Meaning we're searching the change in X per Y, not Y per X
	float DY1 = abs(y1-y0);
	float DY2 = abs(y2 - y0);
	float InverseSlope1 = 0;
	float InverseSlope2 = 0;

	// Initialize XStart and XEnd
	int XStart = 0;
	int XEnd = 0;

	if (DY1 != 0 && DY2 != 0)
	{
		InverseSlope1 = (float)(x1 - x0) / DY1;
		InverseSlope2 = (float)(x2 - x0) / DY2;

		// Loop all scanlines from top to middle
		for (int y = y0; y <= y1; y++)
		{
			// XStart is the left X value (x1) + the difference between current Y in the loop and the left Y (y-y1)
				// multiplied by the left inverted slope
			XStart = x1 + ((y - y1)*InverseSlope1);
			// XEnd is the right X value (x2) + the difference between current Y in the loop and the right Y (y-y2)
			// multiplied by the right inverted slope
			XEnd = x2 + ((y - y2)*InverseSlope2);

			// Ensure that XStart is always to the left of XEnd (this could be inverted because of the vertices order of some faces)
			if (XStart > XEnd)
			{
				Integer_Swap(&XStart, &XEnd);
			}

			// For each looped row, draw a texture pixel in every column, from XStart to XEnd
			for (int x = XStart; x < XEnd; x++)
			{
				//Draw_Pixel(x, y, 0xFFFF00FF); // For testing/debugging
				
				// Draw a pixel with the color that comes from the texture
				Draw_Texel(x, y, VertexA, VertexB, VertexC, AUV, BUV, CUV, texture);
			}
		}
	}

	/////////////////////  DRAW THE Bottom PART OF THE TRIANGLE (FLAT-TOP) ///////////////////////////////////

	// Calculate the inverted slope of each leg, now for the bottom half of the triangle
	DY1 = abs(y2-y1);
	DY2 = abs(y2 - y0);
	if (DY1 != 0 && DY2 != 0)
	{
		InverseSlope1 = (float)(x2 - x1) / DY1;
		InverseSlope2 = (float)(x2 - x0) / DY2;

		// Initialize XStart and XEnd
		XStart = 0;
		XEnd = 0;

		// Loop all scanlines from middle to bottom
		for (int y = y1; y <= y2; y++)
		{
			// XStart is the left X value (x1) + the difference between current Y in the loop and the left Y (y-y1)
			// multiplied by the left inverted slope
			XStart = x1 + ((y - y1)*InverseSlope1);
			// XEnd is the right X value (x2) + the difference between current Y in the loop and the right Y (y-y2)
			// multiplied by the right inverted slope
			XEnd = x2 + ((y - y2)*InverseSlope2);

			// Ensue that XStart is always to the left of XEnd (this could be inverted because of the vertices order of some faces)
			if (XStart > XEnd)
			{
				Integer_Swap(&XStart, &XEnd);
			}

			// For each looped row, draw a texture pixel in every column, from XStart to XEnd
			for (int x = XStart; x < XEnd; x++)
			{
				//Draw_Pixel(x, y, 0xFFFF00FF); // For testing/debugging
				// cool Comic/Manga like screentone effect by coloring black every even-value pixels
				//Draw_Pixel(x, y, (x % 2 == 0) ? 0xFFFF00FF : 0xFF000000);
				
				// Draw a pixel with the color that comes from the texture
				Draw_Texel(x, y, VertexA, VertexB, VertexC, AUV, BUV, CUV, texture);
			}
		}
	}
}

vec3_t Get_Triangle_Normal(vec4_t* TriangleVertices)
{
	vec3_t VectorA = Vec4_To_Vec3(TriangleVertices[0]);  /*   A	    */
	vec3_t VectorB = Vec4_To_Vec3(TriangleVertices[1]); /*  /   \   */
	vec3_t VectorC = Vec4_To_Vec3(TriangleVertices[2]); /* C-----B  */

	// Get vector subtraction of B-A and C-A
	vec3_t SubBA = Vec3_Subtract(VectorB, VectorA);
	vec3_t SubCA = Vec3_Subtract(VectorC, VectorA);
	Vec3_Normalize(&SubBA);
	Vec3_Normalize(&SubCA);

	// Get the normal vector of the current face by calculating the cross product between subtracted B-A and C-A
	// Specifically that order since we're using a left hand coordinate system
	vec3_t FaceNormal = Vec3_Cross(SubBA, SubCA);
	Vec3_Normalize(&FaceNormal);
	return FaceNormal;
}
