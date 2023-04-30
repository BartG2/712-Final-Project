#include <chrono>
#include <random>
#include "include\raylib.h"


const int screenWidth = 2440, screenHeight = 1368, maxTreeDepth = 5;

float RandomFloat(float min, float max, std::mt19937& rng) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(rng);
}

int RandomInt(int min, int max, std::mt19937& rng){
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

bool vectorsEqual(Vector2 v1, Vector2 v2){
    if(v1.x == v2.x && v1.y == v2.y){
        return true;
    }
    else{
        return false;
    }
}

float vector2distance(Vector2 v1, Vector2 v2) {
    float dx = v2.x - v1.x;
    float dy = v2.y - v1.y;
    return std::sqrt(dx * dx + dy * dy);
}

float vector2angle(const Vector2& v1, const Vector2& v2) {
    float deltaX = v2.x - v1.x;
    float deltaY = v2.y - v1.y;

    float angleRadians = atan2(deltaY, deltaX);
    float angleDegrees = angleRadians * (180.0 / M_PI);

    if (angleDegrees < 0) {
        angleDegrees += 360;
    }

    return angleDegrees;
}

std::mt19937 CreateGeneratorWithTimeSeed() {
    // Get the current time in nanoseconds
    auto now = std::chrono::high_resolution_clock::now();
    auto nanos = std::chrono::time_point_cast<std::chrono::nanoseconds>(now).time_since_epoch().count();

    // Create a new mt19937 generator and seed it with the current time in nanoseconds
    std::mt19937 gen(static_cast<unsigned int>(nanos));
    return gen;
}

std::mt19937 rng = CreateGeneratorWithTimeSeed();

