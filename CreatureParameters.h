#pragma once

#include "CreatureType.h"
#include "include\raylib.h"

class Creature;
class QuadTree;
class QuadTreeWrapper;

struct CreatureParameters {
    CreatureType species = CreatureType::GenericPrey;
    double maxSpeed = 1.0;
    float sightRange = 10.0f;
    Vector2 position = {100, 100};
    double direction = 0.0;
    double health = 100;
    double size = 5;
    double energy = 100000;
    double attackDamage = 20;
    //double attackCooldownLength = 1;
    int attackCooldownLength = 1;
    int reproductionCooldownLength = 10;
};
