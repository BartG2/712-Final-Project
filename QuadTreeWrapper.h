#pragma once

#include <chrono>
#include <random>
#include <raylib.h>
#include <iostream>
#include <vector>
#include <array>
#include <fstream>
#include <cmath>
#include <memory>
#include <list>

#include "QuadTree.h"
// Forward declarations
class Creature;
//class QuadTree;

class QuadTreeWrapper {
public:
    QuadTree* quadtree; // Use a pointer to QuadTree instead of the object itself
    std::vector<Creature> creatures;

public:
    QuadTreeWrapper(int depth, const Rectangle& size) {
        quadtree = new QuadTree(depth, size); // Allocate memory for the QuadTree object
    }

    ~QuadTreeWrapper() {
        delete quadtree; // Deallocate memory for the QuadTree object
    }

    void insert(const Creature& newCreature) {
        creatures.push_back(newCreature);
        Creature* creaturePtr = &creatures.back();
        quadtree->insert(creaturePtr); // Use pointer to access member functions
    }

    std::vector<std::reference_wrapper<Creature>> search(Vector2& center, float radius) {
        return quadtree->search(center, radius); // Use pointer to access member functions
    }

    std::vector<Creature*> returnAll(int depth) {
        return quadtree->returnAll(depth); // Use pointer to access member functions
    }

    void clear() {
        creatures.clear();
        quadtree->clear(); // Use pointer to access member functions
    }

    void resize(const Rectangle& newSize) {
        quadtree->resize(newSize); // Use pointer to access member functions
    }

    int size() const {
        return quadtree->size(); // Use pointer to access member functions
    }

    void draw() const {
        quadtree->draw(); // Use pointer to access member functions
    }

    QuadTree& getQuadTree(){
        return *quadtree; // Dereference the pointer to return the QuadTree object
    }
};
