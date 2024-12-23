#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <fstream>

struct TrainData;
struct GameState;

class PongStateGenerator
{
private:
    // Generate possible states
    std::vector<TrainData> generateStates();
public:
    // Determine optimal paddle movement
    int calculate_movement(const GameState& state);

    void generateDataCSV(const std::string& filename);

    std::vector<TrainData> readCSV(const std::string& filename);
};