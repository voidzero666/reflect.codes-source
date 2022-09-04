#include "Math.h"

namespace Math {
	Vector normalize(Vector angs)
	{
		while (angs.y < -180.0f)
			angs.y += 360.0f;
		while (angs.y > 180.0f)
			angs.y -= 360.0f;
		if (angs.x > 89.0f)
			angs.x = 89.0f;
		if (angs.x < -89.0f)
			angs.x = -89.0f;
		angs.z = 0;
		return angs;
	}
	void AngleMatrix(const Vector& angles, matrix& matrix)
	{
		float sr, sp, sy, cr, cp, cy;

		sinCos(DEG2RAD(angles[1]), &sy, &cy);
		sinCos(DEG2RAD(angles[0]), &sp, &cp);
		sinCos(DEG2RAD(angles[2]), &sr, &cr);

		// matrix = (YAW * PITCH) * ROLL
		matrix[0][0] = cp * cy;
		matrix[1][0] = cp * sy;
		matrix[2][0] = -sp;

		float crcy = cr * cy;
		float crsy = cr * sy;
		float srcy = sr * cy;
		float srsy = sr * sy;
		matrix[0][1] = sp * srcy - crsy;
		matrix[1][1] = sp * srsy + crcy;
		matrix[2][1] = sr * cp;

		matrix[0][2] = (sp * crcy + srsy);
		matrix[1][2] = (sp * crsy - srcy);
		matrix[2][2] = cr * cp;

		matrix[0][3] = 0.0f;
		matrix[1][3] = 0.0f;
		matrix[2][3] = 0.0f;
	}
	void MatrixCopy(const matrix& source, matrix& target)
	{
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 4; j++) {
				target[i][j] = source[i][j];
			}
		}
	}
	void ConcatTransforms(const matrix& in1, const matrix& in2, matrix& out) {
		if (&in1 == &out) {
			matrix in1b;
			MatrixCopy(in1, in1b);
			ConcatTransforms(in1b, in2, out);
			return;
		}

		if (&in2 == &out) {
			matrix in2b;
			MatrixCopy(in2, in2b);
			ConcatTransforms(in1, in2b, out);
			return;
		}

		out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] + in1[0][2] * in2[2][0];
		out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] + in1[0][2] * in2[2][1];
		out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] + in1[0][2] * in2[2][2];
		out[0][3] = in1[0][0] * in2[0][3] + in1[0][1] * in2[1][3] + in1[0][2] * in2[2][3] + in1[0][3];

		out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] + in1[1][2] * in2[2][0];
		out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] + in1[1][2] * in2[2][1];
		out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] + in1[1][2] * in2[2][2];
		out[1][3] = in1[1][0] * in2[0][3] + in1[1][1] * in2[1][3] + in1[1][2] * in2[2][3] + in1[1][3];

		out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] + in1[2][2] * in2[2][0];
		out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] + in1[2][2] * in2[2][1];
		out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] + in1[2][2] * in2[2][2];
		out[2][3] = in1[2][0] * in2[0][3] + in1[2][1] * in2[1][3] + in1[2][2] * in2[2][3] + in1[2][3];
	}

	void MatrixSetColumn(const Vector& in, int column, matrix& out)
	{
		out[0][column] = in.x;
		out[1][column] = in.y;
		out[2][column] = in.z;
	}

	void AngleMatrix(const Vector& angles, const Vector& position, matrix& matrix)
	{
		AngleMatrix(angles, matrix);
		MatrixSetColumn(position, 3, matrix);
	}

	RECT GetViewport()
	{
		RECT Viewport = { 0, 0, 0, 0 };
		int w, h;
		interfaces.engine->GetScreenSize(w, h);
		Viewport.right = w; Viewport.bottom = h;
		return Viewport;
	}

	void RotateMatrix(matrix in[128], matrix out[128], float delta, Vector renderOrigin) {
		auto vDelta = Vector(0, delta, 0);
		Vector vOutPos;
		for (int i = 0; i < 128; i++)
		{
			AngleMatrix(vDelta, out[i]);
			MatrixMultiply(out[i], in[i]);
			auto vBonePos = Vector(in[i][0][3],
				in[i][1][3],
				in[i][2][3]) - renderOrigin;
			vOutPos = vector_rotate(vBonePos, vDelta);
			vOutPos += renderOrigin;
			out[i][0][3] = vOutPos.x;
			out[i][1][3] = vOutPos.y;
			out[i][2][3] = vOutPos.z;
		}
	}

	void MatrixMultiply(matrix& in1, const matrix& in2)
	{
		matrix out;
		if (&in1 == &out)
		{
			matrix in1b;
			MatrixCopy(in1, in1b);
			MatrixMultiply(in1b, in2);
			return;
		}
		if (&in2 == &out)
		{
			matrix in2b;
			MatrixCopy(in2, in2b);
			MatrixMultiply(in1, in2b);
			return;
		}
		out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] +
			in1[0][2] * in2[2][0];
		out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] +
			in1[0][2] * in2[2][1];
		out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] +
			in1[0][2] * in2[2][2];
		out[0][3] = in1[0][0] * in2[0][3] + in1[0][1] * in2[1][3] +
			in1[0][2] * in2[2][3] + in1[0][3];
		out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] +
			in1[1][2] * in2[2][0];
		out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] +
			in1[1][2] * in2[2][1];
		out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] +
			in1[1][2] * in2[2][2];
		out[1][3] = in1[1][0] * in2[0][3] + in1[1][1] * in2[1][3] +
			in1[1][2] * in2[2][3] + in1[1][3];
		out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] +
			in1[2][2] * in2[2][0];
		out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] +
			in1[2][2] * in2[2][1];
		out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] +
			in1[2][2] * in2[2][2];
		out[2][3] = in1[2][0] * in2[0][3] + in1[2][1] * in2[1][3] +
			in1[2][2] * in2[2][3] + in1[2][3];

		in1 = out;
	}

	//void AngleMatrix(const Vector &angles, matrix& matrix)
	//{
	//	float sr, sp, sy, cr, cp, cy;
	//
	//	// matrix = (YAW * PITCH) * ROLL
	//	matrix[0][0] = cp * cy;
	//	matrix[1][0] = cp * sy;
	//	matrix[2][0] = -sp;
	//
	//	float crcy = cr * cy;
	//	float crsy = cr * sy;
	//	float srcy = sr * cy;
	//	float srsy = sr * sy;
	//	matrix[0][1] = sp * srcy - crsy;
	//	matrix[1][1] = sp * srsy + crcy;
	//	matrix[2][1] = sr * cp;
	//
	//	matrix[0][2] = (sp*crcy + srsy);
	//	matrix[1][2] = (sp*crsy - srcy);
	//	matrix[2][2] = cr * cp;
	//
	//	matrix[0][3] = 0.0f;
	//	matrix[1][3] = 0.0f;
	//	matrix[2][3] = 0.0f;
	//}
	__forceinline const matrix& world_to_screen_matrix()
	{
		static uintptr_t view_matrix = 0;
		if (!view_matrix)
		{
			view_matrix = (uintptr_t)csgo->Utils.FindPatternIDA(GetModuleHandleA(
				g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
				hs::view_matrix.s().c_str());
			view_matrix = *reinterpret_cast<uintptr_t*>(view_matrix + 0x3) + 176;
		}
		return *reinterpret_cast<matrix*>(view_matrix);
	}

	__forceinline bool screen_transform(const Vector& in, Vector& out)
	{
		auto w2_s_matrix = csgo->viewMatrix;
		out.x = w2_s_matrix[0][0] * in[0] + w2_s_matrix[0][1] * in[1] + w2_s_matrix[0][2] * in[2] + w2_s_matrix[0][3];
		out.y = w2_s_matrix[1][0] * in[0] + w2_s_matrix[1][1] * in[1] + w2_s_matrix[1][2] * in[2] + w2_s_matrix[1][3];
		out.z = 0.0f;

		const auto w = w2_s_matrix[3][0] * in.x + w2_s_matrix[3][1] * in.y + w2_s_matrix[3][2] * in.z + w2_s_matrix[3][3];

		if (w < 0.001f)
		{
			out.x *= 100000;
			out.y *= 100000;
			return false;
		}

		const auto invw = 1.0f / w;
		out.x *= invw;
		out.y *= invw;

		return true;
	}

	bool WorldToScreen(const Vector& in, Vector& out)
	{
		const auto result = screen_transform(in, out);

		out.x = (csgo->w / 2.0f) + (out.x * csgo->w) / 2.0f;
		out.y = (csgo->h / 2.0f) - (out.y * csgo->h) / 2.0f;

		//if (out.x < 0 || out.y < 0)
		//	return false;
		//else if (out.x > w || out.y > h)
		//	return false;
		return result;
	}
	bool WorldToScreen2(Vector& in, Vector& out) // (Vector &in, Vector &out)
	{
		const matrix& worldToScreen = interfaces.engine->WorldToScreenMatrix(); //Grab the world to screen matrix from CEngineClient::WorldToScreenMatrix

		float w = worldToScreen[3][0] * in[0] + worldToScreen[3][1] * in[1] + worldToScreen[3][2] * in[2] + worldToScreen[3][3]; //Calculate the angle in compareson to the player's camera.
		out.z = 0; //Screen doesn't have a 3rd dimension.

		if (w > 0.001) //If the object is within view.
		{
			RECT ScreenSize = GetViewport();
			float fl1DBw = 1 / w; //Divide 1 by the angle.
			out.x = (ScreenSize.right / 2) + (0.5f * ((worldToScreen[0][0] * in[0] + worldToScreen[0][1] * in[1] + worldToScreen[0][2] * in[2] + worldToScreen[0][3]) * fl1DBw) * ScreenSize.right + 0.5f); //Get the X dimension and push it in to the Vector.
			out.y = (ScreenSize.bottom / 2) - (0.5f * ((worldToScreen[1][0] * in[0] + worldToScreen[1][1] * in[1] + worldToScreen[1][2] * in[2] + worldToScreen[1][3]) * fl1DBw) * ScreenSize.bottom + 0.5f); //Get the Y dimension and push it in to the Vector.
			return true;
		}

		return false;
	}

	__forceinline float DotProduct(const Vector& a, const Vector& b)
	{
		return (a.x * b.x + a.y * b.y + a.z * b.z);
	}

	void VectorTransform(const Vector& in1, const matrix& in2, Vector& out) {
		//out.x = in1.x * in2[0][0] + in1.y * in2[0][1] + in1.z * in2[0][2] + in2[0][3];
		//out.y = in1.x * in2[1][0] + in1.y * in2[1][1] + in1.z * in2[1][2] + in2[1][3];
		//out.z = in1.x * in2[2][0] + in1.y * in2[2][1] + in1.z * in2[2][2] + in2[2][3];
		out = {
			in1.Dot(Vector(in2[0][0], in2[0][1], in2[0][2])) + in2[0][3],
			in1.Dot(Vector(in2[1][0], in2[1][1], in2[1][2])) + in2[1][3],
			in1.Dot(Vector(in2[2][0], in2[2][1], in2[2][2])) + in2[2][3]
		};
	}
	float NormalizeYaw(float yaw)
	{
		if (!std::isfinite(yaw))
			return 0.0f;

		if (yaw >= -180.f && yaw <= 180.f)
			return yaw;

		const float rot = std::round(std::abs(yaw / 360.f));

		yaw = (yaw < 0.f) ? yaw + (360.f * rot) : yaw - (360.f * rot);
		return yaw;
	}
	__forceinline float AngleMod(float a)
	{
		return (360.f / 65536) * ((int)(a * (65536.f / 360.0f)) & 65535);
	}
	__forceinline float ApproachAngle(float target, float value, float speed)
	{
		target = AngleMod(target);
		value = AngleMod(value);

		float delta = target - value;

		// Speed is assumed to be positive
		if (speed < 0)
			speed = -speed;

		if (delta < -180)
			delta += 360;
		else if (delta > 180)
			delta -= 360;

		if (delta > speed)
			value += speed;
		else if (delta < -speed)
			value -= speed;
		else
			value = target;

		return value;
	}
	bool IsNearEqual(float v1, float v2, float Tolerance)
	{
		return std::abs(v1 - v2) <= std::abs(Tolerance);
	};
	float AngleDiff(float destAngle, float srcAngle)
	{
		float delta = destAngle - srcAngle;

		if (delta < -180)
			delta += 360;
		else if (delta > 180)
			delta -= 360;

		return delta;
	}
	void sinCos(float radians, PFLOAT sine, PFLOAT cosine)
	{
		__asm
		{
			fld dword ptr[radians]
			fsincos
			mov edx, dword ptr[cosine]
			mov eax, dword ptr[sine]
			fstp dword ptr[edx]
			fstp dword ptr[eax]
		}
	}
	float DotProduct(const float* v1, const float* v2) {
		return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
	};
	void VectorTransform(const float* in1, const matrix& in2, float* out)
	{
		out[0] = DotProduct(in1, in2[0]) + in2[0][3];
		out[1] = DotProduct(in1, in2[1]) + in2[1][3];
		out[2] = DotProduct(in1, in2[2]) + in2[2][3];
	};

	void VectorTransform_Wrapper(const Vector& in1, const matrix& in2, Vector& out)
	{
		VectorTransform(&in1.x, in2, &out.x);
	};
	void AngleVectors(const Vector& angles, Vector& forward)
	{
		//Assert(s_bMathlibInitialized);
		//Assert(forward);

		float sp, sy, cp, cy;

		sy = sin(DEG2RAD(angles[1]));
		cy = cos(DEG2RAD(angles[1]));

		sp = sin(DEG2RAD(angles[0]));
		cp = cos(DEG2RAD(angles[0]));

		forward.x = cp * cy;
		forward.y = cp * sy;
		forward.z = -sp;
	}


	float segment_to_segment(const Vector s1, const Vector s2, const Vector k1, const Vector k2)
	{
		static auto constexpr epsilon = 0.00000001;

		auto u = s2 - s1;
		auto v = k2 - k1;
		const auto w = s1 - k1;

		const auto a = u.Dot(u);
		const auto b = u.Dot(v);
		const auto c = v.Dot(v);
		const auto d = u.Dot(w);
		const auto e = v.Dot(w);
		const auto D = a * c - b * b;
		float sn, sd = D;
		float tn, td = D;

		if (D < epsilon) {
			sn = 0.0;
			sd = 1.0;
			tn = e;
			td = c;
		}
		else {
			sn = b * e - c * d;
			tn = a * e - b * d;

			if (sn < 0.0) {
				sn = 0.0;
				tn = e;
				td = c;
			}
			else if (sn > sd) {
				sn = sd;
				tn = e + b;
				td = c;
			}
		}

		if (tn < 0.0) {
			tn = 0.0;

			if (-d < 0.0)
				sn = 0.0;
			else if (-d > a)
				sn = sd;
			else {
				sn = -d;
				sd = a;
			}
		}
		else if (tn > td) {
			tn = td;

			if (-d + b < 0.0)
				sn = 0;
			else if (-d + b > a)
				sn = sd;
			else {
				sn = -d + b;
				sd = a;
			}
		}

		const float sc = abs(sn) < epsilon ? 0.0 : sn / sd;
		const float tc = abs(tn) < epsilon ? 0.0 : tn / td;

		m128 n;
		auto dp = w + u * sc - v * tc;
		n.f[0] = dp.Dot(dp);
		const auto calc = sqrt_ps(n.v);
		return reinterpret_cast<const m128*>(&calc)->f[0];
	}

	Vector vector_rotate(Vector& in1, matrix& in2)
	{
		return Vector(in1.Dot(in2[0]), in1.Dot(in2[1]), in1.Dot(in2[2]));
	}

	float DotProduct22(const float* v1, const float* v2) {
		return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
	}

	void  VectorRotate(const float* in1, const matrix& in2, float* out)
	{

		out[0] = DotProduct22(in1, in2[0]);
		out[1] = DotProduct22(in1, in2[1]);
		out[2] = DotProduct22(in1, in2[2]);
	}

	void  VectorRotate(const Vector& in1, const matrix& in2, Vector& out)
	{
		VectorRotate(&in1.x, in2, &out.x);
	}

	void  VectorRotate(const Vector& in1, const Vector& in2, Vector& out)
	{
		matrix matRotate;
		AngleMatrix(in2, matRotate);
		VectorRotate(in1, matRotate, out);
	}
	Vector vector_rotate(Vector& in1, Vector& in2)
	{
		matrix m;
		AngleMatrix(in2, m);
		return vector_rotate(in1, m);
	}

	void vector_i_rotate(Vector in1, matrix in2, Vector& out)
	{
		out.x = in1.x * in2[0][0] + in1.y * in2[1][0] + in1.z * in2[2][0];
		out.y = in1.x * in2[0][1] + in1.y * in2[1][1] + in1.z * in2[2][1];
		out.z = in1.x * in2[0][2] + in1.y * in2[1][2] + in1.z * in2[2][2];
	}

	bool intersect_line_with_bb(Vector& start, Vector& end, Vector& min, Vector& max) {
		float d1, d2, f;
		auto start_solid = true;
		auto t1 = -1.0f, t2 = 1.0f;

		const float s[3] = { start.x, start.y, start.z };
		const float e[3] = { end.x, end.y, end.z };
		const float mi[3] = { min.x, min.y, min.z };
		const float ma[3] = { max.x, max.y, max.z };

		for (auto i = 0; i < 6; i++) {
			if (i >= 3) {
				const auto j = i - 3;

				d1 = s[j] - ma[j];
				d2 = d1 + e[j];
			}
			else {
				d1 = -s[i] + mi[i];
				d2 = d1 - e[i];
			}

			if (d1 > 0.0f && d2 > 0.0f)
				return false;

			if (d1 <= 0.0f && d2 <= 0.0f)
				continue;

			if (d1 > 0)
				start_solid = false;

			if (d1 > d2) {
				f = d1;
				if (f < 0.0f)
					f = 0.0f;

				f /= d1 - d2;
				if (f > t1)
					t1 = f;
			}
			else {
				f = d1 / (d1 - d2);
				if (f < t2)
					t2 = f;
			}
		}

		return start_solid || (t1 < t2&& t1 >= 0.0f);
	}

	void vector_i_transform(const Vector& in1, const matrix& in2, Vector& out)
	{
		out.x = (in1.x - in2[0][3]) * in2[0][0] + (in1.y - in2[1][3]) * in2[1][0] + (in1.z - in2[2][3]) * in2[2][0];
		out.y = (in1.x - in2[0][3]) * in2[0][1] + (in1.y - in2[1][3]) * in2[1][1] + (in1.z - in2[2][3]) * in2[2][1];
		out.z = (in1.x - in2[0][3]) * in2[0][2] + (in1.y - in2[1][3]) * in2[1][2] + (in1.z - in2[2][3]) * in2[2][2];
	}

	void AngleVectors(const Vector angles, Vector& forward, Vector& right, Vector& up)
	{
		float angle;
		static float sp, sy, cp, cy;

		angle = angles[0] * (PI / 180.f);
		sp = sin(angle);
		cp = cos(angle);

		angle = angles[1] * (PI / 180.f);
		sy = sin(angle);
		cy = cos(angle);


		forward[0] = cp * cy;
		forward[1] = cp * sy;
		forward[2] = -sp;


		static float sr, cr;

		angle = angles[2] * (PI / 180.f);
		sr = sin(angle);
		cr = cos(angle);


		right[0] = -1 * sr * sp * cy + -1 * cr * -sy;
		right[1] = -1 * sr * sp * sy + -1 * cr * cy;
		right[2] = -1 * sr * cp;

		up[0] = cr * sp * cy + -sr * -sy;
		up[1] = cr * sp * sy + -sr * cy;
		up[2] = cr * cp;
	}
	void AngleVectorsDefault(Vector q, Vector& vec)
	{
		float sp, sy, cp, cy;

		sinCos(DEG2RAD(q[1]), &sy, &cy);
		sinCos(DEG2RAD(q[0]), &sp, &cp);

		vec.x = cp * cy;
		vec.y = cp * sy;
		vec.z = -sp;
	}
	void AngleVector4(const Vector& angles, Vector& forward, Vector& right, Vector& up)
	{
		float sr, sp, sy, cr, cp, cy;

		sinCos(DEG2RAD(angles[1]), &sy, &cy);
		sinCos(DEG2RAD(angles[0]), &sp, &cp);
		sinCos(DEG2RAD(angles[2]), &sr, &cr);

		forward.x = (cp * cy);
		forward.y = (cp * sy);
		forward.z = (-sp);
		right.x = (-1 * sr * sp * cy + -1 * cr * -sy);
		right.y = (-1 * sr * sp * sy + -1 * cr * cy);
		right.z = (-1 * sr * cp);
		up.x = (cr * sp * cy + -sr * -sy);
		up.y = (cr * sp * sy + -sr * cy);
		up.z = (cr * cp);
	}
	void VectorAngles(const Vector& forward, const Vector& pseudoup, Vector& angles)
	{
		auto CrossProducts = [](const Vector& a, const Vector& b)
		{
			return Vector(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
		};

		Vector left = CrossProducts(pseudoup, forward);
		left = left.NormalizeInPlace();

		float forwardDist = forward.Length2D();

		if (forwardDist > 0.001f)
		{
			angles.x = atan2f(-forward.z, forwardDist) * 180 / 3.14159265358979323846f;
			angles.y = atan2f(forward.y, forward.x) * 180 / 3.14159265358979323846f;

			float upZ = (left.y * forward.x) - (left.x * forward.y);
			angles.z = atan2f(left.z, upZ) * 180 / 3.14159265358979323846f;
		}
		else
		{
			angles.x = atan2f(-forward.z, forwardDist) * 180 / 3.14159265358979323846f;
			angles.y = atan2f(-left.x, left.y) * 180 / 3.14159265358979323846f;
			angles.z = 0;
		}
	}
	void AngleVectors(const Vector& angles, Vector* forward, Vector* right, Vector* up)
	{
		float sr, sp, sy, cr, cp, cy;

		sinCos(DEG2RAD(angles[1]), &sy, &cy);
		sinCos(DEG2RAD(angles[0]), &sp, &cp);
		sinCos(DEG2RAD(angles[2]), &sr, &cr);

		if (forward)
		{
			forward->x = cp * cy;
			forward->y = cp * sy;
			forward->z = -sp;
		}

		if (right)
		{
			right->x = (-1 * sr * sp * cy + -1 * cr * -sy);
			right->y = (-1 * sr * sp * sy + -1 * cr * cy);
			right->z = -1 * sr * cp;
		}

		if (up)
		{
			up->x = (cr * sp * cy + -sr * -sy);
			up->y = (cr * sp * sy + -sr * cy);
			up->z = cr * cp;
		}
	}
	void AngleVectors(const Vector& angles, Vector* forward)
	{
		//Assert(s_bMathlibInitialized);
		//Assert(forward);

		float	sp, sy, cp, cy;

		sy = sin(DEG2RAD(angles[1]));
		cy = cos(DEG2RAD(angles[1]));

		sp = sin(DEG2RAD(angles[0]));
		cp = cos(DEG2RAD(angles[0]));

		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	}
	float GetFov(const Vector& viewAngle, const Vector& aimAngle)
	{
		//Vector ang, aim;

		//AngleVectors(viewAngle, aim);
		//AngleVectors(aimAngle, ang);

		//return RAD2DEG(acos(aim.Dot(ang) / aim.LengthSqr()));
		Vector delta = aimAngle - viewAngle;
		delta = delta.NormalizeInPlace();

		return min(sqrtf(powf(delta.x, 2.0f) + powf(delta.y, 2.0f)), 180.0f);
	}

	float DistanceScreen(Vector2D vDstPos, Vector2D vSrcPos)
	{
		return (sqrt(pow(vSrcPos.x - vDstPos.x, 2) + pow(vSrcPos.y - vDstPos.y, 2)));
	}
	float RandomFloat(float a, float b)
	{
		float random = ((float)rand()) / (float)RAND_MAX;
		float diff = b - a;
		float r = random * diff;
		return a + r;
	}
	void SmoothAngles(Vector MyViewAngles, Vector AimAngles, Vector& OutAngles, float Smoothing)
	{
		OutAngles = AimAngles - MyViewAngles;

		normalize(OutAngles);

		OutAngles.x /= Smoothing + MyViewAngles.x;
		OutAngles.y /= Smoothing + MyViewAngles.y;

		normalize(OutAngles);
	}
	void ClampAngles(Vector& angles)
	{
		if (angles.x > 89.0f) angles.x = 89.0f;
		else if (angles.x < -89.0f) angles.x = 89.0f;

		if (angles.y > 180.0f) angles.y = 180.0f;
		else if (angles.y < -180.0f) angles.y = 180.0f;

		angles.z = 0;
	}
	Vector CalculateAngle(Vector src, Vector dst)
	{
		Vector angles;

		Vector delta = src - dst;
		float hyp = delta.Length2D();

		angles.y = std::atanf(delta.y / delta.x) * 57.2957795131f;
		angles.x = std::atanf(-delta.z / hyp) * -57.2957795131f;
		angles.z = 0.0f;

		if (delta.x >= 0.0f)
			angles.y += 180.0f;

		return angles;
	}
	void SetMatrixOrigin(Vector pos, matrix& pMat) {
		for (size_t i{ }; i < 3; ++i) {
			pMat[i][3] = pos[i];
		}
	}

	Vector GetMatrixOrigin(const matrix& src) {
		return Vector(src[0][3], src[1][3], src[2][3]);
	}
	void VectorTransform2(const Vector in1, matrix in2, Vector& out)
	{
		out[0] = DotProduct(in1, Vector(in2[0][0], in2[0][1], in2[0][2])) + in2[0][3];
		out[1] = DotProduct(in1, Vector(in2[1][0], in2[1][1], in2[1][2])) + in2[1][3];
		out[2] = DotProduct(in1, Vector(in2[2][0], in2[2][1], in2[2][2])) + in2[2][3];

	}
	void VectorAngles(const Vector& forward, Vector& angles)
	{
		float tmp, yaw, pitch;

		if (forward[1] == 0 && forward[0] == 0)
		{
			yaw = 0;
			if (forward[2] > 0)
				pitch = 270;
			else
				pitch = 90;
		}
		else
		{
			yaw = (atan2(forward[1], forward[0]) * 180 / PI);
			if (yaw < 0)
				yaw += 360;

			tmp = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
			pitch = (atan2(-forward[2], tmp) * 180 / PI);
			if (pitch < 0)
				pitch += 360;
		}

		angles[0] = pitch;
		angles[1] = yaw;
		angles[2] = 0;
	}
	float Normalize133(Vector& res)
	{
		float l = res.Length();
		if (l != 0.0f)
		{
			res /= l;
		}
		else
		{
			res.x = res.y = res.z = 0.0f;
		}
		return l;
	}
	float DistanceToRay(const Vector& pos, const Vector& rayStart, const Vector& rayEnd, float* along, Vector* pointOnRay)
	{
		Vector to = pos - rayStart;
		Vector dir = rayEnd - rayStart;
		float length = Normalize133(dir);

		float rangeAlong = dir.Dot(to);
		if (along)
			*along = rangeAlong;

		float range;

		if (rangeAlong < 0.0f)
		{
			range = -(pos - rayStart).Length();

			if (pointOnRay)
				*pointOnRay = rayStart;
		}
		else if (rangeAlong > length)
		{
			range = -(pos - rayEnd).Length();

			if (pointOnRay)
				*pointOnRay = rayEnd;
		}
		else
		{
			Vector onRay = rayStart + (dir * rangeAlong);
			range = (pos - onRay).Length();

			if (pointOnRay)
				*pointOnRay = onRay;
		}

		return range;
	}

	float AngleNormalize(float angle)
	{
		angle = fmodf(angle, 360.0f);
		if (angle > 180)
		{
			angle -= 360;
		}
		if (angle < -180)
		{
			angle += 360;
		}
		return angle;
	}


	float VectorNormalize(Vector& v)
	{
		//Assert(v.IsValid());
		float l = v.Length();
		if (l != 0.0f)
		{
			v /= l;
		}
		else
		{
			// FIXME:
			// Just copying the existing implemenation; shouldn't res.z == 0?
			v.x = v.y = 0.0f; v.z = 1.0f;
		}
		return l;
	}
	void VectorMA(const Vector& start, float scale, const Vector& direction, Vector& dest)
	{
		dest.x = start.x + direction.x * scale;
		dest.y = start.y + direction.y * scale;
		dest.z = start.z + direction.z * scale;
	}
}