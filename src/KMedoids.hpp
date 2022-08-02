#ifndef KMEDOIDS_HPP
#define KMEDOIDS_HPP

#include <vector>

#include "Matrix.hpp"
#include "MyRandom.hpp"
#include "utility.hpp"

template <typename DistanceMap>
std::vector<int> getKMedoids(
    const int _k, const DistanceMap& _dist, const int _tMax = 100) {

    std::vector<std::vector<int>> clusters(_k);
    // Init medoids
    std::vector<int> medoids =
        MyRandom::getInstance().getKShuffled<int>(_k, 0u, _dist.shape()[1] - 1);
    bool changed = true;
    for (int t = 0; t < _tMax && changed; ++t) {
        changed = false;
        for (auto& vect : clusters) {
            vect.clear();
        }
        // For each elements, find the closest medoid and assign the element to
        // the cluster of the corresponding medoid
        for (int i = 0; i < _dist.shape()[0]; ++i) {
            int minInd = 0;
            for (int j = 1; j < medoids.size(); ++j) {
                if (_dist[i][medoids[j]] < _dist[i][medoids[minInd]]) {
                    minInd = j;
                }
            }
            clusters[minInd].push_back(i);
        }
        // Search medoid for each cluster
        for (int c = 0; c < clusters.size(); ++c) {
            // Compute distance between all elements of the cluster
            if (clusters[c].empty()) {
                if (medoids[c] != -1) {
                    medoids[c] = -1;
                    changed = true;
                }
                continue;
            }
            int minDistInd = 0;
            double minDist =
                std::accumulate(clusters[c].begin(), clusters[c].end(), 0.0,
                    [&](const double __tmpDist, const int __elj) {
                        return __tmpDist + _dist[clusters[c][0]][__elj];
                    });
            for (int i = 1; i < clusters[c].size(); ++i) {
                double distance = 0.0;
                for (int j = 0; j < clusters[c].size(); ++j) {
                    distance += _dist[clusters[c][i]][clusters[c][j]];
                    if (distance > minDist) {
                        break;
                    }
                }
                if (distance < minDist) {
                    minDistInd = i;
                }
            }
            if (medoids[c] != clusters[c][minDistInd]) {
                changed = true;
            }
            medoids[c] = clusters[c][minDistInd];
        }
    }
    return medoids;
}

template <typename DistanceMap>
std::vector<int> getKMedoids(const int _k, const DistanceMap& _dist,
    const std::vector<double>& _capas, const std::vector<double>& _charge,
    const int _tMax = 100) {
    CppRO_ASSERT(_capas.size() == _dist.shape()[0]);
    CppRO_ASSERT(_charge.size() == _dist.shape()[0]);
    CppRO_ASSERT(_dist.shape()[0] == _dist.shape()[1]);

    std::vector<std::vector<int>> clusters(_k);
    // Init medoids
    std::vector<int> medoids =
        MyRandom::getInstance().getKShuffled<int>(_k, 0, _dist.shape()[1] - 1);
    std::vector<double> usedCapas(_dist.shape()[1], 0.0);
    bool changed = true;
    for (int t = 0; t < _tMax && changed; ++t) {
        std::fill(usedCapas.begin(), usedCapas.end(), 0.0);
        changed = false;
        for (auto& vect : clusters) {
            vect.clear();
        }
        // For each element, find the closest medoid and assign the element to
        // the cluster of the corresponding medoid
        for (int i = 0; i < _dist.shape()[0]; ++i) {
            int minInd = 0;
            for (int j = 1; j < medoids.size(); ++j) {
                if (_dist[i][medoids[j]] < _dist[i][medoids[minInd]]
                    &&                          // Update closest medoid if
                                                // distance is smaller..
                    usedCapas[j] < _capas[j]) { // and still as available capa
                    minInd = j;
                }
            }
            clusters[minInd].push_back(i);
            usedCapas[minInd] -= _charge[i];
        }
        // Search medoid for each cluster
        for (int c = 0; c < clusters.size(); ++c) {
            // Compute distance between all elements of the cluster
            if (clusters[c].empty()) {
                if (medoids[c] != -1) {
                    medoids[c] = -1;
                    changed = true;
                }
                continue;
            }
            int minDistInd = 0;
            double minDist =
                std::accumulate(clusters[c].begin(), clusters[c].end(), 0.0,
                    [&](const double __tmpDist, const int __elj) {
                        return __tmpDist + _dist[clusters[c][0]][__elj];
                    });

            for (int i = 1; i < clusters[c].size(); ++i) {
                double distance = 0.0;
                for (int j = 0; j < clusters[c].size(); ++j) {
                    distance += _dist[clusters[c][i]][clusters[c][j]];
                    if (distance > minDist) {
                        break;
                    }
                }
                if (distance < minDist) {
                    minDistInd = i;
                }
            }

            if (medoids[c]
                != clusters[c][minDistInd]) { // Medoids of cluster c is changed
                changed = true;
                medoids[c] = clusters[c][minDistInd];
            }
        }
    }
    return medoids;
}

#endif
