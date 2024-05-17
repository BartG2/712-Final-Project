#ifndef Creature_H
#define Creature_H

#include "raylib.h"
#include "PositionedObject.h"

enum Species {
    Generic_Predator,
    Generic_Prey,
    Ant
};

class Creature : public PositionedObject {
public:
    Creature(const Vector2&, const Species&);
    Vector2 getPosition() const override;

private:
    Vector2 position;
    unsigned long long ID;
    static unsigned long long nextID;
    Species species;
};

#endif // PositionedObject_H