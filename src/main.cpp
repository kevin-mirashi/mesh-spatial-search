#include "msh.h"
#include <iostream>
#include <chrono>
#include <random>
#include <cassert>

using namespace std;

int main()
{
    // 1. Chargement du maillage
    Mesh m("../square.msh");
    
    // 2. Construction des deux grilles
    double pas_grille = 0.1;
    m.buildGrid(pas_grille);
    m.buildGridPhmap(pas_grille);

    cout << "==================================================" << endl;
    cout << "   TESTS DE RECHERCHE DE POINTS DANS LE MAILLAGE   " << endl;
    cout << "==================================================" << endl;

    // --- TEST 1 : Un point au milieu du domaine ---
    double px1 = 0.5, py1 = 0.5;
    array<double, 2> p1 = {px1, py1};
    
    int t1_naive = m.findTriangleNaive(px1, py1);
    int t1_hash  = m.findTriangleHash(p1);
    int t1_phmap = m.findTrianglePhmap(p1);
    
    cout << "Recherche du point (" << px1 << ", " << py1 << ") :" << endl;
    if (t1_naive != -1) {
        cout << "  -> Naive   : Trouve dans le triangle n°" << t1_naive << endl;
        cout << "  -> Hashmap : Trouve dans le triangle n°" << t1_hash << endl;
        cout << "  -> Phmap   : Trouve dans le triangle n°" << t1_phmap << endl;
    } else {
        cout << "  -> Non trouve (HORS MAILLAGE)" << endl;
    }

    if (t1_naive == t1_hash && t1_naive == t1_phmap) {
        cout << "  >> Verification : LES TROIS ALGORITHMES COINCIDENT ! [OK]" << endl;
    } else {
        cout << "  >> Verification : ATTENTION, DIFFERENCE DETECTEE ! [ERREUR]" << endl;
    }
    cout << "--------------------------------------------------" << endl;

    // --- TEST 2 : Un point en dehors du maillage ---
    double px2 = 1.5, py2 = 1.5;
    array<double, 2> p2 = {px2, py2};
    
    int t2_naive = m.findTriangleNaive(px2, py2);
    int t2_hash  = m.findTriangleHash(p2);
    int t2_phmap = m.findTrianglePhmap(p2);
    
    cout << "Recherche du point (" << px2 << ", " << py2 << ") :" << endl;
    cout << "  -> Naive   : " << (t2_naive == -1 ? "Non trouve (HORS MAILLAGE) -> Correct !" : "Trouve (Erreur !)") << endl;
    cout << "  -> Hashmap : " << (t2_hash  == -1 ? "Non trouve (HORS MAILLAGE) -> Correct !" : "Trouve (Erreur !)") << endl;
    cout << "  -> Phmap   : " << (t2_phmap == -1 ? "Non trouve (HORS MAILLAGE) -> Correct !" : "Trouve (Erreur !)") << endl;
    cout << "==================================================" << endl;

    // --- TEST 3 : Benchmark sur 10 000 points aléatoires ---
    cout << "Lancement des mesures de performance (10 000 points)..." << endl;
    
    int nb_points = 10000;
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<double> dis(0.0, 1.0);

    vector<array<double, 2>> points_test(nb_points);
    for (int i = 0; i < nb_points; ++i)
        points_test[i] = {dis(gen), dis(gen)};

    // A) Naive
    vector<int> resultats_naifs(nb_points);
    auto start_naive = chrono::high_resolution_clock::now();
    for (int i = 0; i < nb_points; ++i)
        resultats_naifs[i] = m.findTriangleNaive(points_test[i][0], points_test[i][1]);
    auto end_naive = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duree_naive = end_naive - start_naive;

    // B) unordered_map
    vector<int> resultats_hash(nb_points);
    auto start_hash = chrono::high_resolution_clock::now();
    for (int i = 0; i < nb_points; ++i)
        resultats_hash[i] = m.findTriangleHash(points_test[i]);
    auto end_hash = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duree_hash = end_hash - start_hash;

    // C) phmap
    vector<int> resultats_phmap(nb_points);
    auto start_phmap = chrono::high_resolution_clock::now();
    for (int i = 0; i < nb_points; ++i)
        resultats_phmap[i] = m.findTrianglePhmap(points_test[i]);
    auto end_phmap = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duree_phmap = end_phmap - start_phmap;

    // D) Affichage
    cout << "--------------------------------------------------" << endl;
    cout << "BENCHMARK : " << nb_points << " POINTS" << endl;
    cout << "  -> Temps Naive         : " << duree_naive.count() << " ms" << endl;
    cout << "  -> Temps unordered_map : " << duree_hash.count()  << " ms" << endl;
    cout << "  -> Temps phmap         : " << duree_phmap.count() << " ms" << endl;
    cout << "  >> Acceleration unordered_map : x" << (duree_naive.count() / duree_hash.count())  << endl;
    cout << "  >> Acceleration phmap         : x" << (duree_naive.count() / duree_phmap.count()) << endl;
    cout << "--------------------------------------------------" << endl;

    // E) Vérification cohérence
    for (int i = 0; i < nb_points; ++i) {
        assert(resultats_naifs[i] == resultats_hash[i]);
        assert(resultats_naifs[i] == resultats_phmap[i]);
    }
    cout << ">> Verification : Les 3 methodes coincident ! [OK]" << endl;
    cout << "==================================================" << endl;

    m.findVoisins();
    m.findVoisinsNaive();
    m.writeGnuplot("gnu.plt");
    m.writeGnuplotPoint("gnu_point.plt", 0.5, 0.5);
    m.writeGnuplotPoint("gnu_hors.plt",  1.5, 1.5);
    
    return 0;
}