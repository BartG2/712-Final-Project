#include "QuadTree.h"
#include "Creature.h"
#include <stack>

void QuadTree::resize(const Rectangle& setSize) {
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

void QuadTree::clear() {
    creatures.clear();

    for (int i = 0; i < 4; i++) {
        if (children[i]) {
            children[i]->clear();
        }
        children[i].reset();
    }
}

/*void QuadTree::insert(Creature* newCreature) {
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
}*/

void QuadTree::insert(Creature* newCreature) {
    QuadTree* currentNode = this;

    while (true) {
        bool inserted = false;
        for (int i = 0; i < 4; i++) {
            if (CheckCollisionPointRec(newCreature->position, currentNode->childAreas[i])) {
                if (currentNode->currentDepth + 1 < currentNode->maxTreeDepth) {
                    if (!currentNode->children[i]) {
                        currentNode->children[i] = std::make_shared<QuadTree>(currentNode->currentDepth + 1, currentNode->childAreas[i]);
                    }
                    currentNode = currentNode->children[i].get();
                    inserted = true;
                    break;
                }
            }
        }

        if (!inserted) {
            currentNode->creatures.emplace_back(newCreature);
            break;
        }
    }
}

std::vector<std::reference_wrapper<Creature>> QuadTree::search(Vector2& center, float radius) {
    std::vector<std::reference_wrapper<Creature>> result;
    std::stack<QuadTree*> nodeStack;

    nodeStack.push(this);

    while (!nodeStack.empty()) {
        QuadTree* currentNode = nodeStack.top();
        nodeStack.pop();

        if (!CheckCollisionCircleRec(center, radius, currentNode->currentSize)) {
            continue;
        }

        for (Creature* creature : currentNode->creatures) {
            if (CheckCollisionPointCircle(creature->position, center, radius)) {
                result.push_back(*creature);
            }
        }

        for (int i = 0; i < 4; i++) {
            if (currentNode->children[i]) {
                nodeStack.push(currentNode->children[i].get());
            }
        }
    }

    return result;
}

/*std::vector<std::reference_wrapper<Creature>> QuadTree::search(Vector2& center, float radius) {
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
}*/

std::vector<Creature*> QuadTree::returnAll(int depth){
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

int QuadTree::size() const{
    int count = creatures.size();

    for(int i = 0 ; i < 4; i++){
        if(children[i]){
            count += children[i]->size();
        }
    }

    return count;
}

void QuadTree::draw() const{
    for(const auto& particle : creatures){
        Color c = (particle->species == CreatureType::GenericPredator) ? RED : GREEN;
        DrawPixelV(particle->position, c);
    }

    //DrawRectangleLinesEx(currentSize, 0.7, GREEN);

    for(int i = 0; i < 4; i++){
        if(children[i]){
            children[i]->draw();
        }
    }
}
