#ifndef PONG_NEURAL_NETWORK_H
#define PONG_NEURAL_NETWORK_H

#include <vector>
#include <fstream>
#include <numeric>
#include <iostream>
#include <cmath>
#include <string>
#include <random>
#include <algorithm>

struct GameState;

class PongNeuralNetwork {
private:
    // Normalization parameters
    double mean_bally = 0.0, std_bally = 1.0;
    double mean_paddley = 0.0, std_paddley = 1.0;

    // Network architecture
    std::vector<int> layer_sizes;
    std::vector<std::vector<std::vector<double>>> weights;
    std::vector<std::vector<double>> biases;
    std::vector<std::vector<double>> layer_outputs;

    // Random number generator for weight initialization
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<> dis;

    // Activation function (tanh) and derivative
    double activation_tanh(double x);
    double tanh_derivative(double x);

    // Softmax function for the output layer
    std::vector<double> softmax(const std::vector<double>& input);

    // Forward propagation
    std::vector<double> forward_propagate(const std::vector<double>& input);

    // Backpropagation
    void backpropagate(const std::vector<double>& input, const std::vector<double>& output_gradient, double learning_rate);

    // Input normalization for game state
    std::vector<double> normalize_input(const GameState& state);
    void compute_normalization_params(const std::vector<GameState>& training_data);

public:
    // Constructor: define network architecture
    PongNeuralNetwork(const std::vector<int>& arch);

    // Copy constructor for PongNeuralNetwork
    PongNeuralNetwork(PongNeuralNetwork& net);

    // Predict optimal paddle movement based on state
    int predict_move(GameState gamestate);

    // Train method using simple gradient descent
    void train(const std::vector<GameState>& training_data, const std::vector<int>& expected_moves, double learning_rate, int epochs);
};

#endif