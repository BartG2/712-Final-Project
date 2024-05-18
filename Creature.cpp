#include "Creature.h"
#include "Vector2Supplements.h"

unsigned long long Creature::nextID = 0;

Creature::Creature(const Vector2& position, const Species& species) : position(position), velocity({0, 0}), maxSpeed(2), acceleration({0, 0}),
    maxForce(0.1), health(100), energy(100), ID(nextID++), species(species), brain({400, 40, 20, 16}) {}

Vector2 Creature::getPosition() const { return position; }

bool Creature::updateMovement(float deltaTime) {
    velocity = Vector2Add(velocity, Vector2Scale(acceleration, deltaTime));
    position = Vector2Add(position, velocity);

    // Limit the speed
    float speed = Vector2Length(velocity);
    if (speed > maxSpeed) {
        velocity = Vector2Scale(Vector2Normalize(velocity), maxSpeed);
    }

    // Decrease the acceleration
    acceleration = Vector2Scale(acceleration, 0.9);

    // Update the direction
    direction = atan2(velocity.y, velocity.x);

    return true;
}

bool Creature::updateEnergy(float deltaTime) {
    // Calculate the energy cost of moving
    float movementCost = 0.001f * size * Vector2Length(velocity) * deltaTime; // Larger creatures pay more for moving
    // Add any other energy costs (e.g., maintaining body temperature, neural activity)

    // Subtract the energy cost from the creature's energy reserves
    energy -= movementCost;

    // Check if the creature has enough energy to survive
    if (energy < 0) {
        // If not, reduce its health and potentially kill it
        health -= -energy; // Convert negative energy to health damage
        energy = 0; // Reset energy to 0
        if (health <= 0) return false; // Creature died
    }

    return true; // Creature survived
}

void Creature::setPosition(const Vector2& position) { this->position = position; }
void Creature::setVelocity(const Vector2& velocity) { this->velocity = velocity; }
void Creature::setAcceleration(const Vector2& acceleration) { this->acceleration = acceleration; }


Vector2 Creature::getVelocity() const { return velocity; }
Vector2 Creature::getAcceleration() const { return acceleration; }
