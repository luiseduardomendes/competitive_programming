#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip> // For std::fixed and std::setprecision
#include <algorithm> // For std::max
#include <limits>    // For std::numeric_limits

// Using std::pair for simplicity
typedef std::pair<double, double> pdd;

// Structure to represent a flip-flop with its original properties
struct FlipFlop {
    int id;
    pdd original_pos;
    int cluster_id; // Assigned cluster ID
};

// Manhattan distance cost
double cost(pdd a, pdd b) {
    return std::abs(b.first - a.first) + std::abs(b.second - a.second);
}

// Calculate the mean point of a set of points
pdd mean_point(const std::vector<FlipFlop*>& flops_in_cluster) {
    if (flops_in_cluster.empty()) {
        return {0.0, 0.0}; // Should not happen in well-behaved K-means with proper initialization,
                           // but defensive coding. A cluster should ideally not become truly empty.
    }
    double x_coord_sum = 0;
    double y_coord_sum = 0;
    for (const auto& flop_ptr : flops_in_cluster) {
        x_coord_sum += flop_ptr->original_pos.first;
        y_coord_sum += flop_ptr->original_pos.second;
    }
    return {x_coord_sum / flops_in_cluster.size(), y_coord_sum / flops_in_cluster.size()};
}

// Main clustering function
void solve() {
    int N, K, SIZE_LIMIT, DISP_LIMIT;
    std::cin >> N >> K >> SIZE_LIMIT >> DISP_LIMIT;

    int MAX_ITERATIONS;
    std::cin >> MAX_ITERATIONS; 

    std::vector<FlipFlop> all_flops(N);
    for (int i = 0; i < N; ++i) {
        all_flops[i].id = i;
        std::cin >> all_flops[i].original_pos.first >> all_flops[i].original_pos.second;
    }

    // Initialize K cluster centers
    std::vector<pdd> cluster_centers(K);
    for (int i = 0; i < K; ++i) {
        cluster_centers[i] = all_flops[i].original_pos; // Simplest initialization: first K flops
    }

    // Data structure to hold current cluster assignments for dynamic weighting
    std::vector<std::vector<FlipFlop*>> clusters(K);

    for (int iter = 0; iter < MAX_ITERATIONS; ++iter) {
        // Clear clusters for current iteration's assignments
        for (int i = 0; i < K; ++i) {
            clusters[i].clear();
        }

        // Assignment Step (Weighted K-means)
        for (auto& flop : all_flops) {
            double min_weighted_cost = std::numeric_limits<double>::max();
            int best_cluster_idx = -1;

            for (int k = 0; k < K; ++k) {
                double original_dist = cost(flop.original_pos, cluster_centers[k]);
                // Apply the weight based on current cluster size
                double current_cluster_size = static_cast<double>(clusters[k].size());
                double weight = std::max(1.0, current_cluster_size / SIZE_LIMIT);
                double weighted_cost = original_dist * weight;

                if (weighted_cost < min_weighted_cost) {
                    min_weighted_cost = weighted_cost;
                    best_cluster_idx = k;
                }
            }
            flop.cluster_id = best_cluster_idx;
            clusters[best_cluster_idx].push_back(&flop); // Add flop pointer to the chosen cluster
        }

        // Update Step: Recalculate cluster centers
        for (int k = 0; k < K; ++k) {
            if (!clusters[k].empty()) {
                cluster_centers[k] = mean_point(clusters[k]);
            }
            // If a cluster becomes empty, its center remains unchanged from the previous iteration.
            // This is a common strategy to prevent "dead" clusters.
        }
    }

    // Constraint Reporting (Post-Clustering)
    int clusters_over_size_limit = 0;
    for (int k = 0; k < K; ++k) {
        if (clusters[k].size() > SIZE_LIMIT) {
            clusters_over_size_limit++;
        }
    }

    int flops_over_disp_limit = 0;
    for (const auto& flop : all_flops) {
        // Ensure flop has a valid cluster_id before calculating cost
        if (flop.cluster_id >= 0 && flop.cluster_id < K) {
            double displacement = cost(flop.original_pos, cluster_centers[flop.cluster_id]);
            if (displacement > DISP_LIMIT) {
                flops_over_disp_limit++;
            }
        }
    }

    // Output results
    std::cout << K << std::endl;
    std::cout << std::fixed << std::setprecision(2); // Set precision for coordinates

    for (const auto& center : cluster_centers) {
        std::cout << center.first << " " << center.second << std::endl;
    }

    std::cout << clusters_over_size_limit << std::endl;
    std::cout << flops_over_disp_limit << std::endl;

    for (const auto& flop : all_flops) {
        std::cout << flop.id << " " << flop.cluster_id << std::endl;
    }
}

int main() {
    std::ios_base::sync_with_stdio(false); // Optimize C++ streams for competitive programming
    std::cin.tie(NULL);
    solve();
    return 0;
}