/*Problem Statement:
Design a signal distribution block with a single input and dual outputs
See next pages for the specification targets and available components
Minimize cost while satisfying the specification targets 

Expected outcome
Block Diagram
Achieved performance and cost */

#include <iostream>
#include <vector>
#include <limits>
#include <string>

using namespace std;

// Base class for components
class Component {
public:
    virtual double getCost() const = 0; // Pure virtual function to get cost
};

// Amplifier class
class Amplifier : public Component {
public:
    string name;
    double gain_min_1GHz;
    double gain_max_1GHz;
    double gain_min_20GHz;
    double gain_max_20GHz;
    double P1dB_1GHz;
    double P1dB_20GHz;
    double cost;

    Amplifier(string n, double g_min_1, double g_max_1, double g_min_20, double g_max_20, double p1dB_1, double p1dB_20, double c) 
        : name(n), gain_min_1GHz(g_min_1), gain_max_1GHz(g_max_1), gain_min_20GHz(g_min_20), gain_max_20GHz(g_max_20), 
        P1dB_1GHz(p1dB_1), P1dB_20GHz(p1dB_20), cost(c) {}

    double getCost() const override {
        return cost;
    }
};

// Switch class
class Switch : public Component {
public:
    string name;
    double gain_1GHz_typ;
    double gain_20GHz_typ;
    double leakage_1GHz;
    double leakage_20GHz;
    double P1dB_input;
    double cost;

    Switch(string n, double g_1, double g_20, double leak_1, double leak_20, double p1dB, double c) 
        : name(n), gain_1GHz_typ(g_1), gain_20GHz_typ(g_20), leakage_1GHz(leak_1), leakage_20GHz(leak_20), 
        P1dB_input(p1dB), cost(c) {}

    double getCost() const override {
        return cost;
    }
};

// Attenuator class
class Attenuator : public Component {
public:
    string name;
    double gain; // Negative value for attenuation
    double max_attenuation;
    double P1dB_input;
    double cost;

    Attenuator(string n, double g, double max_att, double p1dB) 
        : name(n), gain(g), max_attenuation(max_att), P1dB_input(p1dB), cost(0.0) {}

    double getCost() const override {
        return cost;
    }
};

// Power Divider class
class PowerDivider : public Component {
public:
    string name;
    double gain_1GHz;
    double gain_20GHz;
    double cost;

    PowerDivider(string n, double g_1, double g_20) 
        : name(n), gain_1GHz(g_1), gain_20GHz(g_20), cost(0.0) {}

    double getCost() const override {
        return cost;
    }
};

/*Logics and calculations*/
class ConfigurationFinder {
public:
    static void printBestConfiguration(const  vector<Amplifier>& amplifiers, 
                                        const  vector<Switch>& switches, 
                                        const  vector<Attenuator>& attenuators, 
                                        const  vector<PowerDivider>& dividers) {
        double min_cost =  numeric_limits<double>::max();
        string best_combination;

        // Assume input power Pin = 0 dBm
        double input_power_dBm = 0;

        // Iterate over all combinations of amplifier, switch, attenuator, and power divider
        for (const auto& amp : amplifiers) {
            for (const auto& sw : switches) {
                for (const auto& att : attenuators) {
                    for (const auto& div : dividers) {
                        double total_cost = amp.getCost() + sw.getCost() + att.getCost() + div.getCost(); // Cost of attenuator is $0

                        // Calculate effective gains
                        double effective_gain_max_1GHz = amp.gain_max_1GHz + att.gain + div.gain_1GHz; // Maximum gain at 1 GHz
                        double effective_gain_max_20GHz = amp.gain_max_20GHz + att.gain + div.gain_20GHz; // Maximum gain at 20 GHz

                        double effective_gain_min_1GHz = amp.gain_min_1GHz + att.gain + div.gain_1GHz; // Minimum gain at 1 GHz
                        double effective_gain_min_20GHz = amp.gain_min_20GHz +
                        att.gain + div.gain_20GHz; // Minimum gain at 20 GHz

                        // Calculate max output power at 1 GHz and 20 GHz
                        double max_output_power_1GHz = input_power_dBm + effective_gain_max_1GHz; 
                        double max_output_power_20GHz = input_power_dBm + effective_gain_max_20GHz; 

                        // Calculate min output power at 1 GHz and 20 GHz
                        double min_output_power_1GHz = input_power_dBm + effective_gain_min_1GHz; 
                        double min_output_power_20GHz = input_power_dBm + effective_gain_min_20GHz; 

                        // Check ON state specifications
                        if (max_output_power_1GHz <= amp.P1dB_1GHz && max_output_power_20GHz <= amp.P1dB_20GHz &&
                            sw.gain_1GHz_typ >= -1.0 && sw.gain_20GHz_typ >= -2.0 &&
                            amp.P1dB_1GHz >= 12 && amp.P1dB_20GHz >= 10.5) {
                            
                            // Check OFF state specifications
                            if (sw.leakage_1GHz < -55 && sw.leakage_20GHz < -20) {
                                
                                // Check if the attenuator gain does not exceed its max attenuation
                                if (-att.gain <= att.max_attenuation) { // Attenuation is a negative value
                                    // If all conditions are met, check if this is the best configuration
                                    if (total_cost < min_cost) {
                                        min_cost = total_cost;
                                        best_combination = "Amplifier: " + amp.name + ", Switch: " + sw.name + 
                                                        ", Attenuator: " + att.name + ", Power Divider: " + div.name;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        // Output the best configuration found
        if (!best_combination.empty()) {
            cout << "*******Best Configuration:*******"<<endl;
            cout << best_combination << " with total cost: $" << min_cost <<  endl;
        } else {
            cout << "No valid configuration found that meets the specifications." <<  endl;
        }
    }
};

int main() {
    // From the given example data for amplifiers, switches, attenuators, and power dividers
    vector<Amplifier> amplifiers = {
        {"Amp-A", 19.0, 15.0, 18.0, 14.0, 12.0, 10.5, 100.0},
        {"Amp-B", 18.0, 14.0, 17.0, 13.0, 11.0, 9.5, 120.0}
    };

    vector<Switch> switches = {
        {"Switch-1", 0.5, 0.4, -60.0, -25.0, 15.0, 50.0},
        {"Switch-2", 0.6, 0.5, -58.0, -22.0, 14.0, 60.0}
    };

    vector<Attenuator> attenuators = {
        {"Attenuator-1", -3.0, 10.0, 20.0},
        {"Attenuator-2", -6.0, 15.0, 20.0}
    };

    vector<PowerDivider> dividers = {
        {"Divider-1", 0.0, 0.0},
        {"Divider-2", 0.0, 0.0}
    };

    // Find and print the best configuration
    ConfigurationFinder::printBestConfiguration(amplifiers, switches, attenuators, dividers);

    return 0;
}
