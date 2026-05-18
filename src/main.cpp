#include "msh.h"
#include <iostream>

using namespace std;

int main()
{
    // 1. On charge le maillage d'origine (Q3.1)
    Mesh m("../square.msh");
    
    // 2. On construit la grille spatiale pour la hashmap (Q3.3)
    // Le pas de 0.1 découpe bien notre carré de taille 1x1
    m.buildGrid(0.1);

    cout << "==================================================" << endl;
    cout << "  TESTS DE RECHERCHE DE POINTS DANS LE MAILLAGE   " << endl;
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

    // 3. Generation du fichier pour Gnuplot (Q3.1 / Q3.6)
    m.writeGnuplot("gnu.plt");
    
    return 0;
}