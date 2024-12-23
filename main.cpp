#include "network.h"
#include "config.h"
#include "pong.h"
#include "train-data.h"

using namespace std;

int main()
{
    // Setup training data generation objects
    PongStateGenerator generator;
    vector<TrainData> states;
    std::string statesfilename = "statesdata.csv";

    cout << "Generating training data...\n";

    generator.generateDataCSV(statesfilename);
    states = generator.readCSV(statesfilename);

    // Setup network with 2 inputs, 20 hidden nuerons in 2 layers, 3 outputs (up down none)
    PongNeuralNetwork network({5, 10, 10, 3});

    // Separate game states and their corresponding optimal moves
    std::vector<GameState> inputs;
    std::vector<int> expected_moves;

    for (const auto& state : states) {
        // Assuming the GameState struct has an 'optimalmove' field
        inputs.push_back({state.bally, state.paddley});
        expected_moves.push_back(state.optimalmove);
    }


    // Begin fitting the model to the training data
    cout << "starting training...\n";

    // Learning rate 0.0001, 10 epochs
    network.train(inputs, expected_moves, 0.0001, 500);

    // Play the game with the network
    cout << "Training complete. Starting game with trained agent...\n";
    PongGame pong(&network);
    pong.run(true);

    return 0;
}