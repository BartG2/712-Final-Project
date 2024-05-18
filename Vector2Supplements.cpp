#include "Vector2Supplements.h"

#include "raylib.h"
#include "cmath"

Vector2 Vector2Add(const Vector2& v1, const Vector2& v2) {
    return {v1.x + v2.x, v1.y + v2.y};
}

float Vector2Length(const Vector2& v) {
    return std::sqrt(v.x * v.x + v.y * v.y);
}

Vector2 Vector2Scale(const Vector2& v, float scalar) {
    return {v.x * scalar, v.y * scalar};
}

Vector2 Vector2Normalize(const Vector2& v) {
    float length = Vector2Length(v);
    return {v.x / length, v.y / length};
}

float Vector2Distance(Vector2 v1, Vector2 v2) {
    float dx = v2.x - v1.x;
    float dy = v2.y - v1.y;
    return std::sqrt(dx * dx + dy * dy);
}