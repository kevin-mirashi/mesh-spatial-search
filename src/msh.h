#ifndef MESH_H
#define MESH_H

#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <unordered_map> // <-- Ajout pour la hashmap

using namespace std;

// Structure pour servir de clé (i, j) dans notre grille
struct GridKey {
    int i, j;
    bool operator==(const GridKey& other) const {
        return i == other.i && j == other.j;
    }
};

// Foncteur de hachage personnalisé pour GridKey
struct GridKeyHash {
    size_t operator()(const GridKey& k) const {
        // Un hachage simple combinant les coordonnées i et j
        return hash<int>()(k.i) ^ (hash<int>()(k.j) << 1);
    }
};

class Mesh
{
private:
    vector<array<double, 2>> node; 
    vector<array<int, 3>> triangle; 

    // --- Nouveaux attributs pour la grille (Q3.3) ---
    double x_min, x_max, y_min, y_max; // Limites du maillage
    double h_grid;                    // Pas du quadrillage
    unordered_map<GridKey, vector<int>, GridKeyHash> grid; // Hashmap: Case -> Liste d'IDs de triangles

    bool isPointInTriangle(array<double, 2> p, int t_idx) const;

public:
    Mesh(string gmsh2_filename); 
    friend ostream &operator<<(ostream &os, const Mesh &m); 
    void writeGnuplot(const string &filename) const;

    // --- Nouvelles fonctions à implémenter ---
    void buildGrid(double step); // Construit la structure de grille
    int findTriangleHash(array<double, 2> p) const; // Recherche accélérée
};

#endif