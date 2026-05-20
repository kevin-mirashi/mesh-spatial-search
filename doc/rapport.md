Projet C++ L3 Math : Manipulation de maillages 2D
1. Présentation du projet
Ce projet traite de la manipulation de maillages 2D en C++. L'objectif principal est de comparer différentes approches algorithmiques pour rechercher efficacement un triangle contenant un point donné, ainsi que pour retrouver les triangles voisins d'un triangle. Les maillages sont générés avec le logiciel Gmsh.
2. Structure du projet
Le dépôt est organisé de la façon suivante : le dossier src/ contient les fichiers sources C++ (main.cpp, msh.cpp, msh.h), CMakeLists.txt gère la compilation, et README.md constitue le rapport du projet.
Pour compiler et lancer le projet :
bashmkdir build
cd build
cmake ..
cmake --build .
./Debug/go.exe

3. Réponses aux Questions
3.1 Structure de données
A) Structure du fichier square.msh
Le fichier .msh est généré par Gmsh au format version 2. Il est découpé en plusieurs sections textuelles.
La section $Nodes commence par indiquer le nombre total de nœuds. Chaque ligne suivante donne l'identifiant du nœud et ses coordonnées (x, y, z).
La section $Elements indique le nombre total d'éléments. Chaque élément précise son type — le type 2 correspond aux triangles à 3 sommets — ainsi que les identifiants des nœuds qui le composent.
B) Structure de données Mesh en C++
La classe Mesh stocke le maillage en mémoire avec deux tableaux dynamiques. Le vecteur node contient les coordonnées (x, y) de chaque nœud sous la forme vector<array<double, 2>>. Le vecteur triangle contient pour chaque triangle les indices de ses 3 sommets sous la forme vector<array<int, 3>>.
Un point important : dans le fichier .msh, les nœuds sont numérotés à partir de 1, mais les tableaux C++ commencent à 0. Le constructeur décrémente donc chaque indice lors du chargement (t[0]--, etc.).
C) Fonctionnement du programme de test
Le main.cpp charge le maillage depuis ../square.msh, puis appelle writeGnuplot("gnu.plt") pour générer un fichier script visualisable avec Gnuplot. La ligne set size ratio -1 dans ce script force un affichage à l'échelle réelle (1:1), ce qui évite de déformer les triangles à l'écran.

3.2 Recherche simple
La recherche naïve parcourt tous les triangles un par un, ce qui donne une complexité en O(N).
Pour savoir si un point P(x,y) est dans un triangle T formé des sommets P0, P1, P2, on calcule les produits vectoriels 2D sur chaque arête orientée :

CP1 = (X1 - X0)(YP - Y0) - (Y1 - Y0)(XP - X0)
CP2 = (X2 - X1)(YP - Y1) - (Y2 - Y1)(XP - X1)
CP3 = (X0 - X2)(YP - Y2) - (Y0 - Y2)(XP - X2)

Si CP1, CP2 et CP3 ont tous le même signe, le point est à l'intérieur du triangle. Si aucun triangle ne valide ce test, la fonction retourne -1.

3.3 Recherche avec hashmap (Grille)
A) Principe
L'idée est de diviser le domaine en cases carrées de côté h. Chaque case est identifiée par une clé GridKey(i, j).
À la construction avec buildGrid, on parcourt tous les triangles et on les affecte aux cases qu'ils touchent, en se basant sur leur boîte englobante. Ces associations sont stockées dans une std::unordered_map.
Lors d'une recherche, on calcule directement en O(1) quelle case contient le point, puis on teste uniquement les quelques triangles de cette case, au lieu des centaines du maillage entier.
B) Résultats expérimentaux
Benchmark réalisé sur 10 000 points aléatoires dans [0,1] x [0,1], avec raf = 0.1 dans square.geo (environ 346 triangles).
Le temps total de la recherche naïve O(N) est de 91.12 ms. Le temps total de la recherche hashmap avec une grille 10x10 est de 4.63 ms. Le facteur d'accélération mesuré est d'environ x20.
Les deux algorithmes donnent exactement les mêmes résultats sur les 10 000 points, ce qui a été vérifié automatiquement par des assertions dans le code.
L'accélération s'explique simplement : au lieu de tester les 346 triangles pour chaque point, on ne teste que les 3 ou 4 triangles présents dans la case concernée.

3.4 Utilisation de bibliothèques alternatives
A) Limites de std::unordered_map
La hashmap standard fonctionne par chaînage : en cas de collision, elle crée un nouveau nœud dynamique avec un pointeur. Cela fragmente la mémoire et provoque des cache misses fréquents, ce qui peut ralentir les accès sur de gros maillages.
B) La bibliothèque parallel-hashmap
Une alternative intéressante est la bibliothèque parallel-hashmap disponible sur GitHub, développée par Gregory Popovitch et inspirée des Swiss Tables de Google.
Son principal avantage est l'adressage ouvert : les données sont stockées dans des tableaux contigus en mémoire, ce qui réduit fortement les cache misses. L'absence de pointeurs de chaînage allège aussi la structure d'environ 30 à 50% en mémoire. Sur des maillages très fins avec des millions de triangles, on peut espérer un gain supplémentaire de l'ordre de 1.5x à 2x par rapport à std::unordered_map.
L'intégration dans le projet nécessite simplement de remplacer #include <unordered_map> par le header de la bibliothèque et d'adapter le type de grid.

3.5 Recherche des triangles voisins
A) Définition et méthode naïve
Deux triangles sont voisins s'ils partagent exactement 2 nœuds, donc une arête commune.
L'approche naïve compare chaque triangle avec tous les autres : pour chaque paire (i, j), on compte les nœuds en commun. La complexité est en O(N²), ce qui devient très lent sur de grands maillages.
B) Approche optimisée par hashmap
Pour obtenir une complexité O(N), on utilise une hashmap dont la clé est une arête représentée par la paire triée (min(A,B), max(A,B)). Ce tri permet d'éviter les doublons liés à l'orientation de parcours.
L'algorithme fonctionne en deux étapes. On parcourt d'abord tous les triangles et on insère leurs 3 arêtes dans la hashmap en associant à chacune l'indice du triangle. Ensuite, une arête associée à 1 seul triangle est une arête de frontière, et une arête associée à 2 triangles est une arête interne : les deux triangles correspondants sont voisins.
C) Résultats expérimentaux
Sur le maillage square.msh avec raf = 0.1 (environ 346 triangles), le programme a trouvé 543 arêtes uniques au total, dont 48 arêtes sur la frontière externe et 495 arêtes internes partagées entre deux triangles voisins.
La méthode naïve O(N²) a donné le même résultat de 495 arêtes internes, ce qui confirme que l'algorithme hashmap est correct.