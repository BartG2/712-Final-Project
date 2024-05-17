#include "Creature.h"

unsigned long long Creature::nextID = 0;

Creature::Creature(const Vector2& position, const Species& species) : position(position), species(species), ID(nextID++) {}

Vector2 Creature::getPosition() const { return position; }