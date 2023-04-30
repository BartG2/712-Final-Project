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

#include "Functions.h"
#include "Creature.h"
#include "CreatureParameters.h"
#include "QuadTree.h"
#include "CreatureType.h"
#include "QuadTreeWrapper.h"

class Particle{
public:
    Vector2 pos;
    Color color;

    Particle(Vector2 position, Color col){
        pos = position;
        color = col;
    }
};

void initialize(){
    InitWindow(screenWidth, screenHeight, "Predator and Prey Sim");
    SetTargetFPS(100);
}

void drawBackground(){
    ClearBackground(RAYWHITE);
    DrawFPS(screenWidth - 40, 20);
}

void drawHealthBar(Creature& creature, int barWidth, int barHeight, int verticalOffset, Color fullColor, Color emptyColor, int barType){
    double interbarDistance = 8;

    //energy bar
    if(barType == 1){
        DrawRectangle(creature.position.x - barWidth / 2, creature.position.y - verticalOffset, barWidth*(creature.energy/creature.initialEnergy), barHeight, fullColor);
        DrawRectangle(creature.position.x - barWidth / 2 + barWidth*(creature.energy/creature.initialEnergy), creature.position.y - verticalOffset, barWidth*(creature.initialEnergy - creature.energy) / creature.initialEnergy, barHeight,  emptyColor);
    }
    //health bar
    else if(barType == 2){
        DrawRectangle(creature.position.x - barWidth / 2, creature.position.y - verticalOffset - interbarDistance, barWidth*(creature.health/creature.initialHealth), barHeight, fullColor);
        DrawRectangle(creature.position.x - barWidth / 2 + barWidth*(creature.health/creature.initialHealth), creature.position.y - verticalOffset - interbarDistance, barWidth*(creature.initialHealth - creature.health) / creature.initialHealth, barHeight,  emptyColor);
    }
}

QuadTree initializeQT(std::vector<Creature>& predators, std::vector<Creature>& prey){
    QuadTree qt(0, Rectangle{0, 0, screenWidth, screenHeight});
    Color predColor = RED, preyColor = GREEN;

    for(auto& p : predators){
        qt.insert(&p);
    }

    for(auto& p : prey){
        qt.insert(&p);
    }

    return qt;
}

void qtCollisionCheck(std::vector<Creature>& predators, std::vector<Creature>& prey, QuadTreeWrapper& quadTreeWrapper) {

    std::vector<std::reference_wrapper<Creature>> qtPredators;
    std::vector<std::reference_wrapper<Creature>> qtPrey;
    double pr = 0.8;

    for (auto& p : quadTreeWrapper.creatures) {
        if (p.species == CreatureType::GenericPredator) {
            qtPredators.push_back(p);
        } else if (p.species == CreatureType::GenericPrey) {
            qtPrey.push_back(p);
        }
    }


    // Search for prey within predator's detection radius
    for (Creature& predator : qtPredators) {

        Vector2 center = predator.position;
        float radius = predator.size*2 + 10;
        std::vector<std::reference_wrapper<Creature>> nearbyCreatures = quadTreeWrapper.search(center, radius);

        // Iterate through nearby creatures and check for collisions
        for (Creature& creature : nearbyCreatures) {
            if (creature.species == CreatureType::GenericPrey && CheckCollisionCircles(center, predator.size, creature.position, creature.size)) {
                
                // pred-prey collision
                if(predator.canAttack()){
                    creature.health -= predator.attackDamage;
                    predator.attackCooldownTimer = 0;
                    auto temp = creature.energy;
                    predator.energy += creature.energy * predator.attackDamage / creature.initialHealth;
                    creature.energy -=  temp * predator.attackDamage / creature.initialHealth;
                }

                if(creature.health <= 0){
                    predator.energy += creature.energy;
                    creature.die();
                }
            }

            if(creature.species == CreatureType::GenericPredator and CheckCollisionCircles(center, predator.size, creature.position, creature.size) and predator.id != creature.id){

                //pred-pred collision
                if(predator.energy >= predator.initialEnergy * pr and creature.energy >= creature.initialEnergy * pr and predator.canReproduce() and creature.canReproduce()){
                    predator.reproduceS(quadTreeWrapper.creatures, creature);
                    predator.reproductionTimer = 0;
                    creature.reproductionTimer = 0;
                    predator.energy /= 2;
                }
            }
        }
    }

    for(Creature& p1: qtPrey){
        Vector2& center = p1.position;
        float radius = p1.size*2 + 10;
        std::vector<std::reference_wrapper<Creature>> nearbyCreatures = quadTreeWrapper.search(center, radius);

        for(Creature& p2 : nearbyCreatures){
            if (p2.species == CreatureType::GenericPrey && CheckCollisionCircles(center, p1.size, p2.position, p2.size) and p1.id != p2.id){

                if(p1.energy >= p1.initialEnergy * pr and p2.energy >= p2.initialEnergy * pr and p1.canReproduce() and p2.canReproduce()){
                    p1.reproduceS(quadTreeWrapper.creatures, p2);
                    p1.reproductionTimer = 0;
                    p2.reproductionTimer = 0;
                    p1.energy /= 2;
                }
            }
        }
    }

    predators.clear();
    prey.clear();

    for(int i = 0; i < quadTreeWrapper.creatures.size(); i++){
        if(quadTreeWrapper.creatures[i].species == CreatureType::GenericPredator){
            predators.push_back(quadTreeWrapper.creatures[i]);
        }
        else if(quadTreeWrapper.creatures[i].species == CreatureType::GenericPrey){
            prey.push_back(quadTreeWrapper.creatures[i]);
        }
    }

    for(int i = 0; i < prey.size(); i++){
        if(!prey[i].alive){
            prey.erase(prey.begin() + i);
        }
    }
}

int main() {
    initialize();

    int numPredators = 20, numPrey = 40;
    Color predColor = RED, preyColor = GREEN;

    std::vector<Creature> predators(numPredators);
    std::vector<Creature> prey(numPrey);

    CreatureParameters initialPredatorP;
    initialPredatorP.species = CreatureType::GenericPredator;
    initialPredatorP.size = 5;
    initialPredatorP.attackDamage = 50;
    initialPredatorP.energy = 100000;
    initialPredatorP.maxSpeed = 1;
    initialPredatorP.sightRange = 10;
    initialPredatorP.attackCooldownLength = 50;
    initialPredatorP.reproductionCooldownLength = 500;

    CreatureParameters initialPreyP;
    initialPreyP.species = CreatureType::GenericPrey;
    initialPreyP.size = 17;
    initialPreyP.attackDamage = 0;
    initialPreyP.energy = 100000;
    initialPreyP.maxSpeed = 1.1;
    initialPreyP.sightRange = 1;
    initialPreyP.health = 100;
    initialPreyP.attackCooldownLength = 100;
    initialPreyP.sightRange = 10;

    for(int i = 0; i < numPredators; i++){
        initialPredatorP.position = {RandomFloat(0, screenWidth, rng), RandomFloat(0, screenHeight, rng)};
        initialPredatorP.direction = RandomFloat(0, 360, rng); // Set a random direction
        predators[i] = Creature(initialPredatorP);
    }
    for(int i = 0; i < numPrey; i++){
        initialPreyP.position = {RandomFloat(0, screenWidth, rng), RandomFloat(0, screenHeight, rng)};
        initialPreyP.direction = RandomFloat(0, 360, rng); // Set a random direction
        prey[i] = Creature(initialPreyP);
    }

    QuadTree qt(0, {0, 0, screenWidth, screenHeight});

    for(int i = 0; i < numPredators; i++){
        qt.insert(&predators[i]);
    }

    for(int i = 0; i < numPrey; i++){
        qt.insert(&prey[i]);
    }






    for(int frame = 0; !WindowShouldClose(); frame++){

        QuadTreeWrapper qtw(0, {0, 0, screenWidth, screenHeight});
        for(int i = 0; i < predators.size(); i++){
            qtw.insert(predators[i]);
        }
        for(int i = 0; i < prey.size(); i++){
            qtw.insert(prey[i]);
        }
        qtCollisionCheck(predators, prey, qtw);

        BeginDrawing();
        drawBackground();

        // Draw predators
        for(int i = 0 ; i < predators.size(); i++){
            drawHealthBar(predators[i], 40, 3, 4+ predators[i].size, GREEN, RED, 1);
            drawHealthBar(predators[i], 40, 3, 4+ predators[i].size, BLUE, ORANGE, 2);
            DrawCircleV(predators[i].position, predators[i].size, predColor);
        }

        // Draw prey
        for(int i = 0; i < prey.size(); i++){
            drawHealthBar(prey[i], 40, 3, 4+prey[i].size, GREEN, RED, 1);
            drawHealthBar(prey[i], 40, 3, 4+prey[i].size, BLUE, ORANGE, 2);
            DrawCircleV(prey[i].position, prey[i].size, preyColor);
        }

        EndDrawing();

        // Update predators
        for(int i = 0; i < predators.size(); i++){
            if(!predators[i].alive){
                predators.erase(predators.begin() + i);
                i--;
                continue;
            }
            predators[i].update(0.0001, 10, frame, qtw);
            predators[i].shiftDirectionRandomly(0.5);
        }

        // Update prey
        for(int i = 0; i < prey.size(); i++){
            if(!prey[i].alive){
                prey.erase(prey.begin() + i);
                i--;
                continue;
            }
            prey[i].update(0.0001, 10, frame, qtw);
            prey[i].shiftDirectionRandomly(0.5);
        }

        if(frame % 100 == 0){
            std::cout << "Predators: " << predators.size() << ", Prey: " << prey.size() << ", total: " << predators.size() + prey.size() << std::endl;
            //prey[RandomInt(0, prey.size(), rng)].reproduceA(prey);
        }


    }
    return 0;
}