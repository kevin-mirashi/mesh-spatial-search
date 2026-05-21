# Projet C++ L3 Math : Manipulation de maillages 2D

## 1. Présentation du projet

Ce projet traite de la manipulation de maillages 2D en C++. L'objectif principal est de comparer différentes approches algorithmiques pour rechercher efficacement un triangle contenant un point donné, ainsi que pour retrouver les triangles voisins d'un triangle. Les maillages sont générés avec le logiciel Gmsh.

## 2. Structure du projet

Le dépôt est organisé de la façon suivante :

- `src/` : fichiers sources C++ (`main.cpp`, `msh.cpp`, `msh.h`, `predicates.c`, `predicates.hpp`)
- `doc/` : rapport du projet
- `tests/` : programmes de tests (`test_basic.cpp`, `test_pred.cpp`)
- `libs/` : bibliothèque externe `parallel-hashmap` (header-only)
- `CMakeLists.txt` : gestion de la compilation
- `square.geo` : fichier de géométrie Gmsh
- `square.msh`, `square_02.msh`, `square_01.msh`, `square_005.msh`, `square_002.msh` : maillages générés

Pour compiler et lancer le projet :

```bash
mkdir build
cd build
cmake ..
cmake --build .
./Debug/go.exe
```

Pour visualiser avec Gnuplot :

```bash
gnuplot gnu_point.plt
```

## 3. Réponses aux Questions

### 3.1 Structure de données

#### A) Structure du fichier square.msh

Le fichier `.msh` est généré par Gmsh au format version 2. Il est découpé en plusieurs sections textuelles.

La section `$Nodes` commence par indiquer le nombre total de nœuds. Chaque ligne suivante donne l'identifiant du nœud et ses coordonnées (x, y, z).

La section `$Elements` indique le nombre total d'éléments. Chaque élément précise son type — le type 2 correspond aux triangles à 3 sommets — ainsi que les identifiants des nœuds qui le composent.

#### B) Structure de données Mesh en C++

La classe `Mesh` stocke le maillage en mémoire avec deux tableaux dynamiques. Le vecteur `node` contient les coordonnées (x, y) de chaque nœud sous la forme `vector<array<double, 2>>`. Le vecteur `triangle` contient pour chaque triangle les indices de ses 3 sommets sous la forme `vector<array<int, 3>>`.

Un point important : dans le fichier `.msh`, les nœuds sont numérotés à partir de 1, mais les tableaux C++ commencent à 0. Le constructeur décrémente donc chaque indice lors du chargement (`t[0]--`, etc.).

#### C) Fonctionnement du programme de test

Le `main.cpp` charge le maillage depuis `../square.msh`, effectue les benchmarks, puis appelle `writeGnuplot` et `writeGnuplotPoint` pour générer des fichiers visualisables avec Gnuplot.

### 3.2 Recherche simple

La recherche naïve parcourt tous les triangles un par un, ce qui donne une complexité en O(N).

Pour savoir si un point P(x,y) est dans un triangle T formé des sommets P0, P1, P2, on utilise les prédicats robustes de Shewchuk (voir section 3.2.B) basés sur `orient2d`. Le signe du résultat indique de quel côté de l'arête orientée se trouve le point. Si les trois signes sont identiques, le point est à l'intérieur du triangle. Si aucun triangle ne valide ce test, la fonction retourne -1.

#### B) Prédicats robustes de Shewchuk

La fonction `isPointInTriangle` utilise la bibliothèque `predicates.c` de Jonathan Shewchuk (domaine public). La fonction `orient2d(a, b, p)` calcule le signe du déterminant :
| ax-px  ay-py |
| bx-px  by-py |
de façon exacte, sans erreurs d'arrondi en virgule flottante. Cela évite les cas ambigus quand un point est exactement sur une arête. La fonction `exactinit()` est appelée dans le constructeur de `Mesh` pour initialiser les constantes de précision.

### 3.3 Recherche avec hashmap (Grille)

#### A) Principe

L'idée est de diviser le domaine en cases carrées de côté h. Chaque case est identifiée par une clé `GridKey(i, j)`. À la construction avec `buildGrid`, on parcourt tous les triangles et on les affecte aux cases qu'ils touchent, en se basant sur leur boîte englobante. Ces associations sont stockées dans une `std::unordered_map`. Lors d'une recherche, on calcule directement en O(1) quelle case contient le point, puis on teste uniquement les quelques triangles de cette case.

#### B) Résultats expérimentaux

Benchmark sur 10 000 points aléatoires dans [0,1] × [0,1] pour différentes finesses de maillage :

| Maillage | Triangles | Naive (ms) | unordered_map (ms) | phmap (ms) | Accél. hash | Accél. phmap |
|----------|-----------|------------|-------------------|------------|-------------|--------------|
| raf=0.2  | 66        | 15.2       | 4.9               | 5.7        | x3.1        | x2.7         |
| raf=0.1  | 242       | 54.6       | 5.0               | 6.4        | x10.8       | x8.5         |
| raf=0.05 | 944       | 216.3      | 5.8               | 6.1        | x37.6       | x35.7        |
| raf=0.02 | 5824      | 1431.2     | 6.7               | 7.3        | x213.7      | x196.8       |

Ces résultats confirment que la méthode naïve est bien en O(N) : son temps est multiplié par ~90 quand le nombre de triangles est multiplié par ~90. Les deux méthodes hashmap restent stables autour de 5-7 ms quelle que soit la taille du maillage, ce qui confirme le comportement O(1) par recherche. Les trois méthodes donnent exactement les mêmes résultats, vérifiés par assertions.

### 3.4 Utilisation de bibliothèques alternatives

#### A) Limites de std::unordered_map

La hashmap standard fonctionne par chaînage : en cas de collision, elle crée un nouveau nœud dynamique avec un pointeur. Cela fragmente la mémoire et provoque des cache misses fréquents, ce qui peut ralentir les accès sur de gros maillages.

#### B) La bibliothèque parallel-hashmap

La bibliothèque `parallel-hashmap` est disponible sur GitHub, développée par Gregory Popovitch et inspirée des Swiss Tables de Google. Elle est intégrée dans le projet dans le dossier `libs/` (header-only, aucune compilation nécessaire).

Son principal avantage est l'adressage ouvert : les données sont stockées dans des tableaux contigus en mémoire, ce qui réduit fortement les cache misses. Sur les maillages testés, les performances sont comparables à `std::unordered_map`. L'avantage de `phmap` devient plus visible sur des maillages très fins avec des millions de triangles, où l'on peut espérer un gain de l'ordre de 1.5x à 2x.

### 3.5 Recherche des triangles voisins

#### A) Définition et méthode naïve

Deux triangles sont voisins s'ils partagent exactement 2 nœuds, donc une arête commune. L'approche naïve compare chaque triangle avec tous les autres : pour chaque paire (i, j), on compte les nœuds en commun. La complexité est en O(N²).

#### B) Approche optimisée par hashmap

Pour obtenir une complexité O(N), on utilise une hashmap dont la clé est une arête représentée par la paire triée `(min(A,B), max(A,B))`. L'algorithme parcourt tous les triangles, insère leurs 3 arêtes dans la hashmap, puis identifie les arêtes partagées par 2 triangles comme arêtes internes.

#### C) Résultats expérimentaux

Sur le maillage `square.msh` avec raf=0.1 (242 triangles) :

- Total des arêtes uniques : 543
- Arêtes sur la frontière externe : 48
- Arêtes internes partagées : 495

La méthode naïve O(N²) donne le même résultat de 495 arêtes internes, ce qui confirme que l'algorithme hashmap est correct.

### 3.6 Visualisations

La fonction `writeGnuplotPoint(filename, px, py)` génère un script Gnuplot qui affiche :

- le maillage complet en noir
- le triangle contenant le point en rouge
- le point recherché en bleu

Deux visualisations sont générées automatiquement par `main.cpp` :

- `gnu_point.plt` : point (0.5, 0.5) à l'intérieur du domaine
- `gnu_hors.plt` : point (1.5, 1.5) hors du domaine (aucun triangle en rouge)

Pour visualiser :

```bash
gnuplot gnu_point.plt
```

## 4. Annexe : Utilisation de l'IA

### Prompts utilisés

Durant ce projet, j'ai utilisé Claude (Anthropic) comme assistant. Voici les principaux types de demandes effectuées :

1. "Est-ce que ce projet est complet par rapport au sujet ?" — pour identifier les parties manquantes
2. "Corrigeons pas à pas toutes ces choses" — pour guider la correction étape par étape
3. Demandes d'implémentation : "écris la fonction writeGnuplotPoint", "écris buildGridPhmap", etc.
4. Demandes de débogage : correction des erreurs de compilation Windows (`sys/time.h`, `random`)
5. "Voici mon main, mets-le à jour pour que j'aie juste à copier-coller"

### Exploitation critique des résultats

L'IA a été utilisée comme aide à la rédaction et à l'implémentation, mais chaque étape a été vérifiée personnellement :

- Chaque fichier modifié a été relu avant d'être copié
- Chaque compilation a été lancée manuellement et les erreurs analysées
- Les résultats numériques du benchmark ont été obtenus en lançant le programme moi-même
- Les visualisations Gnuplot ont été vérifiées visuellement
- Les corrections Windows (`sys/time.h`, `random`) ont nécessité une compréhension du problème avant d'être appliquées

L'IA n'a pas remplacé la compréhension : les algorithmes (produit vectoriel 2D, hashmap de grille, prédicats de Shewchuk) ont été expliqués et compris avant d'être intégrés.