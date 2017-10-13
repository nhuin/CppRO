#ifndef KMEDOIDS_HPP
#define KMEDOIDS_HPP

#include <vector>

#include "Matrix.hpp"
#include "MyRandom.hpp"
#include "utility.hpp"

std::vector<int> getKMedoids(const int _k, const Matrix<double> &_dist,
                             const int _tMax = 100) {
  assert(_dist.size1() == _dist.size2());
  std::vector<std::vector<int>> clusters(_k);
  // Init medoids
  std::vector<int> medoids =
      MyRandom::getInstance().getKShuffled(_k, 0, _dist.size2() - 1);
  bool changed = true;
  for (int t = 0; t < _tMax && changed; ++t) {
    changed = false;
    for (auto &vect : clusters) {
      vect.clear();
    }
    // For each elements, find the closest medoid and assign the element to the
    // cluster of the corresponding medoid
    for (int i = 0; i < _dist.size1(); ++i) {
      int minInd = 0;
      for (int j = 1; j < static_cast<int>(medoids.size()); ++j) {
        if (_dist(i, medoids[j]) < _dist(i, medoids[minInd])) {
          minInd = j;
        }
      }
      clusters[minInd].push_back(i);
    }
    // Search medoid for each cluster
    for (int c = 0; c < static_cast<int>(clusters.size()); ++c) {
      // Compute distance between all elements of the cluster
      int minDistInd = 0;
      double minDist =
          std::accumulate(clusters[c].begin(), clusters[c].end(), 0.0,
                          [&](const double __tmpDist, const int __elj) {
                            return __tmpDist + _dist(clusters[c][0], __elj);
                          });
      for (int i = 1; i < static_cast<int>(clusters[c].size()); ++i) {
        double distance = 0.0;
        for (int j = 0; j < static_cast<int>(clusters[c].size()); ++j) {
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

std::vector<int> getKMedoids(const int _k, const Matrix<double> &_dist,
                             const std::vector<int> &_capas,
                             const std::vector<double> &_charge,
                             const int _tMax = 100) {
  assert(static_cast<int>(_capas.size()) == _dist.size1());
  assert(static_cast<int>(_charge.size()) == _dist.size1());
  assert(_dist.size1() == _dist.size2());

  std::vector<std::vector<int>> clusters(_k);
  // Init medoids
  std::vector<int> medoids =
      MyRandom::getInstance().getKShuffled(_k, 0, _dist.size2() - 1);
  std::vector<double> usedCapas(_dist.size2(), 0.0);
  bool changed = true;
  for (int t = 0; t < _tMax && changed; ++t) {
    std::fill(usedCapas.begin(), usedCapas.end(), 0.0);
    changed = false;
    for (auto &vect : clusters) {
      vect.clear();
    }
    // For each element, find the closest medoid and assign the element to the
    // cluster of the corresponding medoid
    for (int i = 0; i < _dist.size1(); ++i) {
      int minInd = 0;
      for (int j = 1; j < static_cast<int>(medoids.size()); ++j) {
        if (_dist(i, medoids[j]) <
                _dist(i, medoids[minInd]) && // Update closest medoid if
                                             // distance is smaller..
            usedCapas[j] < _capas[j]) {      // and still as available capa
          minInd = j;
        }
      }
      clusters[minInd].push_back(i);
      usedCapas[minInd] -= _charge[i];
    }
    // Search medoid for each cluster
    for (int c = 0; c < static_cast<int>(clusters.size()); ++c) {
      // Compute distance between all elements of the cluster
      int minDistInd = 0;
      double minDist =
          std::accumulate(clusters[c].begin(), clusters[c].end(), 0.0,
                          [&](const double __tmpDist, const int __elj) {
                            return __tmpDist + _dist(clusters[c][0], __elj);
                          });
      for (int i = 1; i < static_cast<int>(clusters[c].size()); ++i) {
        double distance = 0.0;
        for (int j = 0; j < static_cast<int>(clusters[c].size()); ++j) {
          distance += _dist(clusters[c][i], clusters[c][j]);
          if (distance > minDist) {
            break;
          }
        }
        if (distance < minDist) {
          minDistInd = i;
        }
      }
      if (medoids[c] !=
          clusters[c][minDistInd]) { // Medoids of cluster c is changed
        changed = true;
        medoids[c] = clusters[c][minDistInd];
      }
    }
  }
  return medoids;
}

#endif