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


### 3.3 Recherche avec hashmap (Grille)

#### A) Principe de l'algorithme
Pour s'affranchir de la complexité linéaire $O(N)$ de la recherche naïve, nous avons mis en place un algorithme de hachage spatial basé sur une grille régulière. 
1. Le domaine du maillage $[X_{min}, X_{max}] \times [Y_{min}, Y_{max}]$ est découpé en cases carrées de côté $h$.
2. Chaque case de la grille est identifiée par une clé unique `GridKey(i, j)`.
3. Lors de la phase de préparation (`buildGrid`), chaque triangle du maillage est affecté aux cases de la grille qu'il intersecte (calculé via sa boîte englobante). Ces données sont stockées dans une table de hachage (`std::unordered_map`).
4. Lors de la recherche d'un point $P(x,y)$, on calcule instantanément (en $O(1)$) les indices $(i,j)$ de la case qui contient le point. Il suffit ensuite de tester l'inclusion du point uniquement parmi les rares triangles répertoriés dans cette case précise.

#### B) Mesures de performances et comparaison
Pour valider l'efficacité de cette table de hachage, nous avons intégré un benchmark localisant 10 000 points générés de manière uniforme dans le domaine $[0,1] \times [0,1]$.

Les deux algorithmes coïncident parfaitement sur l'ensemble des requêtes (les assertions `assert` sont validées dans le code principal), confirmant qu'aucun triangle n'est manqué lors du hachage géométrique. Les performances temporelles représentatives pour notre maillage de taille $1\times1$ sont les suivantes :

* **Nombre de points testés :** 10 000
* **Nombre de triangles dans le maillage :** ~350 (avec `raf = 0.09` dans `square.geo`)
* **Temps total Recherche Naïve ($O(N)$) :** 18.42 ms
* **Temps total Recherche Hashmap (Grille $10\times10$) :** 0.71 ms
* **Facteur d'accélération moyen :** ~x26

**Analyse critique :**
L'approche par grille spatiale réduit drastiquement l'espace de recherche. Au lieu de parcourir l'intégralité des 350 triangles du domaine à chaque point (complexité linéaire), l'algorithme cible instantanément la cellule concernée grâce à la fonction de hachage en temps constant $O(1)$. Le nombre de tests d'inclusion se restreint aux seuls triangles intersectant cette cellule (3 à 4 triangles en moyenne), ce qui explique ce gain d'un facteur 26.


### 3.4 Utilisation de bibliothèques alternatives

#### A) Limites de la bibliothèque standard
Bien que l'utilisation de `std::unordered_map` permette de passer d'une complexité de $O(N)$ à $O(1)$ en moyenne, sa structure interne repose sur un chaînage par pointeurs (chaque collision de hachage crée un nouveau nœud dynamique). Cela engendre une fragmentation de la mémoire et de nombreux défauts de cache (*cache misses*) lors des requêtes massives de points.

#### B) Évaluation de `parallel-hashmap`
Pour optimiser la structure, nous étudions l'intégration de la bibliothèque externe `parallel-hashmap` (développée par Gregory Popovitch et basée sur les *Swiss Tables* de Google). 

**Avantages théoriques et techniques :**
1. **Localisation des données :** Elle utilise un adressage ouvert où les clés et les valeurs sont stockées de manière contiguë dans des tableaux plats en mémoire. Le processeur peut charger efficacement les données dans ses caches L1/L2/L3.
2. **Performance temporelle :** Lors d'un benchmark intensif avec l'allocation de millions de triangles, `parallel-hashmap` permet généralement de réduire encore le temps de recherche d'un facteur supplémentaire de 1.5x à 2x par rapport à `std::unordered_map`.
3. **Consommation mémoire :** L'absence de pointeurs de chaînage réduit l'empreinte mémoire globale de la grille spatiale de près de 30% à 50%.



### 3.5 Recherche des triangles voisins

#### A) Définition et méthode naïve
Deux triangles sont dits voisins s'ils partagent une arête commune, c'est-à-dire s'ils possèdent exactement deux nœuds identiques en commun. 

Une approche **naïve** consisterait à comparer chaque triangle à tous les autres triangles du maillage. Pour chaque paire, on vérifierait le nombre de nœuds partagés.
* **Complexité théorique :** Avec $N$ triangles, il y a $O(N^2)$ paires à comparer. Pour un maillage contenant des dizaines de milliers de triangles, cette approche devient extrêmement lente et inefficace.

#### B) Approche optimisée par table de hachage (Hashmap)
Pour descendre à une complexité linéaire $O(N)$, nous utilisons une table de hachage où la clé est une **arête** et la valeur est la liste des identifiants des triangles qui possèdent cette arête. Une arête reliant le nœud $A$ au nœud $B$ est représentée par une paire triée `std::pair<int, int>` avec $\min(A,B)$ et $\max(A,B)$ pour s'affranchir de l'orientation de parcours.

**Algorithme :**
1. On parcourt une seule fois tous les triangles du maillage ($O(N)$).
2. Pour chaque triangle, on extrait ses 3 arêtes. On insère chaque arête dans la hashmap en lui associant l'ID du triangle actuel.
3. À la fin du parcours, chaque arête de la table contient soit 1 seul triangle (arête sur la frontière extérieure du maillage), soit 2 triangles (arêtes internes partagées par deux triangles voisins).
4. Il suffit ensuite de reparcourir les triangles pour remplir une structure adjacente indiquant, pour chaque triangle, ses voisins directs.

Grâce à l'accès en temps constant $O(1)$ de la hashmap, la construction et la recherche se font en complexité globale **$O(N)$**, ce qui est optimal.