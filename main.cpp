#include <iostream>
#include <list>

#include "raylib.h"
#include "Creature.h"
#include "/home/gazda/Code/Optimized_DLA/Quadtree.h"

//---------------------------------------------------------------------------------------------------------------------------------------------

void InitGame();
void UpdateLogic();
void DrawGame(const std::list<Creature>&);
void drawCreature(const Creature&);

//---------------------------------------------------------------------------------------------------------------------------------------------

const static int screenWidth = 3440;
const static int screenHeight = 1440;

//---------------------------------------------------------------------------------------------------------------------------------------------

int main(){
    using std::list;

    InitGame();

    Creature testSubject({100, 100}, Species::Ant);
    Creature testSubject2({200, 200}, Species::Generic_Prey);
    Creature testSubject3({300, 300}, Species::Generic_Predator);
    list<Creature> creatures;
    creatures.push_back(testSubject);
    creatures.push_back(testSubject2);
    creatures.push_back(testSubject3);


    // main loop
    while(!WindowShouldClose()){
        UpdateLogic();
        DrawGame(creatures);
    }

    CloseWindow();

    return 0;
}

void InitGame(){
    InitWindow(screenWidth, screenHeight, "Creature Simulation");
    ToggleFullscreen();               // Only works with hardcoded screen dimensions, not with GetScreenWidth() and GetScreenHeight() or getMonitorWidth() and getMonitorHeight()
}

void UpdateLogic(){

}

void DrawGame(const std::list<Creature>& creatures){
    BeginDrawing();

    ClearBackground(RAYWHITE);

    for(const auto& creature : creatures){
        drawCreature(creature);
    }

    DrawFPS(screenWidth-100, 40);

    EndDrawing();
}

void drawCreature(const Creature& creature){
    DrawCircleV(creature.getPosition(), 5, BLUE);          // default radius and color, change laters
}