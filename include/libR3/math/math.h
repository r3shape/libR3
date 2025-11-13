#ifndef __R3_MATH_H__
#define __R3_MATH_H__

#include <math.h>
#include <include/libR3/r3def.h>
#include <include/libR3/io/log.h>

#define R3LOGV2(v, msg) r3LogStdOutF(R3_LOG_DEV, "v2 %s |%0.1f, %0.1f|\n", msg, v.data[0], v.data[1])
#define R3LOGV3(v, msg) r3LogStdOutF(R3_LOG_DEV, "v3 %s |%0.1f, %0.1f, %0.1f|\n", msg, v.data[0], v.data[1], v.data[2])
#define R3LOGV4(v, msg) r3LogStdOutF(R3_LOG_DEV, "v4 %s |%0.1f, %0.1f, %0.1f, %0.1f|\n", msg, v.data[0], v.data[1], v.data[2], v.data[3])

#define R3LOGM4(m, msg) r3LogStdOutF(R3_LOG_DEV, "m4 %s \n|%0.1f, %0.1f, %0.1f, %0.1f|\n|%0.1f, %0.1f, %0.1f, %0.1f|\n|%0.1f, %0.1f, %0.1f, %0.1f|\n|%0.1f, %0.1f, %0.1f, %0.1f|\n", msg, m.data[0], m.data[1], m.data[2], m.data[3], m.data[4], m.data[5], m.data[6], m.data[7], m.data[8], m.data[9], m.data[10], m.data[11], m.data[12], m.data[13], m.data[14], m.data[15])


/*	TYPES	*/
typedef struct Rect { f32 data[4]; } Rect;
typedef struct AABB { f32 data[4]; } AABB;

typedef struct Vec2 { f32 data[2]; } Vec2;
typedef struct Vec3 { f32 data[3]; } Vec3;
typedef struct Vec4 { f32 data[4]; } Vec4;

#define VEC2(x, y) ((Vec2){ .data = {x, y} })
#define VEC3(x, y, z) ((Vec3){ .data = {x, y, z} })
#define VEC4(x, y, z, w) ((Vec4){ .data = {x, y, z, w} })

typedef struct Mat4 { f32 data[16]; } Mat4;

#define IDENTITY() ((Mat4){ .data = {\
	1.0, 0.0, 0.0, 0.0,\
	0.0, 1.0, 0.0, 0.0,\
	0.0, 0.0, 1.0, 0.0,\
	0.0, 0.0, 0.0, 1.0}\
})

// ACCESSORS
#define VEC_X(v) ((v).data[0])
#define VEC_Y(v) ((v).data[1])
#define VEC_Z(v) ((v).data[2])
#define VEC_W(v) ((v).data[3])

#define RECT_X(r) ((r).data[0])
#define RECT_Y(r) ((r).data[1])
#define RECT_W(r) ((r).data[2])
#define RECT_H(r) ((r).data[3])

#define AABB_X(a) ((a).data[0])
#define AABB_Y(a) ((a).data[1])
#define AABB_W(a) ((a).data[2])
#define AABB_H(a) ((a).data[3])

#define MAT4_RC(m, r, c) ((m).data[(r) * 4 + (c)])	// row-col
#define MAT4_CR(m, c, r) ((m).data[(c) * 4 + (r)])	// col-row

// ROW-MAJOR MATRIX AXIS
#define MAT4_RM(vx, vy, vz) (Mat4){ .data = { \
    vx.data[0], vx.data[1], vx.data[2], 0,    \
    vy.data[0], vy.data[1], vy.data[2], 0,    \
    vz.data[0], vz.data[1], vz.data[2], 0,    \
    0, 0, 0, 1                                \
}}

#define MAT4_X_RM(m) (Vec3){m.data[0], m.data[1], m.data[2]}
#define MAT4_Y_RM(m) (Vec3){m.data[4], m.data[5], m.data[6]}
#define MAT4_Z_RM(m) (Vec3){m.data[8], m.data[9], m.data[10]}

// COLUMN-MAJOR MATRIX AXIS
#define MAT4_CM(vx, vy, vz) (Mat4){ .data = { \
    vx.data[0], vy.data[0], vz.data[0], 0,    \
    vx.data[1], vy.data[1], vz.data[1], 0,    \
    vx.data[2], vy.data[2], vz.data[2], 0,    \
    0, 0, 0, 1                                \
}}

#define RADIANS(degrees) degrees * (PI / 180.0)
#define DEGREES(radians) radians * (180.0 / PI)

#define MAT4_X_CM(m) (Vec3){m.data[0], m.data[4], m.data[8]}
#define MAT4_Y_CM(m) (Vec3){m.data[1], m.data[5], m.data[9]}
#define MAT4_Z_CM(m) (Vec3){m.data[2], m.data[6], m.data[10]}

/*	API	*/
R3_PUBLIC_API f32 r3Vec2Mag(Vec2 vec2);
R3_PUBLIC_API f32 r3Vec3Mag(Vec3 vec3);
R3_PUBLIC_API f32 r3Vec4Mag(Vec4 vec4);


R3_PUBLIC_API Vec2 r3Vec2Norm(Vec2 vec2);
R3_PUBLIC_API Vec3 r3Vec3Norm(Vec3 vec3);
R3_PUBLIC_API Vec4 r3Vec4Norm(Vec4 vec4);


R3_PUBLIC_API f32 r3Vec2Dot(Vec2 veca, Vec2 vecb);
R3_PUBLIC_API f32 r3Vec3Dot(Vec3 veca, Vec3 vecb);
R3_PUBLIC_API f32 r3Vec4Dot(Vec4 veca, Vec4 vecb);


R3_PUBLIC_API Vec2 r3Vec2Add(Vec2 veca, Vec2 vecb);
R3_PUBLIC_API Vec3 r3Vec3Add(Vec3 veca, Vec3 vecb);
R3_PUBLIC_API Vec4 r3Vec4Add(Vec4 veca, Vec4 vecb);


R3_PUBLIC_API Vec2 r3Vec2Sub(Vec2 veca, Vec2 vecb);
R3_PUBLIC_API Vec3 r3Vec3Sub(Vec3 veca, Vec3 vecb);
R3_PUBLIC_API Vec4 r3Vec4Sub(Vec4 veca, Vec4 vecb);


R3_PUBLIC_API Vec2 r3Vec2Mul(Vec2 veca, Vec2 vecb);
R3_PUBLIC_API Vec3 r3Vec3Mul(Vec3 veca, Vec3 vecb);
R3_PUBLIC_API Vec4 r3Vec4Mul(Vec4 veca, Vec4 vecb);


R3_PUBLIC_API Vec2 r3Vec2Div(Vec2 veca, Vec2 vecb);
R3_PUBLIC_API Vec3 r3Vec3Div(Vec3 veca, Vec3 vecb);
R3_PUBLIC_API Vec4 r3Vec4Div(Vec4 veca, Vec4 vecb);


R3_PUBLIC_API Vec2 r3Vec2Scale(f32 scale, Vec2 vec2);
R3_PUBLIC_API Vec3 r3Vec3Scale(f32 scale, Vec3 vec3);
R3_PUBLIC_API Vec4 r3Vec4Scale(f32 scale, Vec4 vec4);


R3_PUBLIC_API Vec3 r3Vec3Cross(Vec3 veca, Vec3 vecb);

R3_PUBLIC_API Vec3 r3Mat4MulVec3(Vec3 vec3, Mat4 mat4);
R3_PUBLIC_API Mat4 r3Mat4MulMat4(Mat4 mata, Mat4 matb);

R3_PUBLIC_API Mat4 r3Mat4RotateX(f32 angle);
R3_PUBLIC_API Mat4 r3Mat4RotateY(f32 angle);
R3_PUBLIC_API Mat4 r3Mat4RotateZ(f32 angle);
R3_PUBLIC_API Mat4 r3Mat4Rotate(Vec3 axis, f32 angle);
R3_PUBLIC_API Mat4 r3Mat4Scale(Vec3 scale, Mat4 mat4);
R3_PUBLIC_API Mat4 r3Mat4Translate(Vec3 translation, Mat4 mat4);

R3_PUBLIC_API Mat4 r3Mat4Transpose(Mat4 mat4);

R3_PUBLIC_API Mat4 r3Mat4Lookat(Vec3 eye, Vec3 center, Vec3 up);
R3_PUBLIC_API Mat4 r3Mat4Perspective(f32 fov, f32 aspect, f32 near, f32 far);
R3_PUBLIC_API Mat4 r3Mat4Ortho(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far);

#endif // __R3_MATH_H__