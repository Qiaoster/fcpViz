#include <GL/glew.h>
#include "utils.h"

typedef struct {
    float x;
    float y;
    float z;
    float w;
} Vec4;

void
Vec4_Flatten(GLfloat* floats, Vec4* vecs, int vecCount) {
    for (int i = 0; i < vecCount; ++i) {
        floats[i*3 + 0] = vecs[i].x;
        floats[i*3 + 1] = vecs[i].y;
        floats[i*3 + 2] = vecs[i].z;
    }
}

typedef struct {
    Vec4 r0;
    Vec4 r1;
    Vec4 r2;
    Vec4 r3;
} Matrix4x4;

Vec4
Vec4_Init_xyz(float x, float y, float z) {
    Vec4 result;
    result.x = x;
    result.y = y;
    result.z = z;
    result.w = 1.0f;

    return result;
}

Matrix4x4
Matrix4x4_Init(float array[16]) {
    Matrix4x4 result;
    result.r0.x = array[0];
    result.r0.y = array[1];
    result.r0.z = array[2];
    result.r0.w = array[3];
    result.r1.x = array[4];
    result.r1.y = array[5];
    result.r1.z = array[6];
    result.r1.w = array[7];
    result.r2.x = array[8];
    result.r2.y = array[9];
    result.r2.z = array[10];
    result.r2.w = array[11];
    result.r3.x = array[12];
    result.r3.y = array[13];
    result.r3.z = array[14];
    result.r3.w = array[15];

    return result;
}

void
Matrix4x4_Flatten(GLfloat* glfloats[16], Matrix4x4 matrix) {
    *glfloats[0] = matrix.r0.x;
    *glfloats[1] = matrix.r0.y;
    *glfloats[2] = matrix.r0.z;
    *glfloats[3] = matrix.r0.w;
    *glfloats[4] = matrix.r1.x;
    *glfloats[5] = matrix.r1.y;
    *glfloats[6] = matrix.r1.z;
    *glfloats[7] = matrix.r1.w;
    *glfloats[8] = matrix.r2.x;
    *glfloats[9] = matrix.r2.y;
    *glfloats[10] = matrix.r2.z;
    *glfloats[11] = matrix.r2.w;
    *glfloats[12] = matrix.r3.x;
    *glfloats[13] = matrix.r3.y;
    *glfloats[14] = matrix.r3.z;
    *glfloats[15] = matrix.r3.w;
}

Vec4
Vec4_Transform(Matrix4x4 matrix, Vec4 vec) {
    Vec4 result;
    result.x = vec.x * matrix.r0.x + vec.y * matrix.r0.y + vec.z * matrix.r0.z + vec.w * matrix.r0.w;
    result.y = vec.x * matrix.r1.x + vec.y * matrix.r1.y + vec.z * matrix.r1.z + vec.w * matrix.r1.w;
    result.z = vec.x * matrix.r2.x + vec.y * matrix.r2.y + vec.z * matrix.r2.z + vec.w * matrix.r2.w;
    result.w = vec.x * matrix.r3.x + vec.y * matrix.r3.y + vec.z * matrix.r3.z + vec.w * matrix.r3.w;
    return result;
}

Matrix4x4
Matrix4x4_Identity() {
    Matrix4x4 id = Matrix4x4_Init((float[16]){1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1});
    return id;
}

Vec4
Vec4_Scale(Vec4 vec, float scaleFactor) {
    Matrix4x4 scaleMatrix = Matrix4x4_Init(
        (float[16]) {
        scaleFactor, 0, 0, 0,
        0, scaleFactor, 0, 0,
        0, 0, scaleFactor, 0,
        0, 0, 0, 1.0f});

    Vec4 result = Vec4_Transform(scaleMatrix, vec);
    return result;
}

#include <math.h>

Vec4
Vec4_Rotate(Vec4 vec, const char dir, const float rad) {
    float sin = sinf(rad);
    float cos = cosf(rad);
    Matrix4x4 rotation = Matrix4x4_Identity();
    switch (dir) {
        case 'x': {
            rotation = Matrix4x4_Init(
                (float[16]) {
                1, 0,      0, 0,
                0, cos, -sin, 0,
                0, sin,  cos, 0,
                0, 0,      0, 1 });
        } break;
        case 'y': {
            rotation = Matrix4x4_Init(
                (float[16]) {
                 cos, 0, sin, 0,
                   0, 1,   0, 0,
                -sin, 0, cos, 0,
                   0, 0,   0, 1 });
        } break;
        case 'z': {
            rotation = Matrix4x4_Init(
                (float[16]) {
                cos, -sin, 0, 0,
                sin,  cos, 0, 0,
                  0,    0, 1, 0,
                  0,    0, 0, 1 });
        } break;
    }

    Vec4 result = Vec4_Transform(rotation, vec);
    return result;
}
