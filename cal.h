#pragma once
#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>

#pragma region Move(translate)
// move
static Matrix4x4 MakeTranslateMatrix(const Vector3& translate) {
	Matrix4x4 result{};
	result.m[3][0] = translate.x;
	result.m[3][1] = translate.y;
	result.m[3][2] = translate.z;
	result.m[0][0] = 1.0f;
	result.m[1][1] = 1.0f;
	result.m[2][2] = 1.0f;
	result.m[3][3] = 1.0f;
	return result;
};
#pragma endregion

#pragma region zoom in out(scale)
// zoom in out
static Matrix4x4 MakeScaleMatrix(const Vector3& scale) {
	Matrix4x4 result{};
	result.m[0][0] = scale.x;
	result.m[1][1] = scale.y;
	result.m[2][2] = scale.z;
	result.m[3][3] = 1.0f;
	return result;
};
#pragma endregion

#pragma region transform
static Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix) {
	Vector3 result{};
	result.x = matrix.m[0][0] * vector.x + matrix.m[1][0] * vector.y + matrix.m[2][0] * vector.z + matrix.m[3][0] * 1.0f;
	result.y = matrix.m[0][1] * vector.x + matrix.m[1][1] * vector.y + matrix.m[2][1] * vector.z + matrix.m[3][1] * 1.0f;
	result.z = matrix.m[0][2] * vector.x + matrix.m[1][2] * vector.y + matrix.m[2][2] * vector.z + matrix.m[3][2] * 1.0f;

	float w = matrix.m[0][3] * vector.x + matrix.m[1][3] * vector.y + matrix.m[2][3] * vector.z + matrix.m[3][3] * 1.0f;

	if (w != 1.0f && w != 0.0f) {
		result.x /= w;
		result.y /= w;
		result.z /= w;
	}
	return result;
};
#pragma endregion

#pragma region Inverse
/// Inverse
static Matrix4x4 Inverse(const Matrix4x4& m) {
	Matrix4x4 result{};
	float determinant = m.m[0][0] * (m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[2][1] * m.m[3][2] * m.m[1][3] + m.m[3][1] * m.m[1][2] * m.m[2][3] - m.m[3][1] * m.m[2][2] * m.m[1][3] -
	                                 m.m[2][1] * m.m[1][2] * m.m[3][3] - m.m[1][1] * m.m[3][2] * m.m[2][3]) -
	                    m.m[0][1] * (m.m[1][0] * m.m[2][2] * m.m[3][3] + m.m[2][0] * m.m[3][2] * m.m[1][3] + m.m[3][0] * m.m[1][2] * m.m[2][3] - m.m[3][0] * m.m[2][2] * m.m[1][3] -
	                                 m.m[2][0] * m.m[1][2] * m.m[3][3] - m.m[1][0] * m.m[3][2] * m.m[2][3]) +
	                    m.m[0][2] * (m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[2][0] * m.m[3][1] * m.m[1][3] + m.m[3][0] * m.m[1][1] * m.m[2][3] - m.m[3][0] * m.m[2][1] * m.m[1][3] -
	                                 m.m[2][0] * m.m[1][1] * m.m[3][3] - m.m[1][0] * m.m[3][1] * m.m[2][3]) -
	                    m.m[0][3] * (m.m[1][0] * m.m[2][1] * m.m[3][2] + m.m[2][0] * m.m[3][1] * m.m[1][2] + m.m[3][0] * m.m[1][1] * m.m[2][2] - m.m[3][0] * m.m[2][1] * m.m[1][2] -
	                                 m.m[2][0] * m.m[1][1] * m.m[3][2] - m.m[1][0] * m.m[3][1] * m.m[2][2]);

	if (determinant != 0) {
		result.m[0][0] = (m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[2][1] * m.m[3][2] * m.m[1][3] + m.m[3][1] * m.m[1][2] * m.m[2][3] - m.m[3][1] * m.m[2][2] * m.m[1][3] -
		                  m.m[2][1] * m.m[1][2] * m.m[3][3] - m.m[1][1] * m.m[3][2] * m.m[2][3]) /
		                 determinant;

		result.m[0][1] = -(m.m[0][1] * m.m[2][2] * m.m[3][3] + m.m[2][1] * m.m[3][2] * m.m[0][3] + m.m[3][1] * m.m[0][2] * m.m[2][3] - m.m[3][1] * m.m[2][2] * m.m[0][3] -
		                   m.m[2][1] * m.m[0][2] * m.m[3][3] - m.m[0][1] * m.m[3][2] * m.m[2][3]) /
		                 determinant;

		result.m[0][2] = (m.m[0][1] * m.m[1][2] * m.m[3][3] + m.m[1][1] * m.m[3][2] * m.m[0][3] + m.m[3][1] * m.m[0][2] * m.m[1][3] - m.m[3][1] * m.m[1][2] * m.m[0][3] -
		                  m.m[1][1] * m.m[0][2] * m.m[3][3] - m.m[0][1] * m.m[3][2] * m.m[1][3]) /
		                 determinant;

		result.m[0][3] = -(m.m[0][1] * m.m[1][2] * m.m[2][3] + m.m[1][1] * m.m[2][2] * m.m[0][3] + m.m[2][1] * m.m[0][2] * m.m[1][3] - m.m[2][1] * m.m[1][2] * m.m[0][3] -
		                   m.m[1][1] * m.m[0][2] * m.m[2][3] - m.m[0][1] * m.m[2][2] * m.m[1][3]) /
		                 determinant;

		result.m[1][0] = -(m.m[1][0] * m.m[2][2] * m.m[3][3] + m.m[2][0] * m.m[3][2] * m.m[1][3] + m.m[3][0] * m.m[1][2] * m.m[2][3] - m.m[3][0] * m.m[2][2] * m.m[1][3] -
		                   m.m[2][0] * m.m[1][2] * m.m[3][3] - m.m[1][0] * m.m[3][2] * m.m[2][3]) /
		                 determinant;

		result.m[1][1] = (m.m[0][0] * m.m[2][2] * m.m[3][3] + m.m[2][0] * m.m[3][2] * m.m[0][3] + m.m[3][0] * m.m[0][2] * m.m[2][3] - m.m[3][0] * m.m[2][2] * m.m[0][3] -
		                  m.m[2][0] * m.m[0][2] * m.m[3][3] - m.m[0][0] * m.m[3][2] * m.m[2][3]) /
		                 determinant;

		result.m[1][2] = -(m.m[0][0] * m.m[1][2] * m.m[3][3] + m.m[1][0] * m.m[3][2] * m.m[0][3] + m.m[3][0] * m.m[0][2] * m.m[1][3] - m.m[3][0] * m.m[1][2] * m.m[0][3] -
		                   m.m[1][0] * m.m[0][2] * m.m[3][3] - m.m[0][0] * m.m[3][2] * m.m[1][3]) /
		                 determinant;

		result.m[1][3] = (m.m[0][0] * m.m[1][2] * m.m[2][3] + m.m[1][0] * m.m[2][2] * m.m[0][3] + m.m[2][0] * m.m[0][2] * m.m[1][3] - m.m[2][0] * m.m[1][2] * m.m[0][3] -
		                  m.m[1][0] * m.m[0][2] * m.m[2][3] - m.m[0][0] * m.m[2][2] * m.m[1][3]) /
		                 determinant;

		result.m[2][0] = (m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[2][0] * m.m[3][1] * m.m[1][3] + m.m[3][0] * m.m[1][1] * m.m[2][3] - m.m[3][0] * m.m[2][1] * m.m[1][3] -
		                  m.m[2][0] * m.m[1][1] * m.m[3][3] - m.m[1][0] * m.m[3][1] * m.m[2][3]) /
		                 determinant;

		result.m[2][1] = -(m.m[0][0] * m.m[2][1] * m.m[3][3] + m.m[2][0] * m.m[3][1] * m.m[0][3] + m.m[3][0] * m.m[0][1] * m.m[2][3] - m.m[3][0] * m.m[2][1] * m.m[0][3] -
		                   m.m[2][0] * m.m[0][1] * m.m[3][3] - m.m[0][0] * m.m[3][1] * m.m[2][3]) /
		                 determinant;

		result.m[2][2] = (m.m[0][0] * m.m[1][1] * m.m[3][3] + m.m[1][0] * m.m[3][1] * m.m[0][3] + m.m[3][0] * m.m[0][1] * m.m[1][3] - m.m[3][0] * m.m[1][1] * m.m[0][3] -
		                  m.m[1][0] * m.m[0][1] * m.m[3][3] - m.m[0][0] * m.m[3][1] * m.m[1][3]) /
		                 determinant;

		result.m[2][3] = -(m.m[0][0] * m.m[1][1] * m.m[2][3] + m.m[1][0] * m.m[2][1] * m.m[0][3] + m.m[2][0] * m.m[0][1] * m.m[1][3] - m.m[2][0] * m.m[1][1] * m.m[0][3] -
		                   m.m[1][0] * m.m[0][1] * m.m[2][3] - m.m[0][0] * m.m[2][1] * m.m[1][3]) /
		                 determinant;

		result.m[3][0] = -(m.m[1][0] * m.m[2][1] * m.m[3][2] + m.m[2][0] * m.m[3][1] * m.m[1][2] + m.m[3][0] * m.m[1][1] * m.m[2][2] - m.m[3][0] * m.m[2][1] * m.m[1][2] -
		                   m.m[2][0] * m.m[1][1] * m.m[3][2] - m.m[1][0] * m.m[3][1] * m.m[2][2]) /
		                 determinant;

		result.m[3][1] = (m.m[0][0] * m.m[2][1] * m.m[3][2] + m.m[2][0] * m.m[3][1] * m.m[0][2] + m.m[3][0] * m.m[0][1] * m.m[2][2] - m.m[3][0] * m.m[2][1] * m.m[0][2] -
		                  m.m[2][0] * m.m[0][1] * m.m[3][2] - m.m[0][0] * m.m[3][1] * m.m[2][2]) /
		                 determinant;

		result.m[3][2] = -(m.m[0][0] * m.m[1][1] * m.m[3][2] + m.m[1][0] * m.m[3][1] * m.m[0][2] + m.m[3][0] * m.m[0][1] * m.m[1][2] - m.m[3][0] * m.m[1][1] * m.m[0][2] -
		                   m.m[1][0] * m.m[0][1] * m.m[3][2] - m.m[0][0] * m.m[3][1] * m.m[1][2]) /
		                 determinant;

		result.m[3][3] = (m.m[0][0] * m.m[1][1] * m.m[2][2] + m.m[1][0] * m.m[2][1] * m.m[0][2] + m.m[2][0] * m.m[0][1] * m.m[1][2] - m.m[2][0] * m.m[1][1] * m.m[0][2] -
		                  m.m[1][0] * m.m[0][1] * m.m[2][2] - m.m[0][0] * m.m[2][1] * m.m[1][2]) /
		                 determinant;
	}

	return result;
};
#pragma endregion

#pragma region rotate(XYZ)
// X axis
static Matrix4x4 MakeRotateXMatrix(float radian) {
	Matrix4x4 result{};

	result.m[0][0] = 1.0f;
	result.m[3][3] = 1.0f;

	result.m[1][1] = std::cos(radian);
	result.m[2][1] = std::sin(radian) * -1;
	result.m[1][2] = std::sin(radian);
	result.m[2][2] = std::cos(radian);

	return result;
};

// Y axis
static Matrix4x4 MakeRotateYMatrix(float radian) {
	Matrix4x4 result{};

	result.m[1][1] = 1.0f;
	result.m[3][3] = 1.0f;

	result.m[0][0] = std::cos(radian);
	result.m[2][0] = std::sin(radian);
	result.m[0][2] = std::sin(radian) * -1;
	result.m[2][2] = std::cos(radian);

	return result;
};

// Z axis
static Matrix4x4 MakeRotateZMatrix(float radian) {
	Matrix4x4 result{};

	result.m[2][2] = 1.0f;
	result.m[3][3] = 1.0f;

	result.m[0][0] = std::cos(radian);
	result.m[1][0] = std::sin(radian) * -1;
	result.m[0][1] = std::sin(radian);
	result.m[1][1] = std::cos(radian);

	return result;
};

#pragma endregion

#pragma region Multiply
/// Multiply
static Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result{};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.m[i][j] = 0;
			for (int k = 0; k < 4; k++) {
				result.m[i][j] += m1.m[i][k] * m2.m[k][j];
			}
		}
	}
	return result;
};
#pragma endregion

#pragma region MakeAffineMatrix
static Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	Matrix4x4 result{};
	Matrix4x4 scaleMatrix = MakeScaleMatrix(scale);
	Matrix4x4 rotateXMatrix = MakeRotateXMatrix(rotate.x);
	Matrix4x4 rotateYMatrix = MakeRotateYMatrix(rotate.y);
	Matrix4x4 rotateZMatrix = MakeRotateZMatrix(rotate.z);
	Matrix4x4 translateMatrix = MakeTranslateMatrix(translate);

	Matrix4x4 rotateMatrix = Multiply(rotateXMatrix, Multiply(rotateYMatrix, rotateZMatrix));

	result = Multiply(scaleMatrix, Multiply(rotateMatrix, translateMatrix));
	return result;
};
#pragma endregion

#pragma region Cam

// MakePerspectiveFovMatrix
static Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
	Matrix4x4 result{};

	result.m[0][0] = ((1 / aspectRatio) * (1.0f / (tan(fovY / 2))));
	result.m[1][1] = (1.0f / (tan(fovY / 2)));

	result.m[2][2] = (farClip / (farClip - nearClip));
	result.m[3][2] = ((-nearClip * farClip) / (farClip - nearClip));

	result.m[2][3] = 1.0f;

	return result;
};
// MakeOrthographicMatrix
static Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
	Matrix4x4 result = {};

	result.m[0][0] = 2.0f / (right - left);
	result.m[1][1] = 2.0f / (top - bottom);
	result.m[2][2] = -2.0f / (farClip - nearClip);
	result.m[3][3] = 1.0f;

	result.m[3][0] = -((right + left) / (right - left));
	result.m[3][1] = -((top + bottom) / (top - bottom));
	result.m[3][2] = nearClip / (farClip - nearClip);

	return result;
};

// MakeViewportMatrix
static Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {
	Matrix4x4 result = {};

	result.m[0][0] = width / 2.0f;
	result.m[3][0] = left + width / 2.0f;

	result.m[1][1] = -height / 2.0f;
	result.m[3][1] = top + height / 2.0f;

	result.m[2][2] = maxDepth - minDepth;
	result.m[3][2] = minDepth;

	result.m[3][3] = 1.0f;

	return result;
};

#pragma endregion

#pragma region Add2
static Vector3 Add(const Vector3& v1, const Vector3& v2) {
	return {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z}; 
}
#pragma endregion

#pragma region Add3
static Vector3 Add3(const Vector3& v1, const Vector3& v2, const Vector3& v3) {
	return {v1.x + v2.x + v3.x, v1.y + v2.y + v3.y, v1.z + v2.z + v3.z};
}
#pragma endregion

#pragma region Vector3 x float
static Vector3 MultiplyVF(const Vector3& v, float scalar) {
	return {v.x * scalar, v.y * scalar, v.z * scalar}; 
}
#pragma endregion