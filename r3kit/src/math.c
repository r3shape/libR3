#include <r3kit/include/io/log.h>
#include <r3kit/include/math.h>
#include <math.h>

/*	UTILITY	*/
none r3_vec2_log(char* message, Vec2 vec2) {
	r3_log_stdoutf(INFO_LOG, "%s: %0.3f %0.3f\n", message, VEC_X(vec2), VEC_Y(vec2));
}

none r3_vec3_log(char* message, Vec3 vec3) {
	r3_log_stdoutf(INFO_LOG, "%s: %0.3f %0.3f %0.3f\n", message, VEC_X(vec3), VEC_Y(vec3), VEC_Z(vec3));
}

none r3_vec4_log(char* message, Vec4 vec4) {
	r3_log_stdoutf(INFO_LOG, "%s: %0.3f %0.3f %0.3f %0.3f\n", message, VEC_X(vec4), VEC_Y(vec4), VEC_Z(vec4), VEC_W(vec4));
}

none r3_mat4_log(char* message, Mat4 mat4) {
	r3_log_stdoutf(INFO_LOG, "%s:\n", message);

	u8 r = 0;
	FOR_I(0, 16, 1) {
		if (r == 3) {
			r3_log_stdoutf(INFO_LOG, "%0.3f \n", mat4.data[i]);
			r = 0;
		} else {
			r3_log_stdoutf(INFO_LOG, "%0.3f ", mat4.data[i]);
			r++;
		}
	}
}


/*	SCALAR	*/
f32 r3_radians(f32 degrees) { return degrees * (PI / 180.0); }
f32 r3_degrees(f32 radians) { return radians * (180.0 / PI); }


/*	VECTOR	*/
f32 r3_vec2_mag(Vec2 vec2) {
	return sqrtf(VEC_X(vec2) * VEC_X(vec2) + VEC_Y(vec2) * VEC_Y(vec2));
}

f32 r3_vec3_mag(Vec3 vec3) {
	return sqrtf(VEC_X(vec3) * VEC_X(vec3) + VEC_Y(vec3) * VEC_Y(vec3) + VEC_Z(vec3) * VEC_Z(vec3));
}

f32 r3_vec4_mag(Vec4 vec4) {
	return sqrtf(VEC_X(vec4) * VEC_X(vec4) + VEC_Y(vec4) * VEC_Y(vec4) + VEC_Z(vec4) * VEC_Z(vec4) + VEC_W(vec4) * VEC_W(vec4));
}


Vec2 r3_vec2_norm(Vec2 vec2) {
	return r3_vec2_scale(1.0 / r3_vec2_mag(vec2), vec2);
}

Vec3 r3_vec3_norm(Vec3 vec3) {
	return r3_vec3_scale(1.0 / r3_vec3_mag(vec3), vec3);
}

Vec4 r3_vec4_norm(Vec4 vec4) {
	return r3_vec4_scale(1.0 / r3_vec4_mag(vec4), vec4);
}


f32 r3_vec2_dot(Vec2 veca, Vec2 vecb) {
	return VEC_X(veca) * VEC_X(vecb) + VEC_Y(veca) * VEC_Y(vecb);
}

f32 r3_vec3_dot(Vec3 veca, Vec3 vecb) {
	return VEC_X(veca) * VEC_X(vecb) + VEC_Y(veca) * VEC_Y(vecb) + VEC_Z(veca) * VEC_Z(vecb);
}

f32 r3_vec4_dot(Vec4 veca, Vec4 vecb) {
	return VEC_X(veca) * VEC_X(vecb) + VEC_Y(veca) * VEC_Y(vecb) + VEC_Z(veca) * VEC_Z(vecb) + VEC_W(veca) * VEC_W(vecb);
}


Vec2 r3_vec2_add(Vec2 veca, Vec2 vecb) {
	return (Vec2){ .data = {VEC_X(veca) + VEC_X(vecb), VEC_Y(veca) + VEC_Y(vecb)} };
}

Vec3 r3_vec3_add(Vec3 veca, Vec3 vecb) {
	return (Vec3){ .data = {VEC_X(veca) + VEC_X(vecb), VEC_Y(veca) + VEC_Y(vecb), VEC_Z(veca) + VEC_Z(vecb)} };
}

Vec4 r3_vec4_add(Vec4 veca, Vec4 vecb) {
	return (Vec4){ .data = {VEC_X(veca) + VEC_X(vecb), VEC_Y(veca) + VEC_Y(vecb), VEC_Z(veca) + VEC_Z(vecb), VEC_W(veca) + VEC_W(vecb)} };
}


Vec2 r3_vec2_sub(Vec2 veca, Vec2 vecb) {
	return (Vec2){ .data = {VEC_X(veca) - VEC_X(vecb), VEC_Y(veca) - VEC_Y(vecb)} };
}

Vec3 r3_vec3_sub(Vec3 veca, Vec3 vecb) {
	return (Vec3){ .data = {VEC_X(veca) - VEC_X(vecb), VEC_Y(veca) - VEC_Y(vecb), VEC_Z(veca) - VEC_Z(vecb)} };
}

Vec4 r3_vec4_sub(Vec4 veca, Vec4 vecb) {
	return (Vec4){ .data = {VEC_X(veca) - VEC_X(vecb), VEC_Y(veca) - VEC_Y(vecb), VEC_Z(veca) - VEC_Z(vecb), VEC_W(veca) - VEC_W(vecb)} };
}


Vec2 r3_vec2_mul(Vec2 veca, Vec2 vecb) {
	return (Vec2){ .data = {VEC_X(veca) * VEC_X(vecb), VEC_Y(veca) * VEC_Y(vecb)} };
}

Vec3 r3_vec3_mul(Vec3 veca, Vec3 vecb) {
	return (Vec3){ .data = {VEC_X(veca) * VEC_X(vecb), VEC_Y(veca) * VEC_Y(vecb), VEC_Z(veca) * VEC_Z(vecb)} };
}

Vec4 r3_vec4_mul(Vec4 veca, Vec4 vecb) {
	return (Vec4){ .data = {VEC_X(veca) * VEC_X(vecb), VEC_Y(veca) * VEC_Y(vecb), VEC_Z(veca) * VEC_Z(vecb), VEC_W(veca) * VEC_W(vecb)} };
}


Vec2 r3_vec2_div(Vec2 veca, Vec2 vecb) {
	return (Vec2){ .data = {VEC_X(veca) / VEC_X(vecb), VEC_Y(veca) / VEC_Y(vecb)} };
}

Vec3 r3_vec3_div(Vec3 veca, Vec3 vecb) {
	return (Vec3){ .data = {VEC_X(veca) / VEC_X(vecb), VEC_Y(veca) / VEC_Y(vecb), VEC_Z(veca) / VEC_Z(vecb)} };
}

Vec4 r3_vec4_div(Vec4 veca, Vec4 vecb) {
	return (Vec4){ .data = {VEC_X(veca) / VEC_X(vecb), VEC_Y(veca) / VEC_Y(vecb), VEC_Z(veca) / VEC_Z(vecb), VEC_W(veca) / VEC_W(vecb)} };
}


Vec2 r3_vec2_scale(f32 scale, Vec2 vec2) {
	return (Vec2){ .data = {VEC_X(vec2) * scale, VEC_Y(vec2) * scale} };
}

Vec3 r3_vec3_scale(f32 scale, Vec3 vec3) {
	return (Vec3){ .data = {VEC_X(vec3) * scale, VEC_Y(vec3) * scale, VEC_Z(vec3) * scale} };
}

Vec4 r3_vec4_scale(f32 scale, Vec4 vec4) {
	return (Vec4){ .data = {VEC_X(vec4) * scale, VEC_Y(vec4) * scale, VEC_Z(vec4) * scale, VEC_W(vec4) * scale} };
}


Vec3 r3_vec3_cross(Vec3 veca, Vec3 vecb) {
	return (Vec3){ .data = {
		VEC_Y(veca) * VEC_Z(vecb) - VEC_Z(veca) * VEC_Y(vecb),
		VEC_Z(veca) * VEC_X(vecb) - VEC_X(veca) * VEC_Z(vecb),
		VEC_X(veca) * VEC_Y(vecb) - VEC_Y(veca) * VEC_X(vecb)
	}};
}


/* RIGHT HANDED COLUMN MAJOR MATRIX */
Vec3 r3_mat4_mul_vec3(Vec3 vec3, Mat4 mat4) {
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

Mat4 r3_mat4_mul_mat4(Mat4 mata, Mat4 matb) {
	Mat4 result = {0};
	FOR(u32, row, 0, 4, 1) {
		FOR(u32, col, 0, 4, 1) {
			FOR_K(0, 4, 1) {
				result.data[col * 4 + row] += mata.data[k * 4 + row] * matb.data[col * 4 + k];
			}
		}
	}
	return result;
}


Mat4 r3_mat4_rotate_x(f32 angle) {
	f32 rad_angle = r3_radians(angle);
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

Mat4 r3_mat4_rotate_y(f32 angle) {
	f32 rad_angle = r3_radians(angle);
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

Mat4 r3_mat4_rotate_z(f32 angle) {
	f32 rad_angle = r3_radians(angle);
	f32 cos_angle = cosf(rad_angle);
	f32 sin_angle = sinf(rad_angle);

	Mat4 result = { .data={
		cos_angle, -sin_angle, 0, 0,
		sin_angle, cos_angle, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	} };

	return result;
}

// axis-angle rotation (Rodrigues rotation formula)
Mat4 r3_mat4_rotate(Vec3 axis, f32 angle) {
	f32 rad_angle = r3_radians(angle);
	f32 cos_angle = cosf(rad_angle);
	f32 sin_angle = sinf(rad_angle);
	Mat4 result = IDENTITY();

	f32 axis_len = r3_vec3_mag(axis);
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

Mat4 r3_mat4_scale(Vec3 scale, Mat4 mat4) {
	Mat4 scale_mat4 = { .data={
		scale.data[0], 0, 0, 0,
		0, scale.data[1], 0, 0,
		0, 0, scale.data[2], 0,
		0, 0, 0, 1.0f
	} };

	return r3_mat4_mul_mat4(scale_mat4, mat4);
}

Mat4 r3_mat4_translate(Vec3 translation, Mat4 mat4) {
	Mat4 trans_mat4 = { .data={
		1.0f, 0, 0, 0,
		0, 1.0f, 0, 0,
		0, 0, 1.0f, 0,
		translation.data[0], translation.data[1], translation.data[2], 1.0f
	} };

	return r3_mat4_mul_mat4(trans_mat4, mat4);
}


Mat4 r3_mat4_lookat(Vec3 eye, Vec3 center, Vec3 up) {
	Vec3 forward_v = r3_vec3_norm(r3_vec3_sub(center, eye));
	Vec3 right_v = r3_vec3_norm(r3_vec3_cross(forward_v, up));
	Vec3 up_v = r3_vec3_norm(r3_vec3_cross(right_v, forward_v));

	return (Mat4){ .data={
		right_v.data[0], up_v.data[0], -forward_v.data[0], 0,
		right_v.data[1], up_v.data[1], -forward_v.data[1], 0,
		right_v.data[2], up_v.data[2], -forward_v.data[2], 0,
		-r3_vec3_dot(right_v, eye), -r3_vec3_dot(up_v, eye), -r3_vec3_dot(forward_v, eye), 1.0f
	} };
}

Mat4 r3_mat4_perspective(f32 fov, f32 aspect, f32 near, f32 far) {
	f32 tan_fov = tanf(r3_radians(fov) / 2.0);
	f32 far_p_near = far + near;
	f32 far_m_near = far - near;
	f32 far_t_near = far * near;

	return (Mat4){ .data={
		1.0f / (aspect * tan_fov), 0, 0, 0,
		0, 1.0f / tan_fov, 0, 0,
		0, 0, -far_p_near / far_m_near, -1.0f,
		0, 0, -(2.0f * far_t_near) / far_m_near, 0
	} };
}

Mat4 r3_mat4_ortho(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far) {
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
