#pragma once

#include <chrono>
#include <random>
#include "include\raylib.h"

const int screenWidth = 2440, screenHeight = 1368;

float RandomFloat(float min, float max, std::mt19937& rng);
int RandomInt(int min, int max, std::mt19937& rng);
bool vectorsEqual(Vector2 v1, Vector2 v2);
float vector2distance(Vector2 v1, Vector2 v2);
float vector2angle(const Vector2& v1, const Vector2& v2);
std::mt19937 CreateGeneratorWithTimeSeed();

extern std::mt19937 rng; // Declare the global variable rng
