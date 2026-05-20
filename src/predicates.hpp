// predicates.hpp
// Header pour la bibliothèque predicates.c (Jonathan Shewchuk)
// Domaine public - https://www.cs.cmu.edu/~quake/robust.html

#ifndef PREDICATES_HPP
#define PREDICATES_HPP

#ifdef __cplusplus
extern "C" {
#endif

// Type flottant (double par défaut, comme dans predicates.c)
#ifndef REAL
#define REAL double
#endif

// ---- Initialisation ----
void exactinit();

// ---- Prédicats géométriques robustes ----
REAL orient2d(REAL *pa, REAL *pb, REAL *pc);
REAL orient3d(REAL *pa, REAL *pb, REAL *pc, REAL *pd);
REAL incircle(REAL *pa, REAL *pb, REAL *pc, REAL *pd);
REAL insphere(REAL *pa, REAL *pb, REAL *pc, REAL *pd, REAL *pe);

// ---- Versions "fast" (non robustes) ----
REAL orient2dfast(REAL *pa, REAL *pb, REAL *pc);
REAL orient3dfast(REAL *pa, REAL *pb, REAL *pc, REAL *pd);
REAL incirclefast(REAL *pa, REAL *pb, REAL *pc, REAL *pd);

#ifdef __cplusplus
}
#endif

#endif // PREDICATES_HPP
