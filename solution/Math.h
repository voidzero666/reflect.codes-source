#pragma once
#include "Hooks.h"
#include <xmmintrin.h>

#define PI			3.14159265358979323846
#define DEG2RAD(x) ((float)(x) * (float)((float)(PI) / 180.0f))
#define RAD2DEG(x) ((float)(x) * (float)(180.0f / (float)(PI)))

namespace Math
{
	RECT GetViewport();
	__forceinline void AngleMatrix(const Vector &angles, matrix& matrix);
	void ConcatTransforms(const matrix& in1, const matrix& in2, matrix& out);
	__forceinline void MatrixMultiply(matrix& in1, const matrix& in2);
	__forceinline void RotateMatrix(matrix in[128], matrix out[128], float delta, Vector renderOrigin);
	__forceinline void VectorRotate(const float* in1, const matrix& in2, float* out);
	__forceinline void VectorRotate(const Vector& in1, const matrix& in2, Vector& out);
	__forceinline void VectorRotate(const Vector& in1, const Vector& in2, Vector& out);
	__forceinline const matrix& world_to_screen_matrix();
	__forceinline bool screen_transform(const Vector & in, Vector & out);
	bool WorldToScreen(const Vector & in, Vector & out);
	bool WorldToScreen2(Vector &in, Vector &out);
	float DotProduct(const Vector& a, const Vector& b);
	void VectorTransform(const Vector& in1, const matrix& in2, Vector& out);
	void AngleMatrix(const Vector &angles, matrix &matrix);
	void MatrixSetColumn(const Vector &in, int column, matrix &out);
	void AngleMatrix(const Vector & angles, const Vector & position, matrix & matrix);
	Vector normalize(Vector angs);
	float NormalizeYaw(float yaw);
	float AngleMod(float a);
	float ApproachAngle(float target, float value, float speed);
	bool IsNearEqual(float v1, float v2, float Tolerance);
	float AngleDiff(float destAngle, float srcAngle);
	VOID sinCos(float radians, PFLOAT sine, PFLOAT cosine);
	float DotProduct(const float *v1, const float *v2);
	void VectorTransform(const float *in1, const matrix& in2, float *out);
	void VectorTransform_Wrapper(const Vector& in1, const matrix &in2, Vector &out);
	void AngleVectors(const Vector& angles, Vector& forward);
	void AngleVectors(const Vector &angles, Vector *forward, Vector *right, Vector *up);
	void AngleVectors(const Vector &angles, Vector *forward);
	void AngleVectors(const Vector& angles, Vector& forward);
	void AngleVectors(const Vector angles, Vector& forward, Vector& right, Vector& up);
	void vector_i_transform(const Vector& in1, const matrix& in2, Vector& out);
	Vector vector_rotate(Vector& in1, matrix& in2);
	Vector vector_rotate(Vector& in1, Vector& in2);
	void vector_i_rotate(Vector in1, matrix in2, Vector& out);
	bool intersect_line_with_bb(Vector& start, Vector& end, Vector& min, Vector& max);
	float segment_to_segment(const Vector s1, const Vector s2, const Vector k1, const Vector k2);
	float GetFov(const Vector& viewAngle, const Vector& aimAngle);
	float DistanceScreen(Vector2D vDstPos, Vector2D vSrcPos);
	void SmoothAngles(Vector MyViewAngles, Vector AimAngles, Vector &OutAngles, float Smoothing);
	Vector CalculateAngle(Vector src, Vector dst);
	void SetMatrixOrigin(Vector pos, matrix& pMat);
	Vector GetMatrixOrigin(const matrix& src);
	void ClampAngles(Vector& angles);
	float RandomFloat(float a, float b);
	void AngleVector4(const Vector &angles, Vector& forward, Vector& right, Vector& up);
	void VectorAngles(const Vector &forward, const Vector &pseudoup, Vector &angles);
	void AngleVectorsDefault(Vector q, Vector& vec);
	void VectorTransform2(const Vector in1, matrix in2, Vector& out);
	void VectorAngles(const Vector& forward, Vector& angles);
	float DistanceToRay(const Vector &pos, const Vector &rayStart, const Vector &rayEnd, float *along = nullptr, Vector *pointOnRay = nullptr);
	float VectorNormalize(Vector& v);
	void VectorMA(const Vector& start, float scale, const Vector& direction, Vector& dest);
	float AngleNormalize(float angle);
}