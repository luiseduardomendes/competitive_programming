#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <utility>
#include <iomanip>
#include <string>
#include <queue>
#include <map>
#include <set>

using namespace std;

// --- Data Structures ---

/**
 * @struct state_t
 * @brief Represents a partial solution state for the DP algorithm (Definition 8).
 * A state is defined by the set of segments used and the orientation of the last segment.
 */
struct state_t {
    vector<int> used_H_indices; // Sorted indices of used horizontal segments
    vector<int> used_V_indices; // Sorted indices of used vertical segments
    char last_orientation;      // 'H' for horizontal, 'V' for vertical, 'N' for none

    // Operator< is required to use this struct as a key in std::map.
    // It defines a canonical ordering for states.
    bool operator<(const state_t& other) const {
        if (used_H_indices != other.used_H_indices) return used_H_indices < other.used_H_indices;
        if (used_V_indices != other.used_V_indices) return used_V_indices < other.used_V_indices;
        return last_orientation < other.last_orientation;
    }
};

// --- Core Algorithm Functions ---

/**
 * @brief Generates the matching configuration matrix (CmH or CmV) from Definition 7.
 */
vector<vector<int>> generate_config_matrix(const vector<double>& D) {
    size_t n = D.size();
    vector<vector<int>> C(n, vector<int>(n));
    auto sign = [](double val) { return (val > 0) - (val < 0); };
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            if (i >= j) C[i][j] = sign(D[i]);
            else C[i][j] = -sign(D[i]);
        }
    }
    return C;
}

/**
 * @brief Prints a matrix to the console for visualization.
 */
void print_matrix(const vector<vector<int>>& matrix, const string& name) {
    cout << name << ":" << endl;
    for (const auto& row : matrix) {
        for (int val : row) {
            cout << setw(3) << val;
        }
        cout << endl;
    }
}

/**
 * @brief Calculates segment lengths based on a distance vector (D_H or D_V) using Lemma 3.
 */
vector<double> calculate_segment_lengths(vector<double> D) {
    if (D.empty()) return {};
    sort(D.begin(), D.end(), [](double a, double b) { return abs(a) < abs(b); });
    size_t n = D.size();
    vector<double> L(n);
    if (n == 1) {
        L[0] = abs(D[0]);
        return L;
    }
    for (size_t i = 1; i < n; ++i) L[i] = (abs(D[i]) - abs(D[i-1])) / 2.0;
    L[0] = (abs(D[0]) + abs(D[n-1])) / 2.0;
    return L;
}

/**
 * @brief Finds the optimal segment ordering using a DP/Dijkstra-like algorithm.
 * This function implements the logic from Figure 7 of the paper.
 */
void find_optimal_ordering(const vector<double>& L_H, const vector<double>& L_V) {
    cout << "\n--- Finding Optimal Segment Ordering (DP Algorithm) ---" << endl;

    // A min-priority queue to store {cost, state}.
    priority_queue<pair<double, state_t>, vector<pair<double, state_t>>, greater<pair<double, state_t>>> pq;

    // Maps to track costs and reconstruct the path.
    map<state_t, double> cost_map;
    map<state_t, pair<state_t, pair<char, int>>> parent_map;

    // Initial state: no segments used, no last orientation.
    state_t initial_state = {{}, {}, 'N'};
    pq.push({0.0, initial_state});
    cost_map[initial_state] = 0.0;

    state_t final_state;
    bool found_solution = false;

    while (!pq.empty()) {
        double current_cost = pq.top().first;
        state_t current_state = pq.top().second;
        pq.pop();

        // If we found a cheaper path to this state already, skip.
        if (current_cost > cost_map[current_state]) {
            continue;
        }

        // Check for goal state: all segments have been used.
        if (current_state.used_H_indices.size() == L_H.size() && current_state.used_V_indices.size() == L_V.size()) {
            final_state = current_state;
            found_solution = true;
            break;
        }

        // --- Expand to next states ---

        // Try adding a horizontal segment if the last was not horizontal.
        if (current_state.last_orientation != 'H') {
            for (int i = 0; i < L_H.size(); ++i) {
                // Check if this segment is already used.
                if (find(current_state.used_H_indices.begin(), current_state.used_H_indices.end(), i) == current_state.used_H_indices.end()) {
                    
                    state_t next_state = current_state;
                    next_state.used_H_indices.push_back(i);
                    sort(next_state.used_H_indices.begin(), next_state.used_H_indices.end());
                    next_state.last_orientation = 'H';

                    // Placeholder cost: in a real scenario, this would check grid congestion.
                    double new_cost = current_cost + L_H[i]; // Cost could be length, congestion, etc.

                    if (cost_map.find(next_state) == cost_map.end() || new_cost < cost_map[next_state]) {
                        cost_map[next_state] = new_cost;
                        pq.push({new_cost, next_state});
                        parent_map[next_state] = {current_state, {'H', i}};
                    }
                }
            }
        }

        // Try adding a vertical segment if the last was not vertical.
        if (current_state.last_orientation != 'V') {
            for (int i = 0; i < L_V.size(); ++i) {
                 if (find(current_state.used_V_indices.begin(), current_state.used_V_indices.end(), i) == current_state.used_V_indices.end()) {
                    
                    state_t next_state = current_state;
                    next_state.used_V_indices.push_back(i);
                    sort(next_state.used_V_indices.begin(), next_state.used_V_indices.end());
                    next_state.last_orientation = 'V';

                    double new_cost = current_cost + L_V[i];

                    if (cost_map.find(next_state) == cost_map.end() || new_cost < cost_map[next_state]) {
                        cost_map[next_state] = new_cost;
                        pq.push({new_cost, next_state});
                        parent_map[next_state] = {current_state, {'V', i}};
                    }
                }
            }
        }
    }

    // --- Backtrack to find and print the path ---
    if (found_solution) {
        cout << "Optimal solution found with total cost: " << cost_map[final_state] << endl;
        cout << "Optimal segment order (from driver to receiver):" << endl;
        
        vector<pair<char, double>> path;
        state_t current = final_state;

        while (parent_map.count(current)) {
            auto p = parent_map[current];
            char type = p.second.first;
            int index = p.second.second;
            double length = (type == 'H') ? L_H[index] : L_V[index];
            path.push_back({type, length});
            current = p.first;
        }
        reverse(path.begin(), path.end());

        for(const auto& p : path) {
            cout << "  -> " << (p.first == 'H' ? "Horizontal" : "Vertical") << " segment of length " << p.second << endl;
        }
    } else {
        cout << "No solution found." << endl;
    }
}


int main() {
    cout << "Running test case from Figure 4..." << endl;

    vector<double> D_H_original = {6.0, 10.0, 18.0};
    vector<double> D_V_original = {5.0, 5.0, 5.0}; // Simplified: only one unique vertical distance

    cout << "\nInput Distances:" << endl;
    cout << "D_H: "; for(double d : D_H_original) cout << d << " ";
    cout << "\nD_V: "; for(double d : D_V_original) cout << d << " "; cout << endl;

    // --- Calculate L_H and L_V ---
    vector<double> L_H = calculate_segment_lengths(D_H_original);
    vector<double> L_V = calculate_segment_lengths(D_V_original);

    cout << "\n--- Calculated Segment Lengths ---" << endl;
    cout << "L_H (Horizontal): "; for (const auto& len : L_H) cout << len << " "; cout << endl;
    cout << "L_V (Vertical):   "; for (const auto& len : L_V) cout << len << " "; cout << endl;
    
    // --- Find Optimal Ordering ---
    find_optimal_ordering(L_H, L_V);

    return 0;
}
