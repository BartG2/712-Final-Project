#pragma once

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

class QuadTree;
#include "QuadTreeWrapper.h"

#include "CreatureType.h"
#include "CreatureParameters.h"

#include "Functions.h"

struct QuadTreeParameters;

class Creature {
public:
    CreatureType species;
    double energy;
    int initialEnergy;
    double maxSpeed;
    double initialMaxSpeed;
    float direction;
    Vector2 position;
    float sightRange;
    double size;
    double health;
    double initialHealth;
    double energyCost;
    unsigned long age;
    bool alive;
    double attackDamage;

    int attackCooldownLength;
    int attackCooldownTimer;

    int reproductionCooldownLength;
    int reproductionTimer;

    bool fightOrFlight;
    float targetDirection;

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
          age(0),
          alive(true),
          attackDamage(params.attackDamage),
          attackCooldownLength(params.attackCooldownLength),
          attackCooldownTimer(0),
          reproductionCooldownLength(params.reproductionCooldownLength),
          reproductionTimer(0),
          fightOrFlight(false),
          targetDirection(0)
          
          {
            generateUniqueId();
          }

    Creature() : Creature(CreatureParameters()) {}

    double calculateEnergyCost(double maxSpeed, int sightRange, int size){
        static constexpr double sizeCost = 0.2, speedCost = 0.2, sightCost = 0.2;
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
                                    std::to_string(sightRange) +
                                    std::to_string(size) +
                                    std::to_string(health) +
                                    std::to_string(initialHealth) +
                                    std::to_string(energyCost) +
                                    std::to_string(age);

        std::hash<std::string> hashFunc;
        id = hashFunc(combinedVars);
    }

    Vector2 nearestPredator(QuadTreeWrapper& qt){
        Vector2 nearestPredatorPos = position;
        float minDistance = std::numeric_limits<float>::max();

        auto nearbyCreatures = qt.search(position, sightRange);

        for(auto& creature : nearbyCreatures){
            if(creature.get().species == CreatureType::GenericPredator){
                float distance = vector2distance(position, creature.get().position);

                if(distance < minDistance){
                    minDistance = distance;
                    nearestPredatorPos = creature.get().position;
                }
            }
        }

        return nearestPredatorPos;
    }

    Vector2 nearestPrey(QuadTreeWrapper& qt){
        Vector2 nearestPreyPos = position;
        float minDistance = std::numeric_limits<float>::max();

        auto nearbyCreatures = qt.search(position, sightRange);

        for(auto& creature : nearbyCreatures){
            if(creature.get().species == CreatureType::GenericPrey){
                float distance = vector2distance(position, creature.get().position);

                if(distance < minDistance){
                    minDistance = distance;
                    nearestPreyPos = creature.get().position;
                }
            }
        }

        return nearestPreyPos;
    }

    float findTargetDirection(QuadTreeWrapper& qt) {
        if (species == CreatureType::GenericPredator) {
            Vector2 preyPosition = nearestPrey(qt);
            
            if (preyPosition.x != 0 || preyPosition.y != 0) {
                return vector2angle(position, preyPosition);
            }
        }

        else if (species == CreatureType::GenericPrey) {
            Vector2 predatorPosition = nearestPredator(qt);
            
            if (predatorPosition.x != 0 || predatorPosition.y != 0) {
                float angleToPredator = vector2angle(position, predatorPosition);
                return (angleToPredator > 180) ? angleToPredator - 180 : angleToPredator + 180;
            }
        }

        return direction;
    }

    void biasDirection(float targetDirection, float biasMagnitude){
        float difference = targetDirection - direction;

        if(difference > 180){
            difference -= 360;
        }
        else if (difference < -180){
            difference += 360;
        }

        direction = direction + biasMagnitude * difference;

        if(direction > 360){
            direction -= 360;
        }
        else if(direction < 0){
            direction += 360;
        }
    }

    //between 0 and 360 degrees
    float constrainAngle(float angle){
        angle = fmod(angle, 360);
        if(angle < 0){
            angle += 360;
        }
        return angle;
    }

    void move(QuadTreeWrapper& qt){
        //direction = constrainAngle(direction);
        //targetDirection = constrainAngle(targetDirection);
        
        //targetDirection = findTargetDirection(qt);
        //float biasmagnitude = 0.005;
        //biasDirection(targetDirection, biasmagnitude);

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

    void update(double ageFactor, double peakAge, int frame, QuadTreeWrapper& qt){
        updateAge(ageFactor, peakAge, frame);
        updateEnergy();
        move(qt);
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
        if(magnitude == 0.25){
            div = 4;
        }
        else if (magnitude == 0.2){
            div = 5;
        }
        else if (magnitude == 0.1){
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

        for(unsigned int i = crossoverPoint; i < traits1.size(); i++){
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
