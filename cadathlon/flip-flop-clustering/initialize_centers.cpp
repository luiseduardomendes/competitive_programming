
#include <vector>
#include <cmath>
#include <limits> // For numeric_limits
#include <map>    // To store clusters

#include <algorithm> // For std::sort
#include <functional> // For std::function (if needed for custom sort criteria)
#include <utility>
#include <vector>
#include <cmath>
#define DISP_LIMIT 60 // As stated in the paper [cite: 280]
#define SIZE_LIMIT 80 // As stated in the paper [cite: 280]

using namespace std;

typedef long long ll;
typedef pair<double, double> pdd;

vector<pdd> cluster_center;

// manhattan cost
double cost(pdd a, pdd b){
    double ax = a.first;
    double ay = a.second;
    double bx = b.first;
    double by = b.second;
    return abs(by - ay) + abs(bx - ax);
}

double calc_cluster_cost(const vector<pdd>& Ck_flops, pdd pt_mean) { // Renamed C to Ck_flops for clarity
    double acc_cost = 0;
    for (const pdd& p : Ck_flops) { // Iterate over pdd objects
        acc_cost += cost(p, pt_mean);
    }
    // The weight is applied in the assignment step, not when calculating the total cost for the objective function.
    // The formula in the paper for cost is Cost = |xi - μx(Ck)| + |yi - μy(Ck)|,
    // and the weighted cost for assignment is Cost * max((|Ck|/size_limit),1)[cite: 162, 214].
    // For the overall objective function, it's just the sum of Manhattan distances[cite: 106].
    return acc_cost;
}

pdd mean_point(const vector<pdd>& S) {
    double x_coord_sum = 0;
    double y_coord_sum = 0;
    for (const pdd& p : S) {
        x_coord_sum += p.first;
        y_coord_sum += p.second;
    }
    if (S.empty()) return make_pair(0.0, 0.0); // Handle empty set to avoid division by zero
    return make_pair(x_coord_sum / S.size(), y_coord_sum / S.size());
}

void split(vector<pdd>& S, vector<pdd>& S1, vector<pdd>& S2, ll K, bool horizontal_split) {
    // Clear S1 and S2 to ensure they are empty before population
    S1.clear();
    S2.clear();

    ll size1 = S.size() * floor(K / 2.0) / K; // Use 2.0 for float division
    // ll size2 = S.size() - size1; // The remaining goes to S2. This simplifies calculation if S.size() is not perfectly divisible.

    // Sort based on x or y coordinates
    if (horizontal_split) { // Sort by x-coordinate for vertical partition
        sort(S.begin(), S.end(), [](pdd a, pdd b) {
            return a.first < b.first;
        });
    } else { // Sort by y-coordinate for horizontal partition
        sort(S.begin(), S.end(), [](pdd a, pdd b) {
            return a.second < b.second;
        });
    }

    for (int i = 0; i < size1; i++) {
        S1.push_back(S[i]);
    }
    for (int i = size1; i < S.size(); i++) { // Correctly populate S2
        S2.push_back(S[i]);
    }
}

void initCenter(vector<pdd> S, ll K, bool horizontal_split) { // Add horizontal_split flag
    if (S.empty()) return; // Handle empty set
    if (S.size() <= SIZE_LIMIT) { // [cite: 149]
        cluster_center.push_back(mean_point(S));
        return;
    }

    vector<pdd> S1, S2;
    split(S, S1, S2, K, horizontal_split); // Pass the split direction

    initCenter(S1, floor(K / 2.0), !horizontal_split); // Toggle split direction
    initCenter(S2, ceil(K / 2.0), !horizontal_split);   // Corrected: S2
}

// Assume pdd, cost, mean_point, initCenter, split functions are defined as above and corrected.

// A structure to hold flip-flop data, potentially including its ID and original position
struct FlipFlop {
    int id;
    pdd original_pos;
    pdd current_pos; // This will change as flops are relocated, but not during clustering directly
    int cluster_id; // To which cluster it's assigned
    // You might also add timing criticality if needed for relocation order [cite: 266]
};

// Main clustering function
void flip_flop_clustering(vector<FlipFlop>& flops) {
    // Step 1: Initialize K cluster centers [cite: 118, 133]
    // The paper sets K initially based on |S|/size_limit[cite: 149].
    // Let's assume K_VALUE is derived from this initial estimation, or provided.
    // Call initCenter with all flops and the initial K
    cluster_center.clear(); // Ensure it's empty
    initCenter(vector<pdd>(flops.begin(), flops.end()), K_VALUE, true); // Assuming initial split is horizontal

    // Store the assignment of flops to clusters (e.g., map cluster_id to a list of flop IDs)
    // A more direct approach for K-means is to store the cluster ID in each FlipFlop object
    // and then rebuild the clusters based on these IDs.
    map<int, vector<FlipFlop*>> clusters; // Map cluster_idx to vector of pointers to flops in that cluster

    double previous_total_cost = numeric_limits<double>::max();
    int iterations_without_improvement = 0;
    const int MAX_ITERATIONS_WITHOUT_IMPROVEMENT = 10; // As stated in the paper [cite: 282]
    const int OVERFLOW_RESOLUTION_INTERVAL = 5; // As stated in the paper [cite: 282]

    for (int iter = 0; ; ++iter) { // Loop until convergence [cite: 97]
        // Clear clusters for current iteration
        for (int i = 0; i < cluster_center.size(); ++i) {
            clusters[i].clear();
        }

        // Step 2: Assign flip-flops to clusters 
        // Implement weighted K-means assignment
        double current_total_cost = 0;
        for (auto& flop : flops) {
            double min_cost = numeric_limits<double>::max();
            int best_cluster_idx = -1;

            for (int k = 0; k < cluster_center.size(); ++k) {
                // Calculate original cost (Manhattan distance) [cite: 162]
                double original_cost = cost(flop.original_pos, cluster_center[k]);
                // Apply the weight 
                double weighted_cost = original_cost * max(1.0, (double)clusters[k].size() / SIZE_LIMIT); // Cast to double

                if (weighted_cost < min_cost) {
                    min_cost = weighted_cost;
                    best_cluster_idx = k;
                }
            }
            // Update cluster assignment immediately to affect weights for subsequent flops 
            flop.cluster_id = best_cluster_idx;
            clusters[best_cluster_idx].push_back(&flop); // Add flop pointer to the cluster
            current_total_cost += cost(flop.original_pos, cluster_center[best_cluster_idx]); // Use unweighted cost for total objective [cite: 106]
        }

        // Step 3: Update cluster centers 
        for (int k = 0; k < cluster_center.size(); ++k) {
            if (!clusters[k].empty()) {
                vector<pdd> current_cluster_flops_pos;
                for (const auto& flop_ptr : clusters[k]) {
                    current_cluster_flops_pos.push_back(flop_ptr->original_pos);
                }
                cluster_center[k] = mean_point(current_cluster_flops_pos);
            }
        }

        // Step 4: Resolve overflow (every certain iterations) [cite: 121, 229, 233]
        if (iter % OVERFLOW_RESOLUTION_INTERVAL == 0 && iter > 0) {
            bool overflow_resolved_in_iter = false;
            // Iterate and find clusters violating size constraint
            for (int k = 0; k < cluster_center.size(); ++k) {
                if (clusters[k].size() > SIZE_LIMIT) { // Cluster is overflowed [cite: 231]
                    // Pick the cluster with most flops among violating clusters (simple for loop might do this implicitly)
                    // Insert a new center near the center of this cluster [cite: 234]
                    // This implies increasing K dynamically. You'd need to add a new center to `cluster_center`.
                    pdd new_center_pos = cluster_center[k]; // Simple start: same as overflowing cluster
                    // You might want to slightly perturb it to avoid immediate re-merging or for better distribution
                    new_center_pos.first += 1.0; // Example perturbation

                    cluster_center.push_back(new_center_pos);
                    int new_cluster_idx = cluster_center.size() - 1;
                    clusters[new_cluster_idx] = vector<FlipFlop*>(); // Initialize new empty cluster

                    // Reassign flops from the overflowed cluster to the new cluster if cost improves [cite: 235]
                    vector<FlipFlop*> overflowed_flops = clusters[k];
                    clusters[k].clear(); // Temporarily clear the original cluster

                    for (auto& flop_ptr : overflowed_flops) {
                        double cost_to_original = cost(flop_ptr->original_pos, cluster_center[k]);
                        double cost_to_new = cost(flop_ptr->original_pos, cluster_center[new_cluster_idx]);

                        // Re-evaluate with current weights
                        double weighted_cost_to_original = cost_to_original * max(1.0, (double)clusters[k].size() / SIZE_LIMIT);
                        double weighted_cost_to_new = cost_to_new * max(1.0, (double)clusters[new_cluster_idx].size() / SIZE_LIMIT);

                        if (weighted_cost_to_new < weighted_cost_to_original && clusters[new_cluster_idx].size() < SIZE_LIMIT) { // Check size limit for new cluster too
                            flop_ptr->cluster_id = new_cluster_idx;
                            clusters[new_cluster_idx].push_back(flop_ptr);
                        } else {
                            flop_ptr->cluster_id = k;
                            clusters[k].push_back(flop_ptr);
                        }
                        // Update weights (cluster sizes) implicitly through push_back/clear
                    }
                    overflow_resolved_in_iter = true;
                    // The paper says "The K-means iteration continues until all the clusters satisfy the size constraints
                    // and there is no improvement on costs of all the flip-flops within certain iterations." [cite: 237]
                    // This implies if overflow is resolved, continue with more K-means iterations.
                }
            }
            if (overflow_resolved_in_iter) {
                // If overflow was resolved, it's a significant change, reset improvement counter
                iterations_without_improvement = 0;
            }
        }

        // Convergence check 
        if (abs(previous_total_cost - current_total_cost) < 1e-6) { // Small epsilon for convergence
            iterations_without_improvement++;
            if (iterations_without_improvement >= MAX_ITERATIONS_WITHOUT_IMPROVEMENT) {
                break; // Converged
            }
        } else {
            iterations_without_improvement = 0;
        }
        previous_total_cost = current_total_cost;
    }

    // After the main loop: Resolve over displacement (post-processing) 
    // Iterate through all flops and check displacement constraint
    for (auto& flop : flops) {
        double displacement = cost(flop.original_pos, cluster_center[flop.cluster_id]);
        if (displacement > DISP_LIMIT) { // If over displacement [cite: 109]
            // Insert a new cluster centered at the violating flip-flop's original location [cite: 245]
            cluster_center.push_back(flop.original_pos);
            int new_cluster_idx = cluster_center.size() - 1;
            flop.cluster_id = new_cluster_idx; // Assign violating flop to new cluster [cite: 246]
            clusters[new_cluster_idx].push_back(&flop); // Add flop pointer to new cluster

            // Reassign nearby flops to this new cluster if cost improves [cite: 247]
            // This would require another mini-assignment pass around the new cluster and its neighbors.
            // For simplicity in initial implementation, you might skip this re-assignment or limit it to closest neighbors.
            // This is a complex step, as it involves finding 'nearby' flops and potentially moving them
            // from existing clusters, which might affect their costs.
        }
    }

    // The output of this stage is the assigned clusters and their centers.
    // The actual "relocation" of flops happens in the next step.
}

// You'd also need a way to pass in the initial K_VALUE or calculate it based on total flops and SIZE_LIMIT[cite: 149].

int main(){}