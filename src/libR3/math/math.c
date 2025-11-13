#include <include/libR3/math/math.h>
#include <include/libR3/io/log.h>

/*	VECTOR	*/
f32 r3Vec2Mag(Vec2 vec2) {
	return sqrtf(VEC_X(vec2) * VEC_X(vec2) + VEC_Y(vec2) * VEC_Y(vec2));
}

f32 r3Vec3Mag(Vec3 vec3) {
	return sqrtf(VEC_X(vec3) * VEC_X(vec3) + VEC_Y(vec3) * VEC_Y(vec3) + VEC_Z(vec3) * VEC_Z(vec3));
}

f32 r3Vec4Mag(Vec4 vec4) {
	return sqrtf(VEC_X(vec4) * VEC_X(vec4) + VEC_Y(vec4) * VEC_Y(vec4) + VEC_Z(vec4) * VEC_Z(vec4) + VEC_W(vec4) * VEC_W(vec4));
}


Vec2 r3Vec2Norm(Vec2 vec2) {
	return r3Vec2Scale(1.0 / r3Vec2Mag(vec2), vec2);
}

Vec3 r3Vec3Norm(Vec3 vec3) {
	return r3Vec3Scale(1.0 / r3Vec3Mag(vec3), vec3);
}

Vec4 r3Vec4Norm(Vec4 vec4) {
	return r3Vec4Scale(1.0 / r3Vec4Mag(vec4), vec4);
}


f32 r3Vec2Dot(Vec2 veca, Vec2 vecb) {
	return VEC_X(veca) * VEC_X(vecb) + VEC_Y(veca) * VEC_Y(vecb);
}

f32 r3Vec3Dot(Vec3 veca, Vec3 vecb) {
	return VEC_X(veca) * VEC_X(vecb) + VEC_Y(veca) * VEC_Y(vecb) + VEC_Z(veca) * VEC_Z(vecb);
}

f32 r3Vec4Dot(Vec4 veca, Vec4 vecb) {
	return VEC_X(veca) * VEC_X(vecb) + VEC_Y(veca) * VEC_Y(vecb) + VEC_Z(veca) * VEC_Z(vecb) + VEC_W(veca) * VEC_W(vecb);
}


Vec2 r3Vec2Add(Vec2 veca, Vec2 vecb) {
	return (Vec2){ .data = {VEC_X(veca) + VEC_X(vecb), VEC_Y(veca) + VEC_Y(vecb)} };
}

Vec3 r3Vec3Add(Vec3 veca, Vec3 vecb) {
	return (Vec3){ .data = {VEC_X(veca) + VEC_X(vecb), VEC_Y(veca) + VEC_Y(vecb), VEC_Z(veca) + VEC_Z(vecb)} };
}

Vec4 r3Vec4Add(Vec4 veca, Vec4 vecb) {
	return (Vec4){ .data = {VEC_X(veca) + VEC_X(vecb), VEC_Y(veca) + VEC_Y(vecb), VEC_Z(veca) + VEC_Z(vecb), VEC_W(veca) + VEC_W(vecb)} };
}


Vec2 r3Vec2Sub(Vec2 veca, Vec2 vecb) {
	return (Vec2){ .data = {VEC_X(veca) - VEC_X(vecb), VEC_Y(veca) - VEC_Y(vecb)} };
}

Vec3 r3Vec3Sub(Vec3 veca, Vec3 vecb) {
	return (Vec3){ .data = {VEC_X(veca) - VEC_X(vecb), VEC_Y(veca) - VEC_Y(vecb), VEC_Z(veca) - VEC_Z(vecb)} };
}

Vec4 r3Vec4Sub(Vec4 veca, Vec4 vecb) {
	return (Vec4){ .data = {VEC_X(veca) - VEC_X(vecb), VEC_Y(veca) - VEC_Y(vecb), VEC_Z(veca) - VEC_Z(vecb), VEC_W(veca) - VEC_W(vecb)} };
}


Vec2 r3Vec2Mul(Vec2 veca, Vec2 vecb) {
	return (Vec2){ .data = {VEC_X(veca) * VEC_X(vecb), VEC_Y(veca) * VEC_Y(vecb)} };
}

Vec3 r3Vec3Mul(Vec3 veca, Vec3 vecb) {
	return (Vec3){ .data = {VEC_X(veca) * VEC_X(vecb), VEC_Y(veca) * VEC_Y(vecb), VEC_Z(veca) * VEC_Z(vecb)} };
}

Vec4 r3Vec4Mul(Vec4 veca, Vec4 vecb) {
	return (Vec4){ .data = {VEC_X(veca) * VEC_X(vecb), VEC_Y(veca) * VEC_Y(vecb), VEC_Z(veca) * VEC_Z(vecb), VEC_W(veca) * VEC_W(vecb)} };
}


Vec2 r3Vec2Div(Vec2 veca, Vec2 vecb) {
	return (Vec2){ .data = {VEC_X(veca) / VEC_X(vecb), VEC_Y(veca) / VEC_Y(vecb)} };
}

Vec3 r3Vec3Div(Vec3 veca, Vec3 vecb) {
	return (Vec3){ .data = {VEC_X(veca) / VEC_X(vecb), VEC_Y(veca) / VEC_Y(vecb), VEC_Z(veca) / VEC_Z(vecb)} };
}

Vec4 r3Vec4Div(Vec4 veca, Vec4 vecb) {
	return (Vec4){ .data = {VEC_X(veca) / VEC_X(vecb), VEC_Y(veca) / VEC_Y(vecb), VEC_Z(veca) / VEC_Z(vecb), VEC_W(veca) / VEC_W(vecb)} };
}


Vec2 r3Vec2Scale(f32 scale, Vec2 vec2) {
	return (Vec2){ .data = {VEC_X(vec2) * scale, VEC_Y(vec2) * scale} };
}

Vec3 r3Vec3Scale(f32 scale, Vec3 vec3) {
	return (Vec3){ .data = {VEC_X(vec3) * scale, VEC_Y(vec3) * scale, VEC_Z(vec3) * scale} };
}

Vec4 r3Vec4Scale(f32 scale, Vec4 vec4) {
	return (Vec4){ .data = {VEC_X(vec4) * scale, VEC_Y(vec4) * scale, VEC_Z(vec4) * scale, VEC_W(vec4) * scale} };
}


Vec3 r3Vec3Cross(Vec3 veca, Vec3 vecb) {
	return (Vec3){ .data = {
		VEC_Y(veca) * VEC_Z(vecb) - VEC_Z(veca) * VEC_Y(vecb),
		VEC_Z(veca) * VEC_X(vecb) - VEC_X(veca) * VEC_Z(vecb),
		VEC_X(veca) * VEC_Y(vecb) - VEC_Y(veca) * VEC_X(vecb)
	}};
}


/* RIGHT HANDED COLUMN MAJOR MATRIX */
Vec3 r3Mat4MulVec3(Vec3 vec3, Mat4 mat4) {
	Vec3 result = {0};
	result.data[0] = mat4.data[0] * vec3.data[0] +
					 mat4.data[4] * vec3.data[1] +
					 mat4.data[8] * vec3.data[2] +
					 mat4.data[12];
	
	result.data[1] = mat4.data[1] * vec3.data[0] +
					 mat4.data[5] * vec3.data[1] +
					 mat4.data[9] * vec3.data[2] +
					 mat4.data[13];
	
	result.data[2] = mat4.data[2] * vec3.data[0] +
					 mat4.data[6] * vec3.data[1] +
					 mat4.data[10] * vec3.data[2] +
					 mat4.data[14];
	return result;
}

Mat4 r3Mat4MulMat4(Mat4 mata, Mat4 matb) {
	Mat4 result = {0};
	FOR(u32, col, 0, 4, 1) {
		FOR(u32, row, 0, 4, 1) {
			FOR_K(0, 4, 1) {
				result.data[col * 4 + row] += mata.data[k * 4 + row] * matb.data[col * 4 + k];
			}
		}
	}
	return result;
}


Mat4 r3Mat4RotateX(f32 angle) {
	f32 rad_angle = RADIANS(angle);
	f32 cos_angle = cosf(rad_angle);
	f32 sin_angle = sinf(rad_angle);

	Mat4 result = { .data={
		1, 0, 0, 0,
		0, cos_angle, sin_angle, 0,
		0, -sin_angle, cos_angle, 0,
		0, 0, 0, 1
	} };

	return result;
}

Mat4 r3Mat4RotateY(f32 angle) {
	f32 rad_angle = RADIANS(angle);
	f32 cos_angle = cosf(rad_angle);
	f32 sin_angle = sinf(rad_angle);

	Mat4 result = { .data={
		cos_angle, 0, -sin_angle, 0,
		0, 1, 0, 0,
		sin_angle, 0, cos_angle, 0,
		0, 0, 0, 1
	} };

	return result;
}

Mat4 r3Mat4RotateZ(f32 angle) {
	f32 rad_angle = RADIANS(angle);
	f32 cos_angle = cosf(rad_angle);
	f32 sin_angle = sinf(rad_angle);

	Mat4 result = { .data={
		cos_angle, sin_angle, 0, 0,
		-sin_angle, cos_angle, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	} };

	return result;
}

// axis-angle rotation (Rodrigues rotation formula)
Mat4 r3Mat4Rotate(Vec3 axis, f32 angle) {
	f32 rad_angle = RADIANS(angle);
	f32 cos_angle = cosf(rad_angle);
	f32 sin_angle = sinf(rad_angle);
	Mat4 result = IDENTITY();

	f32 axis_len = r3Vec3Mag(axis);
	if (axis_len <= 0.0) return result;
	else {
		axis.data[0] /= axis_len;
		axis.data[1] /= axis_len;
		axis.data[2] /= axis_len;
	}

	result.data[0] = cos_angle + (1 - cos_angle) * (axis.data[0] * axis.data[0]);
	result.data[1] = (1 - cos_angle) * (axis.data[0] * axis.data[1]) + sin_angle * axis.data[2];
	result.data[2] = (1 - cos_angle) * (axis.data[0] * axis.data[2]) - sin_angle * axis.data[1];

	result.data[4] = (1 - cos_angle) * (axis.data[1] * axis.data[0]) - sin_angle * axis.data[2];
	result.data[5] = cos_angle + (1 - cos_angle) * (axis.data[1] * axis.data[1]);
	result.data[6] = (1 - cos_angle) * (axis.data[1] * axis.data[2]) + sin_angle * axis.data[0];

	result.data[8] = (1 - cos_angle) * (axis.data[2] * axis.data[0]) + sin_angle * axis.data[1];
	result.data[9] = (1 - cos_angle) * (axis.data[2] * axis.data[1]) - sin_angle * axis.data[0];
	result.data[10] = cos_angle + (1 - cos_angle) * (axis.data[2] * axis.data[2]);

	result.data[3] = 0.0f;
	result.data[7] = 0.0f;
	result.data[11] = 0.0f;
	result.data[12] = 0.0f;
	result.data[13] = 0.0f;
	result.data[14] = 0.0f;
	result.data[15] = 1.0f;

	return result;
}

Mat4 r3Mat4Scale(Vec3 scale, Mat4 mat4) {
	Mat4 scale_mat4 = { .data={
		scale.data[0], 0, 0, 0,
		0, scale.data[1], 0, 0,
		0, 0, scale.data[2], 0,
		0, 0, 0, 1.0f
	} };

	return r3Mat4MulMat4(scale_mat4, mat4);
}

Mat4 r3Mat4Translate(Vec3 translation, Mat4 mat4) {
	Mat4 trans_mat4 = { .data={
		1.0f, 0, 0, 0,
		0, 1.0f, 0, 0,
		0, 0, 1.0f, 0,
		translation.data[0], translation.data[1], translation.data[2], 1.0f
	} };

	return r3Mat4MulMat4(trans_mat4, mat4);
}


Mat4 r3Mat4Transpose(Mat4 mat4) {
    Mat4 result = mat4;
    SWAP(f32, result.data[1], result.data[4]);   // (0,1) <-> (1,0)
    SWAP(f32, result.data[2], result.data[8]);   // (0,2) <-> (2,0)
    SWAP(f32, result.data[3], result.data[12]);  // (0,3) <-> (3,0)
    SWAP(f32, result.data[6], result.data[9]);   // (1,2) <-> (2,1)
    SWAP(f32, result.data[7], result.data[13]);  // (1,3) <-> (3,1)
    SWAP(f32, result.data[11],result.data[14]);  // (2,3) <-> (3,2)
    return result;
}


Mat4 r3Mat4Lookat(Vec3 eye, Vec3 center, Vec3 up) {
    Vec3 f = r3Vec3Norm(r3Vec3Sub(center, eye));
    Vec3 s = r3Vec3Norm(r3Vec3Cross(f, up));
    Vec3 u = r3Vec3Cross(s, f);

    return (Mat4){ .data = {
        s.data[0], u.data[0], -f.data[0], 0.0f,
        s.data[1], u.data[1], -f.data[1], 0.0f,
        s.data[2], u.data[2], -f.data[2], 0.0f,
        -r3Vec3Dot(s, eye),
        -r3Vec3Dot(u, eye),
         r3Vec3Dot(f, eye),
        1.0f
    }};
}

Mat4 r3Mat4Perspective(f32 fov, f32 aspect, f32 near, f32 far) {
    f32 f = 1.0f / tanf(RADIANS(fov) / 2.0f);
    f32 nf = 1.0f / (near - far);

    return (Mat4){ .data = {
        f / aspect, 0.0f, 0.0f, 0.0f,
        0.0f, f, 0.0f, 0.0f,
        0.0f, 0.0f, (far + near) * nf, -1.0f,
        0.0f, 0.0f, (2.0f * far * near) * nf, 0.0f
    }};
}

Mat4 r3Mat4Ortho(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far) {
	f32 right_m_left = right - left;
	f32 top_m_bottom = top - bottom;
	f32 far_m_near = far - near;
	
	f32 right_p_left = right + left;
	f32 top_p_bottom = top + bottom;
	f32 far_p_near = far + near;

	return (Mat4){ .data={
		2.0f / right_m_left, 0, 0, 0,
		0, 2.0f / top_m_bottom, 0, 0,
		0, 0, -(2.0f / far_m_near), 0,
		-(right_p_left / right_m_left), -(top_p_bottom / top_m_bottom), -(far_p_near / far_m_near), 1.0f
	} };
}
