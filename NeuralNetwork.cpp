#include "NeuralNetwork.h"

NeuralNetwork::NeuralNetwork(const std::vector<int>& layerSizes) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(-1.0, 1.0);

    for (int size : layerSizes) {
        layers.push_back(std::vector<double>(size));
        biases.push_back(std::vector<double>(size));
        for (double& bias : biases.back()) {
            bias = dis(gen);
        }
    }

    for (int i = 0; i < layers.size() - 1; i++) {
        weights.push_back(std::vector<std::vector<double>>(layers[i].size(), std::vector<double>(layers[i+1].size())));
        for (auto& row : weights.back()) {
            for (double& weight : row) {
                weight = dis(gen);
            }
        }
    }
}

std::vector<double> NeuralNetwork::feedforward(const std::vector<double>& inputs) {
    for (int i = 0; i < layers[0].size(); i++) {
        layers[0][i] = inputs[i];
    }

    for (int i = 1; i < layers.size(); i++) {
        for (int j = 0; j < layers[i].size(); j++) {
            double sum = 0;
            for (int k = 0; k < layers[i-1].size(); k++) {
                sum += layers[i-1][k] * weights[i-1][k][j];
            }
            sum += biases[i][j];
            layers[i][j] = sigmoid(sum);
        }
    }

    return layers.back();
}

double NeuralNetwork::sigmoid(double x) {
    return 1 / (1 + std::exp(-x));
}

void NeuralNetwork::mutate(double mutationRate, double mutationStrength) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(-mutationStrength, mutationStrength);
    std::uniform_real_distribution<double> prob(0.0, 1.0);

    for (auto& layer : weights) {
        for (auto& row : layer) {
            for (double& weight : row) {
                if (prob(gen) < mutationRate) {
                    weight += dis(gen);
                }
            }
        }
    }

    for (auto& layer : biases) {
        for (double& bias : layer) {
            if (prob(gen) < mutationRate) {
                bias += dis(gen);
            }
        }
    }
}

void NeuralNetwork::addNode(int layerIndex) {
    if (layerIndex == 0 || layerIndex == layers.size() - 1) {
        throw std::invalid_argument("Cannot add node to input or output layer");
    }

    layers[layerIndex].push_back(0.0);
    biases[layerIndex].push_back(0.0);

    for (int i = 0; i < layers[layerIndex - 1].size(); i++) {
        weights[layerIndex - 1][i].push_back(0.0);
    }

    if (layerIndex < layers.size() - 1) {
        weights[layerIndex].push_back(std::vector<double>(layers[layerIndex + 1].size(), 0.0));
    }
}

void NeuralNetwork::addConnection(int fromLayer, int fromNode, int toNode) {
    if (fromLayer >= layers.size() - 1 || toNode >= layers[fromLayer + 1].size()) {
        throw std::invalid_argument("Invalid connection indices");
    }

    weights[fromLayer][fromNode][toNode] = 0.0;
}

void NeuralNetwork::removeConnection(int fromLayer, int fromNode, int toNode) {
    if (fromLayer >= layers.size() - 1 || toNode >= layers[fromLayer + 1].size()) {
        throw std::invalid_argument("Invalid connection indices");
    }

    weights[fromLayer][fromNode][toNode] = 0.0;
}