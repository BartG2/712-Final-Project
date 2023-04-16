#include <chrono>
#include <random>
#include "raylib.h"
#include <iostream>
#include <vector>
#include <array>
#include <fstream>
#include <cmath>

//---------------------------------------------------------------------------------------------------------------------------------

std::mt19937 CreateGeneratorWithTimeSeed();
float RandomFloat(float min, float max, std::mt19937& rng);
int RandomInt(int min, int max, std::mt19937& rng);

//---------------------------------------------------------------------------------------------------------------------------------


const int screenWidth = 2440, screenHeight = 1368;

std::mt19937 rng = CreateGeneratorWithTimeSeed();

//---------------------------------------------------------------------------------------------------------------------------------

enum CreatureType{
    GenericPrey,
    GenericPredator
};

class Creature{
public:
    CreatureType species;
    double energy;
    int initialEnergy;
    double maxSpeed;
    int initialMaxSpeed;
    float direction;
    Vector2 position;
    Vector2 velocity;
    float sightRange;
    double size;
    double health;
    double initialHealth;
    double energyCost;
    int foodLevel;
    int waterLevel;
    unsigned long age;
    bool alive;

    Creature(CreatureType type, int speed, float range){
        species = type;
        maxSpeed = speed;
        initialMaxSpeed = speed;
        sightRange = range;
        position = {RandomFloat(0, screenWidth, rng), RandomFloat(0, screenHeight, rng)};
        direction = RandomFloat(0, 360, rng);
        health = 100;
        initialHealth = 100;
        size = 1;
        energy = 100000;
        initialEnergy = 100000;
        alive = true;
        age = 0;
    }

    double calculateEnergyCost(double maxSpeed, int sightRange, int size){
        static constexpr double sightCost = 1;
        return 100*size*size*size + maxSpeed*maxSpeed + sightCost*sightRange;
    }

    void move(){
        float newX = position.x + maxSpeed*cos(direction);
        float newY = position.y + maxSpeed*sin(direction);
        if(newX <= screenWidth and newX >= 0 and newY >= 0 and newY <= screenHeight){
            position = {newX, newY};
        }
        else{
            direction -= 180;
        }
    }

    void incrementalRandomWalk(float dv){
        float dx, dy;  

        do
        {
            float dx = RandomFloat(-dv, dv, rng);
            float dy = RandomFloat(-dv, dv, rng);
        } while ((position.x > screenWidth and dx > 0) or (position.x < 0 and dx < 0) or (position.y > screenHeight and dy > 0) or (position.y < screenHeight and dy < 0));
        
        velocity = {dx, dy};
    }

    void shiftDirectionRandomly(float magnitude){
        float d = RandomFloat(-magnitude, magnitude, rng);

        direction += d;
        if(direction > 360){
            direction -=  360;
        }
        else if(direction < 0){
            direction += 360;
        }


    }

    void update(){
        age++;

        /*double ageSpeedDecayFactor = 1 / 1000000.0;
        double speedPeakAge = 500;
        double ds = double(initialMaxSpeed) - ageSpeedDecayFactor*(age-speedPeakAge)*(age-speedPeakAge);
        if(ds >= 0){
            maxSpeed = ds;
        }
        std::cout << age << ", " << maxSpeed << std::endl;*/

        double energyCost = calculateEnergyCost(maxSpeed, sightRange, size);
        if(energy - energyCost > 0){
            energy -= energyCost;
        }
        else{
            die();
        }

        move();
        shiftDirectionRandomly(0.4);
    }

    void die(){
        alive = false;
    }
};

//---------------------------------------------------------------------------------------------------------------------------------

std::mt19937 CreateGeneratorWithTimeSeed() {
    // Get the current time in nanoseconds
    auto now = std::chrono::high_resolution_clock::now();
    auto nanos = std::chrono::time_point_cast<std::chrono::nanoseconds>(now).time_since_epoch().count();

    // Create a new mt19937 generator and seed it with the current time in nanoseconds
    std::mt19937 gen(static_cast<unsigned int>(nanos));
    return gen;
}

float RandomFloat(float min, float max, std::mt19937& rng) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(rng);
}

int RandomInt(int min, int max, std::mt19937& rng){
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}


//---------------------------------------------------------------------------------------------------------------------------------

void initialize(){
    InitWindow(screenWidth, screenHeight, "Predator and Prey Sim");
    SetTargetFPS(100);
}

void drawBackground(){
    ClearBackground(RAYWHITE);
    DrawFPS(screenWidth - 40, 20);
}


void run(){
    initialize();

    std::ofstream outFile;
    outFile.open("data.csv");
    outFile.close();

    for(int frame = 0; !WindowShouldClose(); frame++) {

        BeginDrawing();

        drawBackground();

        EndDrawing();
    }

    CloseWindow();
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

//---------------------------------------------------------------------------------------------------------------------------------

int main() {

    initialize();
    Creature adam(GenericPredator, 1, 1);

    for(int frame = 0; !WindowShouldClose(); frame++){
        //Creature adam(GenericPredator, 0, 1);

        BeginDrawing();

        drawBackground();

        if(adam.alive){
            //DrawRectangleV({adam.position.x - 10, adam.position.y - 20}, {20*float((adam.energy/100000)), 2}, GREEN);
            //DrawRectangleV({adam.position.x - 10 + 20*float((adam.energy/100000)), adam.position.y - 20}, {20*float((100000 - adam.energy)/100000), 2}, RED);
            
            drawHealthBar(adam, 40, 3, 20, GREEN, RED, 1);
            drawHealthBar(adam, 40, 3, 20, GREEN, RED, 2);
            DrawCircleV(adam.position, 10, GREEN);
            adam.update();
        }
        else{
            //std::cout << "dead" << std::endl;
            DrawCircleV(adam.position, 10, RED);
        }

        EndDrawing();
    }

    return 0;
}
