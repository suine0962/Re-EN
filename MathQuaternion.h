#pragma once
#include "Vsh.h"
#include<cmath>
#include <stdio.h>
#define _USE_MATH_DEFINES

struct Quaternion {
	float x;
	float y;
	float z;
	float w;
};

class MathQuaternion
{
public:

	static Quaternion QuaternionSubtract(const Quaternion& q1, const Quaternion& q2);
	static Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);
	static Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);
	static Matrix4x4 ScalarMultiply(float scalar, const Matrix4x4& m);
	static Matrix4x4 MakeRotateAxisAngle(const Vector3& axis, float angel);
	static Matrix4x4 DerectionToDerection(const Vector3& from, const Vector3& to);
	static Quaternion IdentityQuaternion();
	static Quaternion Conjugate(const Quaternion& quaternion);
	static Quaternion SlerpConjugate(const Quaternion& quaternion);
	static float Norm(const Quaternion& quaternion);
	static Quaternion NormalizeQuaternion(const Quaternion& quaternion);
	static Quaternion InverseQuaternion(const Quaternion& quaternion);
	static Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angl);
	static Vector3 RotateVector(const Vector3& vector, const Quaternion& quaternion);
	static void invertQuaternion(Quaternion* q);
	static Quaternion Slerp(Quaternion& q0, Quaternion& q1, float t);


};

