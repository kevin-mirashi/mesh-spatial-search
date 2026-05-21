#include "msh.h"
#include <iostream>
#include <chrono>
#include <random>
#include <cassert>

using namespace std;

void benchmark(const string& filename, double pas_grille) {
    Mesh m(filename);
    m.buildGrid(pas_grille);
    m.buildGridPhmap(pas_grille);

    int nb_points = 10000;
    random_device rd;
    mt19937 gen(42); // graine fixe pour reproductibilité
    uniform_real_distribution<double> dis(0.0, 1.0);

    vector<array<double, 2>> points_test(nb_points);
    for (int i = 0; i < nb_points; ++i)
        points_test[i] = {dis(gen), dis(gen)};

    // Naive
    vector<int> res_naive(nb_points);
    auto t0 = chrono::high_resolution_clock::now();
    for (int i = 0; i < nb_points; ++i)
        res_naive[i] = m.findTriangleNaive(points_test[i][0], points_test[i][1]);
    double duree_naive = chrono::duration<double, milli>(chrono::high_resolution_clock::now() - t0).count();

    // unordered_map
    vector<int> res_hash(nb_points);
    t0 = chrono::high_resolution_clock::now();
    for (int i = 0; i < nb_points; ++i)
        res_hash[i] = m.findTriangleHash(points_test[i]);
    double duree_hash = chrono::duration<double, milli>(chrono::high_resolution_clock::now() - t0).count();

    // phmap
    vector<int> res_phmap(nb_points);
    t0 = chrono::high_resolution_clock::now();
    for (int i = 0; i < nb_points; ++i)
        res_phmap[i] = m.findTrianglePhmap(points_test[i]);
    double duree_phmap = chrono::duration<double, milli>(chrono::high_resolution_clock::now() - t0).count();

    // Vérification
    for (int i = 0; i < nb_points; ++i) {
        assert(res_naive[i] == res_hash[i]);
        assert(res_naive[i] == res_phmap[i]);
    }

    // Affichage ligne du tableau
    cout << "| " << filename
         << " | " << m.getNbTriangles()
         << " | " << duree_naive
         << " | " << duree_hash
         << " | " << duree_phmap
         << " | x" << (duree_naive / duree_hash)
         << " | x" << (duree_naive / duree_phmap)
         << " |" << endl;
}

int main()
{
    Mesh m("../square.msh");
    m.buildGrid(0.1);
    m.buildGridPhmap(0.1);

    cout << "==================================================" << endl;
    cout << "   TESTS DE RECHERCHE DE POINTS DANS LE MAILLAGE   " << endl;
    cout << "==================================================" << endl;

    // TEST 1
    double px1 = 0.5, py1 = 0.5;
    array<double, 2> p1 = {px1, py1};
    int t1_naive = m.findTriangleNaive(px1, py1);
    int t1_hash  = m.findTriangleHash(p1);
    int t1_phmap = m.findTrianglePhmap(p1);
    cout << "Recherche du point (" << px1 << ", " << py1 << ") :" << endl;
    cout << "  -> Naive   : Triangle n°" << t1_naive << endl;
    cout << "  -> Hashmap : Triangle n°" << t1_hash  << endl;
    cout << "  -> Phmap   : Triangle n°" << t1_phmap << endl;
    if (t1_naive == t1_hash && t1_naive == t1_phmap)
        cout << "  >> LES TROIS ALGORITHMES COINCIDENT ! [OK]" << endl;
    cout << "--------------------------------------------------" << endl;

    // TEST 2
    double px2 = 1.5, py2 = 1.5;
    array<double, 2> p2 = {px2, py2};
    int t2_naive = m.findTriangleNaive(px2, py2);
    int t2_hash  = m.findTriangleHash(p2);
    int t2_phmap = m.findTrianglePhmap(p2);
    cout << "Recherche du point (" << px2 << ", " << py2 << ") :" << endl;
    cout << "  -> Naive   : " << (t2_naive == -1 ? "HORS MAILLAGE [OK]" : "Erreur !") << endl;
    cout << "  -> Hashmap : " << (t2_hash  == -1 ? "HORS MAILLAGE [OK]" : "Erreur !") << endl;
    cout << "  -> Phmap   : " << (t2_phmap == -1 ? "HORS MAILLAGE [OK]" : "Erreur !") << endl;
    cout << "==================================================" << endl;

    // BENCHMARK multi-raf
    cout << "\nBENCHMARK MULTI-RAF (10 000 points) :" << endl;
    cout << "| Fichier | Triangles | Naive (ms) | unordered_map (ms) | phmap (ms) | Accel. hash | Accel. phmap |" << endl;
    cout << "|---------|-----------|------------|-------------------|------------|-------------|--------------|" << endl;
    benchmark("../square_02.msh",  0.2);
    benchmark("../square_01.msh",  0.1);
    benchmark("../square_005.msh", 0.05);
    benchmark("../square_002.msh", 0.02);

    cout << "==================================================" << endl;
    m.findVoisins();
    m.findVoisinsNaive();
    m.writeGnuplot("gnu.plt");
    m.writeGnuplotPoint("gnu_point.plt", 0.5, 0.5);
    m.writeGnuplotPoint("gnu_hors.plt",  1.5, 1.5);

    return 0;
}