#ifndef NeuralNetwork_H
#define NeuralNetwork_H

#include <vector>
#include <cmath>
#include <random>
#include <stdexcept>

class NeuralNetwork {
private:
    std::vector<std::vector<double>> layers;
    std::vector<std::vector<std::vector<double>>> weights;
    std::vector<std::vector<double>> biases;

public:
    NeuralNetwork(const std::vector<int>&);
    std::vector<double> feedforward(const std::vector<double>&);
    double sigmoid(double);
    void mutate(double, double);
    // add a node to the layer
    void addNode(int);
    // add connection between fromNode in layer fromLayer and toNode in layer fromLayer + 1
    void addConnection(int fromLayer, int fromNode, int toNode);
    // remove connection between fromNode in layer fromLayer and toNode in layer fromLayer + 1
    void removeConnection(int fromLayer, int fromNode, int toNode);
};
#endif // NeuralNetwork_H