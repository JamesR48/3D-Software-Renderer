#pragma once

#ifndef  CAMERA_H
#define CAMERA_H

#include "Vector.h"
#include "Matrix.h"

typedef struct
{
	vec3_t Position;
	vec3_t LookDirection;
	vec3_t Velocity; // Movement speed
	float YawAngle; // Rotation angle in the Y axis (in radians)
	float PitchAngle; // Rotation angle in the X axis (in radians)
} camera_t;

void Initialize_Camera(vec3_t Pos, vec3_t LookDir);

void Set_Camera_Position(vec3_t Pos);
vec3_t Get_Camera_Position(void);

void Set_Camera_LookDirection(vec3_t LookDir);
vec3_t Get_Camera_LookDirection(void);

void Set_Camera_Velocity(float Value);
vec3_t Get_Camera_Velocity(void);

float Get_Camera_YawAngle(void);
float Get_Camera_PitchAngle(void);

void Rotate_Camera_Euler(vec3_t Rot);

vec3_t Get_Camera_LookAt_Target(void);

#endif // ! CAMERA_H

