#ifndef MESH_H
#define MESH_H
#include <iostream>
#include <fstream>
#include <vector>
#include <array>

using namespace std;

// classe maillage 2d avec liste de noeuds et liste de triangles
class Mesh
{
private:
    vector<array<double, 2>> node;
    vector<array<int, 3>> triangle;

public:
    Mesh(string gmsh2_filename);
    friend ostream &operator<<(ostream &os, const Mesh &m);
    // affichage dans un fichier lisible par gnuplot
    void writeGnuplot(const string &filename) const;
    // Question 3.2 : Recherche naive d'un triangle contenant le point (x, y)
    int findTriangleNaive(double x, double y) const;
};

#endif
