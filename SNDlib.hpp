#ifndef SNDLIB_HPP
#define SNDLIB_HPP

#include "DiGraph.hpp"
#include "Matrix.hpp"
#include "utility.hpp"
#include <fstream>
#include <iostream>
#include <vector>

struct Demand {
    Demand(const Graph::Node& _s, const Graph::Node& _t, const double& _d)
        : s(_s)
        , t(_t)
        , d(_d) {}
    Graph::Node s;
    Graph::Node t;
    double d;
};

bool operator==(const Demand& _d1, const Demand& _d2) {
    return _d1.s == _d2.s && _d1.t == _d2.t && _d1.d == _d2.d;
}

std::ostream& operator<<(std::ostream& _out, const Demand& _demand) {
    return _out << '{' << _demand.s << ", " << _demand.t << ", " << _demand.d << '}';
}

std::vector<Demand> loadDemandsFromFile(const std::string& _filename) {
    std::vector<Demand> demands;
    std::ifstream ifs(_filename, std::ifstream::in);
    if (!ifs) {
        std::cerr << _filename << " does not exists!" << std::endl;
        exit(-1);
    }

    while (ifs.good()) {
        std::string line;
        std::getline(ifs, line);
        if (line != "") {
            int s, t;
            double d;
            std::stringstream lineStream(line);
            lineStream >> s >> t >> d;
            demands.emplace_back(s, t, d);
        }
    }
    return demands;
}

Matrix<int> loadNextHop(const std::string& _filename) {
    std::ifstream ifs(_filename, std::ifstream::in);
    if (!ifs) {
        std::cerr << _filename << " does not exists!" << std::endl;
        exit(-1);
    }
    std::stringstream lineStream;
    std::string line;

    std::getline(ifs, line, '\n');
    lineStream.str(line);

    int order = 0;
    lineStream >> order;
    Matrix<int> nextHops(order, order, -1);

    while (ifs.good()) {
        line = "";
        std::getline(ifs, line);
        if (line != "") {
            int u, t, nh = 0;
            std::stringstream lineStream1(line);
            lineStream1 >> u >> t >> nh;
            nextHops(u, t) = nh;
        }
    }
    return nextHops;
}

std::tuple<double, double, double, double> loadEnergy(const std::string& _filename) {
    std::ifstream ifs(_filename, std::ifstream::in);
    if (!ifs) {
        std::cerr << _filename << " does not exists!" << std::endl;
        exit(-1);
    }
    std::tuple<double, double, double, double> energy;

    std::string line;
    std::getline(ifs, line, '\n');
    std::stringstream lineStream(line);
    lineStream >> std::get<0>(energy) >> std::get<1>(energy) >> std::get<2>(energy) >> std::get<3>(energy);
    return energy;
}

Matrix<int> loadTunnels(const std::string& _filename) {
    std::ifstream ifs(_filename, std::ifstream::in);
    if (!ifs) {
        std::cerr << _filename << " does not exists!" << std::endl;
        exit(-1);
    }
    std::string line;
    std::getline(ifs, line, '\n');
    std::stringstream lineStream(line);

    int order = 0;
    lineStream >> order;
    Matrix<int> tunnels(order, order, -1);

    while (ifs.good()) {
        line = "";
        std::getline(ifs, line, '\n');
        if (line != "") {
            int u, t, nh = 0;
            std::stringstream lineStream1(line);
            lineStream1 >> u >> t >> nh;
            assert(u < order);
            assert(t < order);
            assert(u >= 0);
            assert(t >= 0);
            tunnels(u, t) = nh;
        }
    }
    return tunnels;
}

std::vector<int> loadSDNs(const std::string& _filename, const int& nbSDNs, int _type) {
    std::ifstream ifs(_filename, std::ifstream::in);
    if (!ifs) {
        std::cerr << _filename << " does not exists!" << std::endl;
        exit(-1);
    }
    std::string line;
    std::getline(ifs, line, '\n');
    std::stringstream lineStream(line);

    int order, u = 0;
    lineStream >> order;
    std::vector<int> SDNs(order, false);
    int i = 0;
    while (i < nbSDNs) {
        line = "";
        std::getline(ifs, line);
        if (line != "") {
            std::stringstream lineStream1(line);
            for (int i = 0; i < _type; ++i) {
                lineStream1 >> u;
            }
            lineStream1 >> u;
            SDNs[u] = true;
        }
        ++i;
    }
    return SDNs;
}

std::vector<int> loadSDNsMaxCover(const std::string& _filename, const int& _nbSDNs) {
    std::ifstream ifs(_filename, std::ifstream::in);
    if (!ifs) {
        std::cerr << _filename << " does not exists!" << std::endl;
        exit(-1);
    }
    std::string line;
    std::getline(ifs, line, '\n');
    std::stringstream lineStream(line);

    int order, u = 0;
    lineStream >> order;

    std::vector<int> SDNs(order, false);

    for (int i = 0; i < _nbSDNs; ++i) {
        std::getline(ifs, line);
        std::cout << line << '\n';
    }

    std::getline(ifs, line);
    std::stringstream lineStream1(line);
    for (int i = 0; i < _nbSDNs; ++i) {
        lineStream1 >> u;
        SDNs[u] = true;
    }

    return SDNs;
}

#endif