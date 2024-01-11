#include "Camera.h"

static camera_t Camera =
{
	.Position = {0,0,0}, // Starts at the origin
	.LookDirection = {0,0,1}, // Look forward (+Z axis)
	.Velocity = {0,0,0}, // Movement velocity
	.YawAngle = 0.0, // Rotation around Y axis
	.PitchAngle = 0.0 // Rotation around X axis
};

void Initialize_Camera(vec3_t Pos, vec3_t LookDir)
{
	Camera.Position = Pos;
	Camera.LookDirection = LookDir;
	Camera.Velocity = Vec3_New(0,0,0);
	Camera.YawAngle = 0.0;
	Camera.PitchAngle = 0.0;
}

void Set_Camera_Position(vec3_t position)
{
	Camera.Position = position;
}

vec3_t Get_Camera_Position(void)
{
	return Camera.Position;
}

void Set_Camera_LookDirection(vec3_t LookDir)
{
	Camera.LookDirection = LookDir;
}

vec3_t Get_Camera_LookDirection(void)
{
	return Camera.LookDirection;
}

void Set_Camera_Velocity(float Value)
{
	Camera.Velocity = Vec3_ScalarMultiply(Camera.LookDirection, Value);
}

vec3_t Get_Camera_Velocity(void)
{
	return Camera.Velocity;
}

float Get_Camera_YawAngle(void)
{
	return Camera.YawAngle;
}

float Get_Camera_PitchAngle(void)
{
	return Camera.PitchAngle;
}

void Rotate_Camera_Euler(vec3_t Rot)
{
	Camera.YawAngle += Rot.y;
	Camera.PitchAngle += Rot.x;
}

vec3_t Get_Camera_LookAt_Target(void)
{
	// Initialize the target looking at the positive z-axis
	vec3_t Target = { 0, 0, 1 };

	mat4_t CameraYawRot = Mat4_MakeRotationY(Camera.YawAngle);
	mat4_t CameraPitchRot = Mat4_MakeRotationX(Camera.PitchAngle);

	// Create camera rotation matrix based on yaw and pitch
	mat4_t camera_rotation = Mat4_Identity();
	camera_rotation = Mat4_Multiply_Mat4(CameraPitchRot, camera_rotation);
	camera_rotation = Mat4_Multiply_Mat4(CameraYawRot, camera_rotation);

	// Update camera direction based on the rotation
	vec4_t CameraLookDir = Mat4_Multiply_Vec4(camera_rotation, Vec3_To_Vec4(Target));
	Camera.LookDirection = Vec4_To_Vec3(CameraLookDir);

	// Offset the camera position in the direction where the camera is pointing at
	Target = Vec3_Add(Camera.Position, Camera.LookDirection);

	return Target;
}
