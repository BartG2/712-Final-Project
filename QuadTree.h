#pragma once

#include <chrono>
#include <random>
#include "include\raylib.h"
#include <iostream>
#include <vector>
#include <array>
#include <fstream>
#include <cmath>
#include <memory>
#include <list>

class Creature;
//#include "Creature.h"

#include "CreatureType.h"
#include "CreatureParameters.h"
#include "QuadTree.h"
#include "Functions.h"

class QuadTreeWrapper;

struct QuadTreeParameters;

class QuadTree {
public:
    int currentDepth;
    Rectangle currentSize;
    std::vector<Creature*> creatures; // Store pointers to Creature objects
    std::array<std::shared_ptr<QuadTree>, 4> children{};
    std::array<Rectangle, 4> childAreas{};

    QuadTree(const int setDepth, const Rectangle& setSize) {
        currentDepth = setDepth;
        resize(setSize);
    }

    void resize(const Rectangle& setSize);

    void clear();

    void insert(Creature* newCreature);

    std::vector<std::reference_wrapper<Creature>> search(Vector2& center, float radius);
    
    std::vector<Creature*> returnAll(int depth);

    int size() const;

    void draw() const;
};
