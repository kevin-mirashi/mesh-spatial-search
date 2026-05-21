#include "../src/msh.h"
#include <cassert>
#include <iostream>

int main()
{
    Mesh m("../square.msh");
    m.buildGrid(0.1);
    m.buildGridPhmap(0.1);

    // ---- Q3.2 : findTriangleNaive ----
    int t1 = m.findTriangleNaive(0.5, 0.5);
    assert(t1 != -1);
    std::cout << "Test 1 OK : point (0.5, 0.5) trouve dans triangle " << t1 << " (naive)" << std::endl;

    int t2 = m.findTriangleNaive(1.5, 1.5);
    assert(t2 == -1);
    std::cout << "Test 2 OK : point (1.5, 1.5) hors maillage (naive)" << std::endl;

    // ---- Q3.3 : findTriangleHash ----
    array<double, 2> p1 = {0.5, 0.5};
    int t3 = m.findTriangleHash(p1);
    assert(t3 != -1);
    std::cout << "Test 3 OK : point (0.5, 0.5) trouve dans triangle " << t3 << " (hash)" << std::endl;

    array<double, 2> p2 = {1.5, 1.5};
    int t4 = m.findTriangleHash(p2);
    assert(t4 == -1);
    std::cout << "Test 4 OK : point (1.5, 1.5) hors maillage (hash)" << std::endl;

    // ---- Q3.4 : findTrianglePhmap ----
    int t5 = m.findTrianglePhmap(p1);
    assert(t5 != -1);
    std::cout << "Test 5 OK : point (0.5, 0.5) trouve dans triangle " << t5 << " (phmap)" << std::endl;

    int t6 = m.findTrianglePhmap(p2);
    assert(t6 == -1);
    std::cout << "Test 6 OK : point (1.5, 1.5) hors maillage (phmap)" << std::endl;

    // ---- Cohérence entre les 3 méthodes ----
    assert(t1 == t3 && t1 == t5);
    std::cout << "Test 7 OK : les 3 methodes donnent le meme resultat" << std::endl;

    // ---- Point aux coins du domaine ----
    int t7 = m.findTriangleNaive(0.0, 0.0);
    std::cout << "Test 8 OK : point (0.0, 0.0) -> triangle " << t7 << " (coin)" << std::endl;

    int t8 = m.findTriangleNaive(1.0, 1.0);
    std::cout << "Test 9 OK : point (1.0, 1.0) -> triangle " << t8 << " (coin)" << std::endl;

    std::cout << "\nTous les tests sont passes !" << std::endl;
    return 0;
}