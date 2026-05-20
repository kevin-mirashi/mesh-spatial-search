// test_pred.cpp
// Compilation : g++ test_pred.cpp predicates.c -O2 -o test_pred

#include <iostream>
#include "predicates.hpp"

// --- Point dans un triangle ---
bool pointDansTriangle(REAL *p, REAL *a, REAL *b, REAL *c) {
    REAL o1 = orient2d(a, b, p);
    REAL o2 = orient2d(b, c, p);
    REAL o3 = orient2d(c, a, p);
    bool has_neg = (o1 < 0) || (o2 < 0) || (o3 < 0);
    bool has_pos = (o1 > 0) || (o2 > 0) || (o3 > 0);
    return !(has_neg && has_pos);
}

// --- Point dans cercle circonscrit ---
bool pointDansCercle(REAL *p, REAL *a, REAL *b, REAL *c) {
    return incircle(a, b, c, p) > 0;
}

int main() {
    exactinit();

    // TEST 1 : Triangle (0,0), (2,0), (0,2)
    REAL triangle[3][2] = {{0.0, 0.0}, {2.0, 0.0}, {0.0, 2.0}};
    REAL p_in[2] = {0.5, 0.5};
    REAL p_out[2] = {3.0, 3.0};

    std::cout << "=== TEST 1 : Point dans triangle ===" << std::endl;
    std::cout << "Point (0.5, 0.5) dans triangle? "
              << (pointDansTriangle(p_in, triangle[0], triangle[1], triangle[2]) ? "OUI" : "NON") << std::endl;
    std::cout << "Point (3.0, 3.0) dans triangle? "
              << (pointDansTriangle(p_out, triangle[0], triangle[1], triangle[2]) ? "OUI" : "NON") << std::endl << std::endl;

    // TEST 2 : Cercle circonscrit de (0,0), (1,0), (0,1)
    REAL cercle[3][2] = {{0.0, 0.0}, {1.0, 0.0}, {0.0, 1.0}};
    REAL p_c_in[2] = {0.25, 0.25};
    REAL p_c_out[2] = {2.0, 2.0};

    std::cout << "=== TEST 2 : Point dans cercle circonscrit ===" << std::endl;
    std::cout << "Point (0.25, 0.25) dans cercle? "
              << (pointDansCercle(p_c_in, cercle[0], cercle[1], cercle[2]) ? "OUI" : "NON") << std::endl;
    std::cout << "Point (2.0, 2.0) dans cercle? "
              << (pointDansCercle(p_c_out, cercle[0], cercle[1], cercle[2]) ? "OUI" : "NON") << std::endl;

    return 0;
}
