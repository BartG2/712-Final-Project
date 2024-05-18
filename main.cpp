#include <iostream>
#include <list>

#include "raylib.h"
#include "Creature.h"
#include "/home/gazda/Code/Optimized_DLA/Quadtree.h"
#include "Vector2Supplements.h"

//---------------------------------------------------------------------------------------------------------------------------------------------

void InitGame();
void UpdateLogic(std::list<Creature>& creatures);
void DrawGame(std::list<Creature>&);
void drawCreature(const Creature&);

//---------------------------------------------------------------------------------------------------------------------------------------------

const static int screenWidth = 3440;
const static int screenHeight = 1440;

//---------------------------------------------------------------------------------------------------------------------------------------------

int main(){
    using namespace std;

    InitGame();

    Creature testSubject({100, 100}, Species::Ant);
    Creature testSubject2({200, 200}, Species::Generic_Prey);
    Creature testSubject3({300, 300}, Species::Generic_Predator);
    list<Creature> creatures;
    creatures.push_back(testSubject);
    creatures.push_back(testSubject2);
    creatures.push_back(testSubject3);

    vector<double> randomNumbers;
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<double> dis(0.0, 1.0);
    for (int i = 0; i < 400; ++i) {
        randomNumbers.push_back(dis(gen));
    }

    // main loop
    while(!WindowShouldClose()){
        UpdateLogic(creatures);
        DrawGame(creatures);
    }
    CloseWindow();

    return 0;
}

void InitGame(){
    SetTargetFPS(100);
    InitWindow(screenWidth, screenHeight, "Creature Simulation");
    ToggleFullscreen();               // Only works with hardcoded screen dimensions, not with GetScreenWidth() and GetScreenHeight() or getMonitorWidth() and getMonitorHeight()
}

void UpdateLogic(std::list<Creature>& creatures){
    float deltaTime = GetFrameTime();
    for(auto& creature : creatures){
        creature.updateMovement(deltaTime);
        creature.updateEnergy(deltaTime);
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mousePosition = GetMousePosition();
        for (auto& creature : creatures) {
            float distance = Vector2Distance(creature.getPosition(), mousePosition);
            if (distance < 10) { // adjust the distance as needed
                creature.selected = !creature.selected; // toggle selection
            } else {
                creature.selected = false; // unselect all other creatures
            }
        }
    }

    for (auto& creature : creatures) {
        if (creature.selected) {
            if (IsKeyDown(KEY_W)) {
                Vector2 currentAcceleration = creature.getAcceleration();
                currentAcceleration.y -= 0.1;
                creature.setAcceleration(currentAcceleration);
            }
            if (IsKeyDown(KEY_S)) {
                Vector2 currentAcceleration = creature.getAcceleration();
                currentAcceleration.y += 0.1;
                creature.setAcceleration(currentAcceleration);
            }
            if (IsKeyDown(KEY_A)) {
                Vector2 currentAcceleration = creature.getAcceleration();
                currentAcceleration.x -= 0.1;
                creature.setAcceleration(currentAcceleration);
            }
            if (IsKeyDown(KEY_D)) {
                Vector2 currentAcceleration = creature.getAcceleration();
                currentAcceleration.x += 0.1;
                creature.setAcceleration(currentAcceleration);
            }
        }
    }
}

void DrawGame(std::list<Creature>& creatures){
    BeginDrawing();

    ClearBackground(RAYWHITE);

    for(auto& creature : creatures){
        drawCreature(creature);
    }

    DrawFPS(screenWidth-100, 40);

    EndDrawing();
}

void drawCreature(const Creature& creature){
    Color color = creature.selected ? RED : BLUE;
    DrawCircleV(creature.getPosition(), 5, color); // change color based on selection
}