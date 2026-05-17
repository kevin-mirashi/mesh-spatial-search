# projetcpp

# Projet C++ L3 Math : Manipulation de maillages 2D

## 1. Présentation du projet
Ce projet consiste à manipuler des maillages bidimensionnels et à comparer différents algorithmes de recherche de points ainsi que de recherche de triangles voisins.

## 2. Structure du projet
L'arborescence du dépôt respecte la hiérarchie suivante :
* `src/` : Fichiers sources C++ (`main.cpp`, `msh.cpp`, `msh.h`).
* `CMakeLists.txt` : Configuration de la compilation.
* `README.md` : Rapport textuel des travaux.

---

## 3. Réponses aux Questions

### 3.1 Structure de données
#### A) Structure du fichier `square.msh`
Le fichier de maillage est généré au format Gmsh (version 2) et possède une structure textuelle découpée en sections spécifiques :
* **Section `$Nodes`** : Elle commence par indiquer le nombre total $n$ de nœuds du maillage. Les lignes suivantes contiennent l'identifiant unique du nœud, suivi de ses coordonnées cartésiennes $(x, y, z)$.
* **Section `$Elements`** : Elle indique le nombre total d'éléments (lignes de bordure, triangles, etc.). Chaque ligne d'élément spécifie son identifiant, son type (le `type == 2` correspond aux triangles à 3 nœuds), le nombre de balises (tags), et enfin les identifiants des sommets interconnectés qui forment l'élément.

#### B) Structure de données `Mesh` en C++
En mémoire, la classe `Mesh` utilise des structures de la bibliothèque standard C++ (`std::vector` et `std::array`) encapsulées de manière privée :
* `vector<array<double, 2>> node;` : Un vecteur dynamique stockant pour chaque nœud ses coordonnées bidimensionnelles $[x, y]$.
* `vector<array<int, 3>> triangle;` : Un vecteur dynamique contenant des triplets d'entiers. Chaque triplet représente les indices des trois nœuds formant un triangle du maillage.

*Remarque sur la lecture :* Les indices des nœuds dans le fichier `.msh` commencent à 1, alors que les conteneurs C++ commencent à 0. Le constructeur effectue donc une décrémentation systématique (`t[0]--;`, etc.) lors du chargement des triangles pour adapter la numérotation.

#### C) Fonctionnement du programme de test
Le programme principal `src/main.cpp` réalise les opérations suivantes :
1. Il instancie l'objet `Mesh` en important le fichier de géométrie standard `../square.msh`. Le constructeur ouvre, parse et charge les données du fichier textuel en mémoire.
2. Il appelle la méthode de classe `m.writeGnuplot("gnu.plt");`. Cette fonction parcourt séquentiellement le tableau des triangles pour écrire leurs sommets dans un fichier script lisible par Gnuplot. L'instruction `set size ratio -1` y est insérée afin de préserver l'échelle géométrique réelle (1:1) lors de l'affichage à l'écran.

### 3.2 Recherche simple
L'algorithme de recherche naïve parcourt l'intégralité du tableau `triangle` (complexité algorithmique en $O(N)$ où $N$ est le nombre de triangles). 

Pour déterminer si le point $P(x,y)$ se trouve à l'intérieur d'un triangle $T$ formé par les sommets $P_0, P_1, P_2$, nous utilisons le signe du produit vectoriel 2D pour chaque arête orientée :
* $CP_1 = (X_1 - X_0)(Y_P - Y_0) - (Y_1 - Y_0)(X_P - X_0)$
* $CP_2 = (X_2 - X_1)(Y_P - Y_1) - (Y_2 - Y_1)(X_P - X_1)$
* $CP_3 = (X_0 - X_2)(Y_P - Y_2) - (Y_0 - Y_2)(X_P - X_2)$

Si $CP_1, CP_2$ et $CP_3$ possèdent tous le même signe (tous positifs ou tous négatifs), le point se situe nécessairement à l'intérieur de l'élément (ou sur sa frontière). Si aucun triangle ne valide cette condition, la fonction retourne `-1`.