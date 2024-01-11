#include "Light.h"

static light_t SunLight =
{
	//.LightDirection = { 1, 0.5, 0.5}
	.LightDirection = { 0, 0, 1}
};

void Set_SunLight(vec3_t Direction)
{
	Vec3_Normalize(&Direction);
	SunLight.LightDirection = Direction;
}

light_t Get_SunLight(void)
{
	return SunLight;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Change triangle face color based on a percentage factor to represent light intensity
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

color_t Light_Apply_Intensity(color_t FaceColor, float PercentageFactor)
{
	if (PercentageFactor < 0)
	{
		PercentageFactor = 0;
	}
	if (PercentageFactor > 1)
	{
		PercentageFactor = 1;
	}

	color_t Alpha = (FaceColor & 0xFF000000);
	color_t Red = (FaceColor & 0x00FF0000) * PercentageFactor;
	color_t Green = (FaceColor & 0x0000FF00) * PercentageFactor;
	color_t Blue = (FaceColor & 0x000000FF) * PercentageFactor;

	color_t ResultColor = Alpha | (Red & 0x00FF0000) | (Green & 0x0000FF00) | (Blue & 0x000000FF);
	return ResultColor;
}