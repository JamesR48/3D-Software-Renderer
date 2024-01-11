#include <math.h>
#include "Matrix.h"

mat4_t Mat4_Identity(void)
{
	/*
		| 1 0 0 0 |
		| 0 1 0 0 |
		| 0 0 1 0 |
		| 0 0 0 1 |
	*/

	mat4_t IdentityMat = 
	{
		{
			{1, 0, 0, 0},
			{0, 1, 0, 0},
			{0, 0, 1, 0},
			{0, 0, 0, 1},
		}
	};

	return IdentityMat;
}

mat4_t Mat4_MakeRotationX(float Angle)
{
	// Counter-clockwise X rotation
	/*
		|  1       0          0    0 |
		|  0  Cos(Ang)  -Sin(Ang)  0 |
		|  0  Sin(Ang)   Cos(Ang)  0 |
		|  0       0          0    1 |
	*/

	float Cos = cos(Angle);
	float Sin = sin(Angle);

	mat4_t Mat = Mat4_Identity();

	Mat.matrix[1][1] = Cos;
	Mat.matrix[1][2] = -Sin;
	Mat.matrix[2][1] = Sin;
	Mat.matrix[2][2] = Cos;

	return Mat;
}

mat4_t Mat4_MakeRotationY(float Angle)
{
	// Clockwise Y rotation
	/*
		|   Cos(Ang)  0  Sin(Ang)  0 |
		|        0    1       0    0 |
		|  -Sin(Ang)  0  Cos(Ang)  0 |
		|        0    0       0    1 |
	*/

	float Cos = cos(Angle);
	float Sin = sin(Angle);

	mat4_t Mat = Mat4_Identity();

	Mat.matrix[0][0] = Cos;
	Mat.matrix[0][2] = Sin;
	Mat.matrix[2][0] = -	Sin;
	Mat.matrix[2][2] = Cos;

	return Mat;
}

mat4_t Mat4_MakeRotationZ(float Angle)
{
	// Counter-clockwise Z rotation
	/*
		|  Cos(Ang)  -Sin(Ang)  0  0 |
		|  Sin(Ang)   Cos(Ang)  0  0 |
		|       0          0    1  0 |
		|       0          0    0  1 |
	*/

	float Cos = cos(Angle);
	float Sin = sin(Angle);

	mat4_t Mat = Mat4_Identity();

	Mat.matrix[0][0] = Cos;
	Mat.matrix[0][1] = -Sin;
	Mat.matrix[1][0] = Sin;
	Mat.matrix[1][1] = Cos;

	return Mat;
}

mat4_t Mat4_MakeScale(float Sx, float Sy, float Sz)
{
	/*
		| Sx   0   0  0 |
		|  0  Sy   0  0 |
		|  0   0  Sz  0 |
		|  0   0   0  1 |
	*/

	mat4_t Mat = Mat4_Identity();

	Mat.matrix[0][0] = Sx;
	Mat.matrix[1][1] = Sy;
	Mat.matrix[2][2] = Sz;

	return Mat;
}

mat4_t Mat4_MakeTranslation(float Tx, float Ty, float Tz)
{
	/*
		|  1   0   0  Tx |
		|  0   1   0  Ty |
		|  0   0   1  Tz |
		|  0   0   0   1 |
	*/

	mat4_t Mat = Mat4_Identity();

	Mat.matrix[0][3] = Tx;
	Mat.matrix[1][3] = Ty;
	Mat.matrix[2][3] = Tz;

	return Mat;
}

mat4_t Mat4_Multiply_Mat4(mat4_t MatA, mat4_t MatB)
{
	// MatrixB (the one at the right) is the matriz that we want to transform

	mat4_t Result;

	for(int Row = 0; Row < 4; Row++)
	{
		for (int Col = 0; Col < 4; Col++)
		{
			Result.matrix[Row][Col] = 
				(MatA.matrix[Row][0]* MatB.matrix[0][Col]) + (MatA.matrix[Row][1] * MatB.matrix[1][Col]) +
				(MatA.matrix[Row][2] * MatB.matrix[2][Col]) + (MatA.matrix[Row][3] * MatB.matrix[3][Col]);
		}
	}

	return Result;
}

vec4_t Mat4_Multiply_Vec4(mat4_t Mat, vec4_t Vec)
{
	vec4_t Result;
	// each matrix's row multiplied by the vector
	Result.x = (Mat.matrix[0][0] * Vec.x) + (Mat.matrix[0][1] * Vec.y) + (Mat.matrix[0][2] * Vec.z) + (Mat.matrix[0][3] * Vec.w);
	Result.y = (Mat.matrix[1][0] * Vec.x) + (Mat.matrix[1][1] * Vec.y) + (Mat.matrix[1][2] * Vec.z) + (Mat.matrix[1][3] * Vec.w);
	Result.z = (Mat.matrix[2][0] * Vec.x) + (Mat.matrix[2][1] * Vec.y) + (Mat.matrix[2][2] * Vec.z) + (Mat.matrix[2][3] * Vec.w);
	Result.w = (Mat.matrix[3][0] * Vec.x) + (Mat.matrix[3][1] * Vec.y) + (Mat.matrix[3][2] * Vec.z) + (Mat.matrix[3	][3] * Vec.w);

	return Result;
}

mat4_t Mat4_MakeWorldMatrix(mat4_t Scale, mat4_t RotX, mat4_t RotY, mat4_t RotZ, mat4_t Translation)
{
	mat4_t World = Mat4_Identity();

	// The matrix at the right is the one that we want to transform
	World = Mat4_Multiply_Mat4(Scale, World);
	World = Mat4_Multiply_Mat4(RotX, World);
	World = Mat4_Multiply_Mat4(RotY, World);
	World = Mat4_Multiply_Mat4(RotZ, World);
	World = Mat4_Multiply_Mat4(Translation, World);

	return World;
}

mat4_t Mat4_Make_PerspectiveProjection(float FOV, float AspectRatio, float ZNear, float ZFar)
{
	// AspectRatio = WindowHeight / WindowWidth

	/*
		|  (AspectRatio)*(1/tan(FOV/2))              0                   0                             0 |
		|                             0   1/tan(FOV/2)                   0                             0 |
		|                             0              0   ZFar/(ZFar-ZNear)  -((ZFar*ZNear)/(ZFar-ZNear)) |
		|                             0              0                   1                             0 |
	*/

	mat4_t PerspectiveProjection = { { { 0 } } };
	PerspectiveProjection.matrix[0][0] = (AspectRatio) * ( 1 / tan(FOV / 2) );
	PerspectiveProjection.matrix[1][1] = (1 / tan(FOV / 2));
	PerspectiveProjection.matrix[2][2] = ZFar / (ZFar - ZNear);
	PerspectiveProjection.matrix[2][3] = -( (ZFar*ZNear) / (ZFar - ZNear) );
	PerspectiveProjection.matrix[3][2] = 1;

	return PerspectiveProjection;
}

vec4_t Mat4_Multiply_Vec4_Projection(mat4_t ProjectionMatrix, vec4_t Vector)
{
	// Multiply the projection matrix by the original vector
	vec4_t Result = Mat4_Multiply_Vec4(ProjectionMatrix, Vector);

	// Perform perspective divide with the original Z value stored in W
	if (Result.w != 0)
	{
		Result.x /= Result.w;
		Result.y /= Result.w;
		Result.z /= Result.w;
	}

	return Result;
}

// Compute the Look At function
mat4_t Mat4_Look_At(vec3_t CameraEye, vec3_t LookTarget, vec3_t UpVector)
{
	// The CameraEye is basically the camera position in the world
	// The UpVector passed is the world's up vector (0,1,0), which locks the camera rotation
	vec3_t ZForward = Vec3_Subtract(LookTarget, CameraEye); // Camera's Forward (Z) vector
	Vec3_Normalize(&ZForward);

	vec3_t XRight = Vec3_Cross(UpVector, ZForward); // Camera's Right (X) vector
	Vec3_Normalize(&XRight);

	vec3_t YUp = Vec3_Cross(ZForward, XRight); // Camera's Up (Y) vector

	/*
		|   right.x     right.y     right.z    -dot(right, eye) |
		|      up.x        up.y        up.z       -dot(up, eye) |
		| forward.x   forward.y   forward.z  -dot(forward, eye) |
		|         0           0           0                   1 |
	*/

	mat4_t ViewMatrix =
	{ {
		{  XRight.x,   XRight.y,   XRight.z,   -Vec3_Dot(XRight, CameraEye)},
		{     YUp.x,      YUp.y,      YUp.z,      -Vec3_Dot(YUp, CameraEye)},
		{ZForward.x, ZForward.y, ZForward.z, -Vec3_Dot(ZForward, CameraEye)},
		{		  0,			  0,			  0,								  1},
	} };

	return ViewMatrix;
}
