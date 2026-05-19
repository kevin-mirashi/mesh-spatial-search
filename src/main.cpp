#include "msh.h"
#include <iostream>
#include <chrono>   // Pour mesurer le temps précis
#include <random>   // Pour générer des points aléatoires
#include <cassert>  // Pour automatiser les tests de sécurité

using namespace std;

int main()
{
    // 1. On charge le maillage d'origine (Q3.1)
    Mesh m("../square.msh");
    
    // 2. On construit la grille spatiale pour la hashmap (Q3.3)
    // Le pas de 0.1 découpe bien notre carré de taille 1x1
    double pas_grille = 0.1;
    m.buildGrid(pas_grille);

    cout << "==================================================" << endl;
    cout << "   TESTS DE RECHERCHE DE POINTS DANS LE MAILLAGE   " << endl;
    cout << "==================================================" << endl;

    // --- TEST 1 : Un point au milieu du domaine ---
    double px1 = 0.5, py1 = 0.5;
    array<double, 2> p1 = {px1, py1};
    
    int t1_naive = m.findTriangleNaive(px1, py1);
    int t1_hash  = m.findTriangleHash(p1);
    
    cout << "Recherche du point (" << px1 << ", " << py1 << ") :" << endl;
    if (t1_naive != -1) {
        cout << "  -> Naive   : Trouve dans le triangle n°" << t1_naive << endl;
        cout << "  -> Hashmap : Trouve dans le triangle n°" << t1_hash << endl;
    } else {
        cout << "  -> Non trouve (HORS MAILLAGE)" << endl;
    }
    
    // Petite vérification de sécurité demandée par le sujet
    if (t1_naive == t1_hash) {
        cout << "  >> Verification : LES DEUX ALGORITHMES COINCIDENT ! [OK]" << endl;
    } else {
        cout << "  >> Verification : ATTENTION, DIFFERENCE DETECTEE ! [ERREUR]" << endl;
    }
    cout << "--------------------------------------------------" << endl;


    // --- TEST 2 : Un point en dehors du maillage ---
    double px2 = 1.5, py2 = 1.5;
    array<double, 2> p2 = {px2, py2};
    
    int t2_naive = m.findTriangleNaive(px2, py2);
    int t2_hash  = m.findTriangleHash(p2);
    
    cout << "Recherche du point (" << px2 << ", " << py2 << ") :" << endl;
    cout << "  -> Naive   : " << (t2_naive == -1 ? "Non trouve (HORS MAILLAGE) -> Correct !" : "Trouve (Erreur !)") << endl;
    cout << "  -> Hashmap : " << (t2_hash == -1 ? "Non trouve (HORS MAILLAGE) -> Correct !" : "Trouve (Erreur !)") << endl;
    cout << "==================================================" << endl;


    // --- TEST 3 : Mesures de performance sur 10 000 points aléatoires (Q3.3) ---
    cout << "Lancement des mesures de performance (10 000 points)..." << endl;
    
    int nb_points = 10000;
    // Configuration du générateur aléatoire pour rester dans le domaine [0, 1]x[0, 1]
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<double> dis(0.0, 1.0);

    vector<array<double, 2>> points_test(nb_points);
    for (int i = 0; i < nb_points; ++i) {
        points_test[i] = {dis(gen), dis(gen)};
    }

    // A) Chronométrage de la méthode Naïve
    vector<int> resultats_naifs(nb_points);
    auto start_naive = chrono::high_resolution_clock::now();
    for (int i = 0; i < nb_points; ++i) {
        resultats_naifs[i] = m.findTriangleNaive(points_test[i][0], points_test[i][1]);
    }
    auto end_naive = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duree_naive = end_naive - start_naive;

    // B) Chronométrage de la méthode Hashmap
    vector<int> resultats_hash(nb_points);
    auto start_hash = chrono::high_resolution_clock::now();
    for (int i = 0; i < nb_points; ++i) {
        resultats_hash[i] = m.findTriangleHash(points_test[i]);
    }
    auto end_hash = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duree_hash = end_hash - start_hash;

    // C) Affichage des résultats temporels
    cout << "--------------------------------------------------" << endl;
    cout << "BENCHMARK DE RECHERCHE : " << nb_points << " POINTS" << endl;
    cout << "  -> Temps total Naive   : " << duree_naive.count() << " ms" << endl;
    cout << "  -> Temps total Hashmap : " << duree_hash.count() << " ms" << endl;
    if (duree_hash.count() > 0) {
        cout << "  >> Facteur d'acceleration : x" << (duree_naive.count() / duree_hash.count()) << endl;
    }
    cout << "--------------------------------------------------" << endl;

    // D) Vérification automatique de la cohérence globale
    for (int i = 0; i < nb_points; ++i) {
        assert(resultats_naifs[i] == resultats_hash[i]);
    }
    cout << ">> Verification globale : Tous les resultats coïncident sur l'echantillon ! [OK]" << endl;
    cout << "==================================================" << endl;
    m.findVoisins();
    // 4. Generation du fichier pour Gnuplot (Q3.1 / Q3.6)
    m.writeGnuplot("gnu.plt");
    
    return 0;
}