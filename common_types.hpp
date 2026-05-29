#pragma once

#include <stdint.h>

typedef struct Vector2
{
    float x;
    float y;
} Vector2;

typedef struct Vector3
{
    float x;
    float y;
    float z;
} Vector3;

struct Vector4 {
    float x, y, z, w;

    // Subtract two vectors
    Vector4 operator-(const Vector4& other) const {
        return Vector4{ x - other.x, y - other.y, z - other.z, 0.0f };
    }

    // Calculate length (magnitude)
    float length() const {
        return sqrtf(x * x + y * y + z * z);
    }

    // Normalize the vector (make it unit length)
    Vector4 normalized() const {
        float len = length();
        if (len > 0.0001f) {
            return Vector4{ x / len, y / len, z / len, 0.0f };
        }
        else {
            return Vector4{ 0,0,0,0 };
        }
    }
};

struct Quaternion {
    float x, y, z, w;

    Quaternion() : x(0), y(0), z(0), w(1) {}
    Quaternion(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}

    // Add quaternion methods here (like normalization, multiplication, etc.)
};
