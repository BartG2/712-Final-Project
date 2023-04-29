#include <chrono>
#include <random>
#include "raylib.h"
#include <iostream>
#include <vector>
#include <array>
#include <fstream>
#include <cmath>
#include <memory>
#include <list>

//---------------------------------------------------------------------------------------------------------------------------------

std::mt19937 CreateGeneratorWithTimeSeed();
float RandomFloat(float min, float max, std::mt19937& rng);
int RandomInt(int min, int max, std::mt19937& rng);

//---------------------------------------------------------------------------------------------------------------------------------


const int screenWidth = 2440, screenHeight = 1368, maxTreeDepth = 5;

std::mt19937 rng = CreateGeneratorWithTimeSeed();

//---------------------------------------------------------------------------------------------------------------------------------

class Particle{
public:
    Vector2 pos;
    Color color;

    Particle(Vector2 position, Color col){
        pos = position;
        color = col;
    }
};

enum CreatureType{
    GenericPrey,
    GenericPredator, 
    potato
};

struct CreatureParameters {
    CreatureType species = GenericPrey;
    double maxSpeed = 1.0;
    float sightRange = 10.0f;
    Vector2 position = {100, 100};
    double direction = 0.0;
    double health = 100;
    double size = 5;
    double energy = 100000;
    double attackDamage = 20;
    //double attackCooldownLength = 1;
    int attackCooldownLength = 1;
    int reproductionCooldownLength = 10;
};

class Creature {
public:
    CreatureType species;
    double energy;
    int initialEnergy;
    double maxSpeed;
    double initialMaxSpeed;
    float direction;
    Vector2 position;
    Vector2 velocity;
    float sightRange;
    double size;
    double health;
    double initialHealth;
    double energyCost;
    unsigned long age;
    bool alive;
    double attackDamage;
    //double lastAttackTime;
    //double attackCooldownLength;

    int attackCooldownLength;
    int attackCooldownTimer;

    int reproductionTimer;
    int reproductionCooldownLength;

    Creature(const CreatureParameters& params)
        : species(params.species),
          maxSpeed(params.maxSpeed),
          initialMaxSpeed(params.maxSpeed),
          sightRange(params.sightRange),
          position(params.position),
          direction(params.direction),
          health(params.health),
          initialHealth(params.health),
          size(params.size),
          energy(params.energy),
          initialEnergy(int(params.energy)),
          alive(true),
          age(0),
          attackDamage(params.attackDamage),
          attackCooldownLength(params.attackCooldownLength),
          attackCooldownTimer(0),
          reproductionCooldownLength(params.reproductionCooldownLength),
          reproductionTimer(0) 
          {

          }

    Creature() : Creature(CreatureParameters()) {}

    double calculateEnergyCost(double maxSpeed, int sightRange, int size){
        static constexpr double sightCost = 1;
        return 0.01*size*size*size + 1*maxSpeed*maxSpeed + sightCost*sightRange;
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

    void die(){
        alive = false;
    }

    bool canAttack(){
        if(attackCooldownTimer >= attackCooldownLength){
            return true;
        }
        else{
            return false;
        }
    }

    bool canReproduce(){
        if(reproductionTimer >= reproductionCooldownLength){
            return true;
        }
        else{
            return false;
        }
    }

    void update(double ageFactor, double peakAge, int frame){
        updateAge(ageFactor, peakAge, frame);
        updateEnergy();
        move();
        attackCooldownTimer += 1;
        reproductionTimer += 1;
    }

    void updateAge(double ageSpeedDecayFactor, double speedPeakAge, int frame){             //age is measured in seconds at 100 FPS
        if(frame == 0){
            try{
                if((initialMaxSpeed - ageSpeedDecayFactor*(age-speedPeakAge)*(age-speedPeakAge)) < 0){
                    throw std::runtime_error("Initial Speed negative");
                }   
            }
            catch(const std::exception& ex){
                std::cerr << "Error: " << ex.what() << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        if(frame % 100 == 0){
            age++;
        }

        double temp = initialMaxSpeed - ageSpeedDecayFactor*(age-speedPeakAge)*(age-speedPeakAge);      //decay factor must be small or the parabola starts negative
        if(temp > 0){
            maxSpeed = temp;
        }
        else{
            maxSpeed = 0;
            die();
        }
    }

    void updateEnergy(){
        double energyCost = calculateEnergyCost(maxSpeed, sightRange, size);
        if(energy - energyCost > 0){
            energy -= energyCost;
        }
        else{
            die();
        }
    }

    void reproduceA(std::vector<Creature>& creatures){

        CreatureParameters kidParams;
        kidParams.species = species;
        kidParams.maxSpeed = initialMaxSpeed;
        kidParams.sightRange = sightRange;
        kidParams.position = position;
        kidParams.direction = direction;
        kidParams.health = initialHealth;
        kidParams.size = size;
        kidParams.energy = initialEnergy;
        kidParams.attackDamage = attackDamage;
        kidParams.attackCooldownLength = attackCooldownLength;
        kidParams.reproductionCooldownLength = reproductionCooldownLength;

        if(position.x > 2*size){
            Creature kid(kidParams);
            kid.direction = direction += 180;
            creatures.push_back(kid);
        }
        else if(screenWidth - position.x > 2*size){
            Creature kid(kidParams);
            kid.direction = direction += 180;
            creatures.push_back(kid);
        }
    }

    void reproduceS(std::vector<Creature>& creatures, Creature parent2){
        CreatureParameters kidParameters = singlePointCrossover(parent2);
        double mutationChance = 0.1;
        double mutationMagnitude = 0.25;
        kidParameters = mutate(kidParameters, mutationChance, mutationMagnitude);


    }

    CreatureParameters mutate(CreatureParameters& p, double mChance, double magnitude){
        double div;
        if(magnitude = 0.25){
            div = 4;
        }
        else if (magnitude = 0.2){
            div = 5;
        }
        else if (magnitude = 0.1){
            div = 10;
        }
        else{
            div = 10;
        }

        CreatureParameters params = p;


        params.maxSpeed += (RandomFloat(0, 1, rng) <= mChance) ? RandomFloat(-params.maxSpeed/div, params.maxSpeed/div, rng) : 0;
        params.size += (RandomFloat(0, 1, rng) <= mChance) ? RandomFloat(-params.size/div, params.size/div, rng) : 0;
        params.energy += (RandomFloat(0, 1, rng) <= mChance) ? RandomFloat(-params.energy/div, params.energy/div, rng) : 0;
        params.maxSpeed += (RandomFloat(0, 1, rng) <= mChance) ? RandomFloat(-params.sightRange/div, params.sightRange/div, rng) : 0;
        params.maxSpeed += (RandomFloat(0, 1, rng) <= mChance) ? RandomFloat(-params.health/div, params.health/div, rng) : 0;
        params.maxSpeed += (RandomFloat(0, 1, rng) <= mChance) ? RandomFloat(-params.attackDamage/div, params.attackDamage/div, rng) : 0;

        return params;
    }

    CreatureParameters singlePointCrossover(Creature p2){
        CreatureParameters kidParameters;
        std::vector<double> traits1 = {maxSpeed, size, double(initialEnergy), double(sightRange), initialHealth, attackDamage, double(attackCooldownLength)};
        std::vector<double> traits2 = {p2.maxSpeed, p2.size, double(p2.initialEnergy), double(p2.sightRange), p2.initialHealth, p2.attackDamage, double(p2.attackCooldownLength)};
        std::vector<double> traits3;
        int crossoverPoint = RandomInt(0, 6, rng);

        for(int i = 0; i < crossoverPoint; i++){
            traits3[i] = traits1[i];
        }

        for(int i = crossoverPoint; i < traits1.size(); i++){
            traits3[i] = traits2[i];
        }

        kidParameters.maxSpeed = traits3[0];
        kidParameters.size = traits3[1];
        kidParameters.energy = traits3[2];
        kidParameters.sightRange = traits3[3];
        kidParameters.health = traits3[4];
        kidParameters.attackDamage = traits3[5];
        kidParameters.attackCooldownLength = traits3[6];

        kidParameters.species = species;
        kidParameters.direction = direction;
        kidParameters.position = position;
        kidParameters.reproductionCooldownLength = reproductionCooldownLength;

        return kidParameters;
    }

};

class QuadTree{
public:

    int currentDepth;
    Rectangle currentSize;
    std::vector<Particle> particles;
    std::array<std::shared_ptr<QuadTree>, 4> children{};
    std::array<Rectangle, 4> childAreas{};

    QuadTree(const int setDepth, const Rectangle& setSize){
        currentDepth = setDepth;
        resize(setSize);
    }

    void resize(const Rectangle& setSize){
        clear(); 
        currentSize = setSize;

        float newWidth = currentSize.width / 2.0f, newHeight = currentSize.height / 2.0f;
        float x = currentSize.x, y = currentSize.y;

        childAreas = {
            Rectangle{x + newWidth, y, newWidth, newHeight},
            Rectangle{x, y, newWidth, newHeight},
            Rectangle{x, y + newHeight, newWidth, newHeight},
            Rectangle{x + newWidth, y + newHeight, newWidth, newHeight}
        };

    }

    void clear(){
        particles.clear();

        for(int i = 0; i < 4; i++){
            if(children[i]){
                children[i]->clear();
            }
            children[i].reset();
        }
    }

    void insert(const Particle& newParticle){
        for(int i = 0 ; i < 4; i++){
            if(CheckCollisionPointRec(newParticle.pos, childAreas[i])){
                if(currentDepth + 1 < maxTreeDepth){
                    if(!children[i]){
                        children[i] = std::make_shared<QuadTree>(currentDepth + 1, childAreas[i]);
                    }
                    children[i]->insert(newParticle);
                    return;
                }
            }
        }

        //didn't fit in children, so must go here
        particles.emplace_back(newParticle);
    }

    std::list<Particle> search(Vector2& center, float radius, bool removeSearched){
        std::list<Particle> result;

        // Check if the search area intersects the QuadTree node's boundary
        if(!CheckCollisionCircleRec(center, radius, currentSize)) {
            return result;
        }

        // If this node has particles, add the ones within the search area to the result list
        for(unsigned int i = 0; i < particles.size(); i++){
            if(CheckCollisionPointCircle(particles[i].pos, center, radius)){
                result.push_back(particles[i]);
                if(removeSearched){
                    particles.erase(particles.begin() + i);
                }
            }
        }

        // Recursively search the children nodes
        for(int i = 0; i < 4; i++){
            if(children[i]){
                auto childResult = children[i]->search(center, radius, removeSearched);
                result.splice(result.end(), childResult);
            }
        }

        return result;
    }

    std::vector<Particle> returnAll(int depth){
        std::vector<Particle> result;

        if(currentDepth >= depth){
            result.insert(result.end(), particles.begin(), particles.end());
        }

        for(int i = 0; i < 4; i++){
            if(children[i]){
                auto childResult = children[i]->returnAll(depth);
                result.insert(result.end(), childResult.begin(), childResult.end());
            }
        }

        return result;
    }

    int size() const{
        int count = particles.size();

        for(int i = 0 ; i < 4; i++){
            if(children[i]){
                count += children[i]->size();
            }
        }

        return count;
    }

    void draw() const{
        for(const auto& particle : particles){
            DrawPixelV(particle.pos, particle.color);
        }

        //DrawRectangleLinesEx(currentSize, 0.7, GREEN);

        for(int i = 0; i < 4; i++){
            if(children[i]){
                children[i]->draw();
            }
        }
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

bool vectorsEqual(Vector2 v1, Vector2 v2){
    if(v1.x == v2.x && v1.y == v2.y){
        return true;
    }
    else{
        return false;
    }
}

float vector2distance(Vector2 v1, Vector2 v2) {
    float dx = v2.x - v1.x;
    float dy = v2.y - v1.y;
    return std::sqrt(dx * dx + dy * dy);
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

QuadTree initializeQT(std::vector<Creature>& predators, std::vector<Creature>& prey){
    QuadTree qt(0, Rectangle{0, 0, screenWidth, screenHeight});
    Color predColor = RED, preyColor = GREEN;

    for(const auto& p : predators){
        qt.insert(Particle{p.position, predColor});
    }

    for(const auto& p : prey){
        qt.insert(Particle{p.position, preyColor});
    }

    return qt;
}

void checkCollisions(std::vector<Creature>& predators, std::vector<Creature>& prey, QuadTree& qt){
    double searchRadius = 100;
    std::list<Particle> colliding;

    for(auto& predator : predators){
        qt.search(predator.position, searchRadius, false);
    }
}

void primativeCollisionCheck(std::vector<Creature>& predators, std::vector<Creature>& prey){
    auto currentTime = std::chrono::high_resolution_clock::now();
    double currentTimeInSeconds = std::chrono::duration<double>(std::chrono::duration_cast<std::chrono::microseconds>(currentTime.time_since_epoch())).count();
    
    for(int i = 0; i < predators.size(); i++){
        for(int j = 0; j < prey.size(); j++){
            if(CheckCollisionCircles(predators[i].position, predators[i].size, prey[j].position, prey[j].size)){
                
                if(predators[i].canAttack()){
                    prey[j].health -= predators[i].attackDamage;
                    predators[i].attackCooldownTimer = 0;
                    predators[i].energy += prey[j].health * predators[i].attackDamage / prey[j].initialHealth;
                }

                if(prey[j].health <= 0){
                    predators[i].energy += prey[j].energy;
                    prey.erase(prey.begin() + j);
                }

                /*if(predators[i].energy >= predators[i].initialEnergy and predators[i].canReproduce()){
                    predators[i].reproduceA(predators);
                    predators[i].reproductionTimer = 0;
                    predators[i].energy = predators[i].initialEnergy/2;
                }*/
            }
        }
    }
}

void qtCollisionCheck(){

}

//---------------------------------------------------------------------------------------------------------------------------------

int main() {
    initialize();

    int numPredators = 2, numPrey = 500;
    Color predColor = RED, preyColor = GREEN;

    std::vector<Creature> predators(numPredators);
    std::vector<Creature> prey(numPrey);

    CreatureParameters initialPredatorP;
    initialPredatorP.species = GenericPredator;
    initialPredatorP.size = 7;
    initialPredatorP.attackDamage = 50;
    initialPredatorP.energy = 100000;
    initialPredatorP.maxSpeed = 1;
    initialPredatorP.sightRange = 1;
    initialPredatorP.attackCooldownLength = 50;
    initialPredatorP.reproductionCooldownLength = 500;

    CreatureParameters initialPreyP;
    initialPreyP.species = GenericPrey;
    initialPreyP.size = 7;
    initialPreyP.attackDamage = 0;
    initialPreyP.energy = 100000;
    initialPreyP.maxSpeed = 1.1;
    initialPreyP.sightRange = 1;
    initialPreyP.health = 100;
    initialPreyP.attackCooldownLength = 100;

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
        Particle p(predators[i].position, RED);
        qt.insert(p);
    }

    for(int i = 0; i < numPrey; i++){
        Particle p(prey[i].position, GREEN);
        qt.insert(p);
    }


    for(int frame = 0; !WindowShouldClose(); frame++){

        qt = initializeQT(predators, prey);
        primativeCollisionCheck(predators, prey);

        BeginDrawing();
        drawBackground();

        // Draw predators
        for(int i = 0 ; i < predators.size(); i++){
            drawHealthBar(predators[i], 40, 3, 20, GREEN, RED, 1);
            drawHealthBar(predators[i], 40, 3, 20, BLUE, ORANGE, 2);
            DrawCircleV(predators[i].position, predators[i].size, predColor);
        }

        // Draw prey
        for(int i = 0; i < prey.size(); i++){
            drawHealthBar(prey[i], 40, 3, 20, GREEN, RED, 1);
            drawHealthBar(prey[i], 40, 3, 20, BLUE, ORANGE, 2);
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
            predators[i].update(0.001, 10, frame);
            predators[i].shiftDirectionRandomly(0.7);
        }

        // Update prey
        for(int i = 0; i < prey.size(); i++){
            if(!prey[i].alive){
                prey.erase(prey.begin() + i);
                i--;
                continue;
            }
            prey[i].update(0.0001, 10, frame);
            prey[i].shiftDirectionRandomly(0.7);
        }

        if(frame % 100 == 0){
            prey[RandomInt(0, prey.size(), rng)].reproduceA(prey);
        }


    }
    return 0;
}
