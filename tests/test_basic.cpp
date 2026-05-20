#include "../src/msh.h"
#include <cassert>
#include <iostream>

int main()
{
    Mesh m("../square.msh");

    // Test 1 : point dans le maillage
    int t1 = m.findTriangleNaive(0.5, 0.5);
    assert(t1 != -1);
    std::cout << "Test 1 OK : point (0.5, 0.5) trouve dans triangle " << t1 << std::endl;

    // Test 2 : point hors maillage
    int t2 = m.findTriangleNaive(1.5, 1.5);
    assert(t2 == -1);
    std::cout << "Test 2 OK : point (1.5, 1.5) hors maillage" << std::endl;

    std::cout << "Tous les tests sont passes !" << std::endl;
    return 0;
}
