#ifndef KMEDOIDS_HPP
#define KMEDOIDS_HPP

#include <vector>

#include "Matrix.hpp"
#include "MyRandom.hpp"
#include "utility.hpp"

std::vector<std::size_t> getKMedoids(const std::size_t _k, const Matrix<double>& _dist,
    const std::size_t _tMax = 100) {
    assert(_dist.size1() == _dist.size2());
    assert(_dist.size1() > 0);
    assert(_dist.size2() > 0);
    std::vector<std::vector<std::size_t>> clusters(_k);
    // Init medoids
    std::vector<std::size_t> medoids =
        MyRandom::getInstance().getKShuffled<std::size_t>(_k, 0u, _dist.size2() - 1);
    bool changed = true;
    for (std::size_t t = 0; t < _tMax && changed; ++t) {
        changed = false;
        for (auto& vect : clusters) {
            vect.clear();
        }
        // For each elements, find the closest medoid and assign the element to the
        // cluster of the corresponding medoid
        for (std::size_t i = 0; i < _dist.size1(); ++i) {
            std::size_t minInd = 0;
            for (std::size_t j = 1; j < medoids.size(); ++j) {
                if (_dist(i, medoids[j]) < _dist(i, medoids[minInd])) {
                    minInd = j;
                }
            }
            clusters[minInd].push_back(i);
        }
        // Search medoid for each cluster
        for (std::size_t c = 0; c < clusters.size(); ++c) {
            // Compute distance between all elements of the cluster
            std::size_t minDistInd = 0;
            double minDist =
                std::accumulate(clusters[c].begin(), clusters[c].end(), 0.0,
                    [&](const double __tmpDist, const std::size_t __elj) {
                        return __tmpDist + _dist(clusters[c][0], __elj);
                    });
            for (std::size_t i = 1; i < clusters[c].size(); ++i) {
                double distance = 0.0;
                for (std::size_t j = 0; j < clusters[c].size(); ++j) {
                    distance += _dist(clusters[c][i], clusters[c][j]);
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

std::vector<std::size_t> getKMedoids(const std::size_t _k, const Matrix<double>& _dist,
    const std::vector<std::size_t>& _capas,
    const std::vector<double>& _charge,
    const std::size_t _tMax = 100) {
    assert(_capas.size() == _dist.size1());
    assert(_charge.size() == _dist.size1());
    assert(_dist.size1() == _dist.size2());

    std::vector<std::vector<std::size_t>> clusters(_k);
    // Init medoids
    std::vector<std::size_t> medoids =
        MyRandom::getInstance().getKShuffled<std::size_t>(_k, 0, _dist.size2() - 1);
    std::vector<double> usedCapas(_dist.size2(), 0.0);
    bool changed = true;
    for (std::size_t t = 0; t < _tMax && changed; ++t) {
        std::fill(usedCapas.begin(), usedCapas.end(), 0.0);
        changed = false;
        for (auto& vect : clusters) {
            vect.clear();
        }
        // For each element, find the closest medoid and assign the element to the
        // cluster of the corresponding medoid
        for (std::size_t i = 0; i < _dist.size1(); ++i) {
            std::size_t minInd = 0;
            for (std::size_t j = 1; j < medoids.size(); ++j) {
                if (_dist(i, medoids[j]) < _dist(i, medoids[minInd]) && // Update closest medoid if
                                                                        // distance is smaller..
                    usedCapas[j] < _capas[j]) {                         // and still as available capa
                    minInd = j;
                }
            }
            clusters[minInd].push_back(i);
            usedCapas[minInd] -= _charge[i];
        }
        // Search medoid for each cluster
        for (std::size_t c = 0; c < clusters.size(); ++c) {
            // Compute distance between all elements of the cluster
            std::size_t minDistInd = 0;
            double minDist =
                std::accumulate(clusters[c].begin(), clusters[c].end(), 0.0,
                    [&](const double __tmpDist, const std::size_t __elj) {
                        return __tmpDist + _dist(clusters[c][0], __elj);
                    });
            for (std::size_t i = 1; i < clusters[c].size(); ++i) {
                double distance = 0.0;
                for (std::size_t j = 0; j < clusters[c].size(); ++j) {
                    distance += _dist(clusters[c][i], clusters[c][j]);
                    if (distance > minDist) {
                        break;
                    }
                }
                if (distance < minDist) {
                    minDistInd = i;
                }
            }
            if (medoids[c] != clusters[c][minDistInd]) { // Medoids of cluster c is changed
                changed = true;
                medoids[c] = clusters[c][minDistInd];
            }
        }
    }
    return medoids;
}

#endif