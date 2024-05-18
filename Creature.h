#ifndef Creature_H
#define Creature_H

#include "raylib.h"
#include "PositionedObject.h"
#include "NeuralNetwork.h"
#include <vector>
#include <array>

enum Species {
    Generic_Predator,
    Generic_Prey,
    Ant
};

class Creature : public PositionedObject {
public:

    bool selected = false;

    // Constructors
    Creature(const Vector2&, const Species&);

    // Accessors
    Vector2 getPosition() const override;
    Vector2 getVelocity() const;
    Vector2 getAcceleration() const;

    //Mutators
    bool updateAll();
    bool updateMovement(float);
    bool updateEnergy(float);
    bool updateSight();
    //bool updateSound();

    void setPosition(const Vector2&);
    void setVelocity(const Vector2&);
    void setAcceleration(const Vector2&);

private:
    // movement
    Vector2 position;
    Vector2 velocity;
    Vector2 acceleration;
    float direction;
    float maxSpeed;
    float maxForce;

    // stats
    float health;
    float energy;
    float attackDamage;
    float size;

    bool isAlive = true;
    

    // identification
    unsigned int age;
    unsigned long long ID;
    static unsigned long long nextID;
    Species species;

    // brain
    NeuralNetwork brain;
    std::vector<Ray> rays;
    //std::array<int, 64> sound;
};

#endif // PositionedObject_H