#pragma once

#include <cmath>

struct Vec3
{
    float x;
    float y;
    float z;
};

inline Vec3
add(const Vec3& a,
    const Vec3& b)
{
    return { a.x + b.x, a.y + b.y, a.z + b.z };
}

inline Vec3
scale(const Vec3& v,
      float factor)
{
    return { v.x * factor, v.y * factor, v.z * factor };
}

inline Vec3
normalize(const Vec3& v)
{
    float length = std::sqrt(
        v.x * v.x + v.y * v.y + v.z * v.z
    );

    if (length == 0.0f)
        return { 0.0f, 0.0f, 0.0f };

    return { v.x / length, v.y / length, v.z / length };
}

inline float
dot(const Vec3& a, const Vec3& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline Vec3
cross(const Vec3& a, const Vec3& b)
{
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}
struct Mat4
{
    float m[16];

    Mat4()
    {
        for (int i = 0; i < 16; ++i)
            m[i] = 0.0f;
    };

    static Mat4
    identity()
    {
        Mat4 ret{};
        ret.m[0] = ret.m[5] = ret.m[10] = ret.m[15] = 1.0f;
        return ret;
    }

    static Mat4
    scale(float scale)
    {
        Mat4 ret = identity();
        ret.m[0] = ret.m[5] = ret.m[10] = scale;
        return ret;
    }
    static Mat4
    scale(float x,
          float y,
          float z)
    {
        Mat4 ret = identity();
        ret.m[0] = x;
        ret.m[5] = y;
        ret.m[10] = z;
        return ret;
    }

    static Mat4
    multiply(const Mat4 &a,
             const Mat4 &b)
    {
        Mat4 result{};
        for (int row = 0; row < 4; ++row)
            for (int col = 0; col < 4; ++col)
                for (int k = 0; k < 4; ++k)
                    result.m[col*4 + row] += a.m[k*4 + row]*b.m[col*4 + k];
        return result;
    }

    static Mat4
    perspective(float fov,
                float aspectRatio,
                float near,
                float far)
    {
        Mat4 result{};
        float f = 1.0f / tan(fov / 2.0f);
        result.m[0] = f / aspectRatio;
        result.m[5] = f;
        result.m[10] = (far + near) / (near - far);
        result.m[11] = -1.0f;
        result.m[14] = (2.0f * far * near) / (near - far);
        return result;
    }

    static Mat4
    translate(float tx, float ty, float tz)
    {
        Mat4 ret = identity();
        ret.m[12] = tx;
        ret.m[13] = ty;
        ret.m[14] = tz;
        return ret;
    }

    static Mat4 viewFromDirection(
        const Vec3& position,
        const Vec3& front,
        const Vec3& worldUp)
    {
        Vec3 forward = normalize(front);
        Vec3 right = normalize(cross(forward, worldUp));
        Vec3 up = cross(right, forward);
        Vec3 back = ::scale(forward, -1.0f);

        Mat4 result = identity();

        result.m[0] = right.x;
        result.m[4] = right.y;
        result.m[8] = right.z;
        result.m[12] = -dot(right, position);

        result.m[1] = up.x;
        result.m[5] = up.y;
        result.m[9] = up.z;
        result.m[13] = -dot(up, position);

        result.m[2] = back.x;
        result.m[6] = back.y;
        result.m[10] = back.z;
        result.m[14] = -dot(back, position);

        return result;

    }
    static Mat4
    rotateZ(float angle)
    {
		Mat4 ret = identity();
        ret.m[0] = cos(angle);
        ret.m[1] = sin(angle);
        ret.m[4] = -sin(angle);
        ret.m[5] = cos(angle);
        return ret;
    }
    static Mat4
    rotateY(float angle)
    {
		Mat4 ret = identity();
        ret.m[0] = cos(angle);
        ret.m[2] = -sin(angle);
        ret.m[8] = sin(angle);
        ret.m[10] = cos(angle);
        return ret;
    }
    static Mat4
    rotateX(float angle)
    {
		Mat4 ret = identity();
        ret.m[5] = cos(angle);
        ret.m[6] = sin(angle);
        ret.m[9] = -sin(angle);
        ret.m[10] = cos(angle);
        return ret;
    }
};
