#pragma once

#include"Vsh.h"


class VectorTransform
{
public:
	

	static Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m);

	static Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);
private:

};

