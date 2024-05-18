#ifndef VECTOR2_SUPPLEMENTS_H
#define VECTOR2_SUPPLEMENTS_H

#include "raylib.h"

Vector2 Vector2Add(const Vector2&, const Vector2&);
float Vector2Length(const Vector2&);
Vector2 Vector2Scale(const Vector2&, float);
Vector2 Vector2Normalize(const Vector2&);
float Vector2Distance(Vector2, Vector2);

#endif // VECTOR2_SUPPLEMENTS_H