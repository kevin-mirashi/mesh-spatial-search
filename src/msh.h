#ifndef MESH_H
#define MESH_H

#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <unordered_map>

using namespace std;

struct GridKey {
    int i, j;
    bool operator==(const GridKey& other) const {
        return i == other.i && j == other.j;
    }
};

struct GridKeyHash {
    size_t operator()(const GridKey& k) const {
        return hash<int>()(k.i) ^ (hash<int>()(k.j) << 1);
    }
};

struct PairHash {
    template <class T1, class T2>
    size_t operator()(const pair<T1, T2>& p) const {
        return hash<T1>()(p.first) ^ (hash<T2>()(p.second) << 1);
    }
};

class Mesh
{
private:
    vector<array<double, 2>> node;
    vector<array<int, 3>> triangle;

    double x_min, x_max, y_min, y_max;
    double h_grid;
    unordered_map<GridKey, vector<int>, GridKeyHash> grid;

    bool isPointInTriangle(array<double, 2> p, int t_idx) const;

public:
    Mesh(string gmsh2_filename);
    friend ostream &operator<<(ostream &os, const Mesh &m);
    void writeGnuplot(const string &filename) const;
    void writeGnuplotPoint(const string& filename, double px, double py) const;

    // Q3.2
    int findTriangleNaive(double x, double y) const;  // ← LIGNE MANQUANTE

    // Q3.3
    void buildGrid(double step);
    int findTriangleHash(array<double, 2> p) const;

    // Q3.5
    void findVoisins() const;
    void findVoisinsNaive() const;
};

#endif