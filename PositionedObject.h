#ifndef PositionedObject_H
#define PositionedObject_H

#include "raylib.h"

class PositionedObject {
public:
    virtual Vector2 getPosition() const = 0;
};

#endif // PositionedObject_H