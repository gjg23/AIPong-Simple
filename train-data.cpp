#include "train-data.h"
#include "config.h"
#include <cfloat>
#include <cmath>
#include <limits>

// Generate possible states
std::vector<TrainData> PongStateGenerator::generateStates()
{
    std::vector<TrainData> states;

    // Generate comprehensive set of states
    for (int bally = 0; bally <= SCREEN_HEIGHT; bally += 10)
    {
        for (int paddley = 0; paddley < SCREEN_HEIGHT - PADDLE_HEIGHT; paddley += (SCREEN_HEIGHT - PADDLE_HEIGHT) / 50)
        {
            states.push_back({bally, paddley, calculate_movement({bally, paddley})});
        }
    }
    return states;
}

int PongStateGenerator::calculate_movement(const GameState& state) {
    if (state.bally > state.paddley){
        return 0;
    }
    else if (state.bally < state.paddley){
        return 1;
    }
    else{
        return 2;
    }
}

// Make csv file
void PongStateGenerator::generateDataCSV(const std::string& filename)
{
    // Generate states
    std::vector<TrainData> states = generateStates();

    // Open csv file for writing
    std::ofstream csvFile(filename);

    // Write CSV Header
    csvFile << "bally,paddley,optimalmove\n";

    // Process and write each state
    for (const auto& state : states)
    {
        // Write state and mevement to file
        csvFile << state.bally << ","
                << state.paddley << ","
                << state.optimalmove << "\n";
    }

    std::cout << "Generated CSV with " << states.size() << " Game states to " << filename << std::endl;
}

// Read csv file
std::vector<TrainData> PongStateGenerator::readCSV(const std::string& filename)
{
    std::vector<TrainData> states;

    // Open csv for reading
    std::ifstream csvFile(filename);
    if (!csvFile.is_open())
    {
        std::cerr << "Error opening file: " << filename << std::endl;
        return states;
    }

    // Skip header
    std::string headerline;
    std::getline(csvFile, headerline);

    // Read state data
    std::string line;
    while (std::getline(csvFile, line))
    {
        std::stringstream ss(line);
        std::string item;
        TrainData state;

        // Parse line
        std::getline(ss, item, ','); state.bally = std::stoi(item);
        std::getline(ss, item, ','); state.paddley = std::stoi(item);
        std::getline(ss, item, ','); state.optimalmove = std::stoi(item);

        states.push_back(state);
    }

    std::cout << "Read " << states.size() << " game states" << std::endl;
    return states;
}