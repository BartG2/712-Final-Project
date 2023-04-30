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

    std::size_t id;

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
          reproductionTimer(0){
            generateUniqueId();
          }

    Creature() : Creature(CreatureParameters()) {}

    double calculateEnergyCost(double maxSpeed, int sightRange, int size){
        static constexpr double sizeCost = 0.5, speedCost = 0.5, sightCost = 0.5;
        return sizeCost*size*size + speedCost*maxSpeed*maxSpeed + sightCost*sightCost*sightRange;
    }

    void generateUniqueId() {
        std::string combinedVars = std::to_string(static_cast<int>(species)) +
                                    std::to_string(energy) +
                                    std::to_string(initialEnergy) +
                                    std::to_string(maxSpeed) +
                                    std::to_string(initialMaxSpeed) +
                                    std::to_string(direction) +
                                    std::to_string(position.x) +
                                    std::to_string(position.y) +
                                    std::to_string(velocity.x) +
                                    std::to_string(velocity.y) +
                                    std::to_string(sightRange) +
                                    std::to_string(size) +
                                    std::to_string(health) +
                                    std::to_string(initialHealth) +
                                    std::to_string(energyCost) +
                                    std::to_string(age);

        std::hash<std::string> hashFunc;
        id = hashFunc(combinedVars);
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
        double mutationChance = 1.0;
        double mutationMagnitude = 0.25;
        kidParameters = mutate(kidParameters, mutationChance, mutationMagnitude);

        if(position.x > 2*size){
            Creature kid(kidParameters);
            kid.direction = direction += 180;
            creatures.push_back(kid);
        }
        else if(screenWidth - position.x > 2*size){
            Creature kid(kidParameters);
            kid.direction = direction += 180;
            creatures.push_back(kid);
        }
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
        params.sightRange += (RandomFloat(0, 1, rng) <= mChance) ? RandomFloat(-params.sightRange/div, params.sightRange/div, rng) : 0;
        params.health += (RandomFloat(0, 1, rng) <= mChance) ? RandomFloat(-params.health/div, params.health/div, rng) : 0;
        params.attackDamage += (RandomFloat(0, 1, rng) <= mChance) ? RandomFloat(-params.attackDamage/div, params.attackDamage/div, rng) : 0;

        return params;
    }

    CreatureParameters singlePointCrossover(Creature p2){
        CreatureParameters kidParameters;
        std::vector<double> traits1 = {maxSpeed, size, double(initialEnergy), double(sightRange), initialHealth, attackDamage, double(attackCooldownLength)};
        std::vector<double> traits2 = {p2.maxSpeed, p2.size, double(p2.initialEnergy), double(p2.sightRange), p2.initialHealth, p2.attackDamage, double(p2.attackCooldownLength)};
        std::vector<double> traits3(traits1.size());
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

class QuadTree {
public:
    int currentDepth;
    Rectangle currentSize;
    std::vector<Creature*> creatures; // Store pointers to Creature objects
    std::array<std::shared_ptr<QuadTree>, 4> children{};
    std::array<Rectangle, 4> childAreas{};

    QuadTree(const int setDepth, const Rectangle& setSize) {
        currentDepth = setDepth;
        resize(setSize);
    }

    void resize(const Rectangle& setSize) {
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

    void clear() {
        creatures.clear();

        for (int i = 0; i < 4; i++) {
            if (children[i]) {
                children[i]->clear();
            }
            children[i].reset();
        }
    }

    void insert(Creature* newCreature) {
        for (int i = 0; i < 4; i++) {
            if (CheckCollisionPointRec(newCreature->position, childAreas[i])) {
                if (currentDepth + 1 < maxTreeDepth) {
                    if (!children[i]) {
                        children[i] = std::make_shared<QuadTree>(currentDepth + 1, childAreas[i]);
                    }
                    children[i]->insert(newCreature);
                    return;
                }
            }
        }

        // Didn't fit in children, so must go here
        creatures.emplace_back(newCreature);
    }

std::vector<std::reference_wrapper<Creature>> search(Vector2& center, float radius) {
    std::vector<std::reference_wrapper<Creature>> result;

    if (!CheckCollisionCircleRec(center, radius, currentSize)) {
        return result;
    }

    for (Creature* creature : creatures) {
        if (CheckCollisionPointCircle(creature->position, center, radius)) {
            result.push_back(*creature);
        }
    }

    for (int i = 0; i < 4; i++) {
        if (children[i]) {
            auto childResult = children[i]->search(center, radius);
            result.insert(result.end(), childResult.begin(), childResult.end());
        }
    }

    return result;
}


    std::vector<Creature*> returnAll(int depth){
        std::vector<Creature*> result;

        if(currentDepth >= depth){
            result.insert(result.end(), creatures.begin(), creatures.end());
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
        int count = creatures.size();

        for(int i = 0 ; i < 4; i++){
            if(children[i]){
                count += children[i]->size();
            }
        }

        return count;
    }

    void draw() const{
        for(const auto& particle : creatures){
            Color c = (particle->species == GenericPredator) ? RED : GREEN;
            DrawPixelV(particle->position, c);
        }

        //DrawRectangleLinesEx(currentSize, 0.7, GREEN);

        for(int i = 0; i < 4; i++){
            if(children[i]){
                children[i]->draw();
            }
        }
    }

};

class QuadTreeWrapper {
public:
    QuadTree quadtree;
    std::vector<Creature> creatures;

public:
    QuadTreeWrapper(int depth, const Rectangle& size) : quadtree(depth, size) {}

    void insert(const Creature& newCreature) {
        creatures.push_back(newCreature);
        Creature* creaturePtr = &creatures.back();
        quadtree.insert(creaturePtr);
    }

    std::vector<std::reference_wrapper<Creature>> search(Vector2& center, float radius) {
        return quadtree.search(center, radius);
    }


    std::vector<Creature*> returnAll(int depth) {
        return quadtree.returnAll(depth);
    }

    void clear() {
        creatures.clear();
        quadtree.clear();
    }

    void resize(const Rectangle& newSize) {
        quadtree.resize(newSize);
    }

    int size() const {
        return quadtree.size();
    }

    void draw() const {
        quadtree.draw();
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
        if (p.species == GenericPredator) {
            qtPredators.push_back(p);
        } else if (p.species == GenericPrey) {
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
            if (creature.species == GenericPrey && CheckCollisionCircles(center, predator.size, creature.position, creature.size)) {
                
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

            if(creature.species == GenericPredator and CheckCollisionCircles(center, predator.size, creature.position, creature.size) and predator.id != creature.id){

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
            if (p2.species == GenericPrey && CheckCollisionCircles(center, p1.size, p2.position, p2.size) and p1.id != p2.id){

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
        if(quadTreeWrapper.creatures[i].species == GenericPredator){
            predators.push_back(quadTreeWrapper.creatures[i]);
        }
        else if(quadTreeWrapper.creatures[i].species == GenericPrey){
            prey.push_back(quadTreeWrapper.creatures[i]);
        }
    }

    for(int i = 0; i < prey.size(); i++){
        if(!prey[i].alive){
            prey.erase(prey.begin() + i);
        }
    }
}


//---------------------------------------------------------------------------------------------------------------------------------

int main() {
    initialize();

    int numPredators = 20, numPrey = 40;
    Color predColor = RED, preyColor = GREEN;

    std::vector<Creature> predators(numPredators);
    std::vector<Creature> prey(numPrey);

    CreatureParameters initialPredatorP;
    initialPredatorP.species = GenericPredator;
    initialPredatorP.size = 5;
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
            predators[i].update(0.0001, 10, frame);
            predators[i].shiftDirectionRandomly(0.5);
        }

        // Update prey
        for(int i = 0; i < prey.size(); i++){
            if(!prey[i].alive){
                prey.erase(prey.begin() + i);
                i--;
                continue;
            }
            prey[i].update(0.0001, 10, frame);
            prey[i].shiftDirectionRandomly(0.5);
        }

        if(frame % 100 == 0){
            std::cout << "Predators: " << predators.size() << ", Prey: " << prey.size() << ", total: " << predators.size() + prey.size() << std::endl;
            //prey[RandomInt(0, prey.size(), rng)].reproduceA(prey);
        }


    }
    return 0;
}
