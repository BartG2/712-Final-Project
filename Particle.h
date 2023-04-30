#pragma once

#include "include\raylib.h"

class Particle{
public:
    Vector2 pos;
    Color color;

    Particle(Vector2 position, Color col){
        pos = position;
        color = col;
    }
};