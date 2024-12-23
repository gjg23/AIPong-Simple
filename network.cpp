#include "config.h"
#include "network.h"

// Constructors--------------------------------------------

// define network architecture
PongNeuralNetwork::PongNeuralNetwork(const std::vector<int>& arch) : layer_sizes(arch), gen(rd()), dis(-1.0, 1.0)
{
    // Initialize weights and biases with rand values
    for (size_t i = 0; i < layer_sizes.size() - 1; ++i)
    {
        std::vector<std::vector<double>> layer_weights;
        for (int j = 0; j < layer_sizes[i + 1]; ++j)
        {
            std::vector<double> neuron_weights;
            for (int k = 0; k < layer_sizes[i]; ++k)
            {
                // Xavier/Glorot initialization
                double scale = std::sqrt(2.0 / (layer_sizes[i] + layer_sizes[i+1]));
                neuron_weights.push_back(dis(gen) * scale);
            }
            layer_weights.push_back(neuron_weights);
        }
        weights.push_back(layer_weights);

        // Biases for each neuron in the layer
        std::vector<double> layer_biases;
        for (int j = 0; j < layer_sizes[i + 1]; ++j)
        {
            layer_biases.push_back(dis(gen));
        }
        biases.push_back(layer_biases);
    }
}

// Copy constructor for PongNeuralNetwork
PongNeuralNetwork::PongNeuralNetwork(PongNeuralNetwork& net) 
    : layer_sizes(net.layer_sizes),  // Copy layer architecture
    gen(rd()),                     // Initialize random generator
    dis(-1.0, 1.0)                 // Maintain distribution range
{
    // Deep copy weights
    weights.clear();
    for (const auto& layer_weight : net.weights) {
        std::vector<std::vector<double>> copied_layer_weights;
        for (const auto& neuron_weights : layer_weight) {
            copied_layer_weights.push_back(neuron_weights);
        }
        weights.push_back(copied_layer_weights);
    }

    // Deep copy biases
    biases.clear();
    for (const auto& layer_bias : net.biases) {
        biases.push_back(layer_bias);
    }

    // Optional: Copy layer outputs if needed
    layer_outputs.clear();
    for (const auto& layer_output : net.layer_outputs) {
        layer_outputs.push_back(layer_output);
    }
}



// Helper Functions----------------------------------------

// Activation function (tanh)
double PongNeuralNetwork::activation_tanh(double x)
{
    return std::tanh(x);
}

// Derivative of tanh for backpropagation
double PongNeuralNetwork::tanh_derivative(double x)
{
    double tanh_x = std::tanh(x);
    return 1.0 - tanh_x * tanh_x;
}

// Softmax for the output layer to get probability dist
std::vector<double> PongNeuralNetwork::softmax(const std::vector<double>& input)
{
    std::vector<double> output(input.size());
    double mal_val = *std::max_element(input.begin(), input.end());
    double exp_sum = 0.0;

    // Compute exponentials and sum
    for (size_t i = 0; i < input.size(); ++i)
    {
        output[i] = std::exp(input[i] - mal_val);
        exp_sum += output[i];
    }

    // Normalize
    for (double& val : output)
    {
        val /= exp_sum;
    }

    return output;
}



// Propagation through net ----------------------------------------------

// Forward propagation with storage of layer outputs
std::vector<double> PongNeuralNetwork::forward_propagate(const std::vector<double>& input)
{
    layer_outputs.clear();
    layer_outputs.push_back(input);

    std::vector<double> layer_output = input;

    // Propagate through hidden layers
    for (size_t i = 0; i < layer_sizes.size() - 1; ++i) {
        std::vector<double> next_layer(layer_sizes[i + 1], 0.0);

        // Compute next layer activations
        for (int j = 0; j < layer_sizes[i + 1]; ++j) {
            for (int k = 0; k < layer_sizes[i]; ++k) {
                next_layer[j] += weights[i][j][k] * layer_output[k];
            }
            next_layer[j] += biases[i][j];
        }

        // Use tanh for hidden layers, softmax for output
        if (i != layer_sizes.size() - 2)
        {
            for (auto& val : next_layer)
            {
                val = activation_tanh(val);
            }
        }

        // Store layer output
        layer_outputs.push_back(next_layer);
        layer_output = next_layer;
    }

    // Apply softmax to output layer
    return softmax(layer_output);
}

// Back propagateion
void PongNeuralNetwork::backpropagate(const std::vector<double>& input, const std::vector<double>& output_gradient, double learning_rate)
{
    // Re-run forward propagation to refresh layer outputs
    forward_propagate(input);

    // Initialize delta (gradient) for each layer
    std::vector<std::vector<double>> layer_deltas(layer_sizes.size());

    // Output layer delta (already computed in the gradient passed in)
    layer_deltas.back() = output_gradient;

    // Backpropagate the gradient
    for (int layer = layer_sizes.size() - 2; layer >= 0; --layer)
    {
        // Resize delta for current layer
        layer_deltas[layer].resize(layer_sizes[layer]);

        // Compute delta for each neuron in the current layer
        for (int neuron = 0; neuron < layer_sizes[layer]; ++neuron)
        {
            double delta = 0.0;
            
            // Compute gradient based on next layer's deltas
            for (int next_neuron = 0; next_neuron < layer_sizes[layer + 1]; ++next_neuron)
            {
                delta += layer_deltas[layer + 1][next_neuron] * weights[layer][next_neuron][neuron];
            }

            // Apply activation derivative
            delta *= tanh_derivative(layer_outputs[layer][neuron]);

            layer_deltas[layer][neuron] = delta;
        }
    }

    // Update weights and biases
    for (size_t layer = 0; layer < layer_sizes.size() - 1; ++layer)
    {
        for (int next_neuron = 0; next_neuron < layer_sizes[layer + 1]; ++next_neuron)
        {
            for (int current_neuron = 0; current_neuron < layer_sizes[layer]; ++current_neuron)
            {
                // Weight update
                weights[layer][next_neuron][current_neuron] -= 
                    learning_rate * layer_deltas[layer + 1][next_neuron] * layer_outputs[layer][current_neuron];
            }

            // Bias update
            biases[layer][next_neuron] -= 
                learning_rate * layer_deltas[layer + 1][next_neuron];
        }
    }
}



// Input preparation -----------------------------------------------------

// Compute normalization parameters from training data
void PongNeuralNetwork::compute_normalization_params(const std::vector<GameState>& training_data)
{
    // Reset sums and counts
    double sum_bally = 0.0;
    double sum_paddley = 0.0;
    size_t n = training_data.size();

    // Compute means
    for (const auto& state : training_data) {
        sum_bally += state.bally;
        sum_paddley += state.paddley;
    }

    mean_bally = sum_bally / n;
    mean_paddley = sum_paddley / n;

    // Compute standard deviations
    double var_bally = 0.0;
    double var_paddley = 0.0;

    for (const auto& state : training_data) {
        var_bally += std::pow(state.bally - mean_bally, 2);
        var_paddley += std::pow(state.paddley - mean_paddley, 2);
    }

    std_bally = std::sqrt(var_bally / n);
    std_paddley = std::sqrt(var_paddley / n);

    // Prevent division by zero
    std_bally = std_bally > 0 ? std_bally : 1.0;
    std_paddley = std_paddley > 0 ? std_paddley : 1.0;
}

// Normalize input
std::vector<double> PongNeuralNetwork::normalize_input(const GameState& state)
{
    return {
        (state.bally - mean_bally) / std_bally,
        (state.paddley - mean_paddley) / std_paddley
    };
}




// Public Net functions ---------------------------------------------------

// Predict optimal paddle movement based on state
int PongNeuralNetwork::predict_move(GameState gamestate)
{
    GameState state{static_cast<double>(gamestate.bally),
                    static_cast<double>(gamestate.paddley)};
    
    std::vector<double> input = normalize_input(state);
    std::vector<double> output = forward_propagate(input); 

    int movement = std::max_element(output.begin(), output.end()) - output.begin();
    return movement;
}

// Training the neural network
void PongNeuralNetwork::train(const std::vector<GameState>& training_data, const std::vector<int>& expected_moves, double learning_rate, int epochs)
{
    compute_normalization_params(training_data);

    for (int epoch = 0; epoch < epochs; ++epoch)
    {
        double total_loss = 0.0;
        double max_gradient = 0.0;
        double min_gradient = std::numeric_limits<double>::max();

        // Shuffle the training data
        std::vector<int> indices(training_data.size());
        std::iota(indices.begin(), indices.end(), 0);
        std::shuffle(indices.begin(), indices.end(), gen);

        for (size_t i = 0; i < training_data.size(); ++i)
        {
            int index = indices[i];
            std::vector<double> input = normalize_input(training_data[index]);
            std::vector<double> output = forward_propagate(input); 

            // Compute target
            std::vector<double> target(output.size(), 0.0);
            target[expected_moves[index]] = 1.0;

            // Compute loss
            std::vector<double> loss(output.size());
            for (size_t j = 0; j < output.size(); ++j)
            {
                loss[j] = -target[j] * std::log(std::max(output[j], 1e-15));
                total_loss += loss[j];
            }

            // Compute gradients
            std::vector<double> gradient = output;
            for (size_t j = 0; j < output.size(); ++j)
            {
                gradient[j] -= target[j];
                
                // Track gradient magnitude
                max_gradient = std::max(max_gradient, std::abs(gradient[j]));
                min_gradient = std::min(min_gradient, std::abs(gradient[j]));
            }

            // Backpropagate and update weights and biases
            backpropagate(input, gradient, learning_rate);
        }

        std::cout << "Epoch " << epoch + 1 << "/" << epochs << " - Loss: " << total_loss / training_data.size() << " - Max Gradient: " << max_gradient << " - Min Gradient: " << min_gradient << std::endl;
    }
}