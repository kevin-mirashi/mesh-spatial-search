#include "msh.h"
#include <cassert>
#include <string>
#include <map>

// classe maillage 2d avec liste de noeuds et liste de triangles
Mesh::Mesh(string gmsh2_filename)
{
    ifstream file = ifstream(gmsh2_filename);
    if (!file.is_open()) {
        throw runtime_error("file not found");
    }
    string line;
    while (getline(file, line))
    {
        if (line == "$Nodes")
        {
            int n;
            file >> n;
            node.resize(n);
            for (int i = 0; i < n; i++)
            {
                int id;
                double z;
                file >> id;
                file >> node[id - 1][0] >> node[id - 1][1] >> z;
            }
        }
        if (line == "$Elements")
        {
            int n;
            file >> n;
            for (int i = 0; i < n; i++)
            {
                int id, type, ntags;
                file >> id >> type >> ntags;
                for (int j = 0; j < ntags; j++)
                {
                    int tag;
                    file >> tag;
                }
                if (type == 2)
                {
                    array<int, 3> t;
                    file >> t[0] >> t[1] >> t[2];
                    t[0]--;
                    t[1]--;
                    t[2]--;
                    triangle.push_back(t);
                } // sinon sauter la ligne (on ne connait pas sa taille)
                else
                {
                    string line;
                    getline(file, line);
                }
            }
        }
    }
};




bool Mesh::isPointInTriangle(array<double, 2> p, int t_idx) const {
    int n0 = triangle[t_idx][0];
    int n1 = triangle[t_idx][1];
    int n2 = triangle[t_idx][2];

    // Coordonnées des sommets
    double x0 = node[n0][0], y0 = node[n0][1];
    double x1 = node[n1][0], y1 = node[n1][1];
    double x2 = node[n2][0], y2 = node[n2][1];

    // Calcul des produits vectoriels 2D (signe des orientations)
    double d1 = (p[0] - x1) * (y0 - y1) - (x0 - x1) * (p[1] - y1);
    double d2 = (p[0] - x2) * (y1 - y2) - (x1 - x2) * (p[1] - y2);
    double d3 = (p[0] - x0) * (y2 - y0) - (x2 - x0) * (p[1] - y0);

    bool has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    bool has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    // Le point est dans le triangle s'ils ont tous le même signe
    return !(has_neg && has_pos);
}



// affichage dans un fichier lisible par gnuplot
void Mesh::writeGnuplot(const string &filename) const
{
    ofstream file(filename);
    // file << "set terminal postscript eps enhanced color" << endl;
    // file << "set output '" << filename << ".eps'" << endl;
    //  file << "set xrange [-1:1]" << endl;
    //  file << "set yrange [-1:1]" << endl;
    //  freeze aspect ratio
    file << "set size ratio -1" << endl;
    file << "plot '-' with linespoints" << endl;
    for (int i = 0; i < triangle.size(); i++)
    {
        file << node[triangle[i][0]][0] << " " << node[triangle[i][0]][1] << endl;
        file << node[triangle[i][1]][0] << " " << node[triangle[i][1]][1] << endl;
        file << node[triangle[i][2]][0] << " " << node[triangle[i][2]][1] << endl;
        file << node[triangle[i][0]][0] << " " << node[triangle[i][0]][1] << endl;
        file << endl;
    }
    file << "e" << endl;
    file << "pause -1";
    file.close();
}

ostream &operator<<(ostream &os, const Mesh &m)
{
    os << "Mesh with " << m.node.size() << " nodes and " << m.triangle.size() << " triangles" << endl;
    for (int i = 0; i < m.node.size(); i++)
    {
        os << "Node " << i << ": " << m.node[i][0] << " " << m.node[i][1] << endl;
    }
    for (int i = 0; i < m.triangle.size(); i++)
    {
        os << "Triangle " << i << ": " << m.triangle[i][0] << " " << m.triangle[i][1] << " " << m.triangle[i][2] << endl;
    }
    return os;
}


int Mesh::findTriangleNaive(double x, double y) const
{
    // On parcourt tous les triangles du maillage
    for (int i = 0; i < triangle.size(); i++)
    {
        // Récupération des coordonnées des 3 sommets du triangle i
        double x0 = node[triangle[i][0]][0];
        double y0 = node[triangle[i][0]][1];
        
        double x1 = node[triangle[i][1]][0];
        double y1 = node[triangle[i][1]][1];
        
        double x2 = node[triangle[i][2]][0];
        double y2 = node[triangle[i][2]][1];

        // Calcul des composantes du produit vectoriel 2D pour chaque arête
        // Arête 0 -> 1 avec le point P(x,y)
        double cp1 = (x1 - x0) * (y - y0) - (y1 - y0) * (x - x0);
        
        // Arête 1 -> 2 avec le point P(x,y)
        double cp2 = (x2 - x1) * (y - y1) - (y2 - y1) * (x - x1);
        
        // Arête 2 -> 0 avec le point P(x,y)
        double cp3 = (x0 - x2) * (y - y2) - (y0 - y2) * (x - x2);

        // Si les trois ont le même signe, le point est dans le triangle i
        bool tous_positifs = (cp1 >= 0 && cp2 >= 0 && cp3 >= 0);
        bool tous_negatifs = (cp1 <= 0 && cp2 <= 0 && cp3 <= 0);

        if (tous_positifs || tous_negatifs)
        {
            return i; // On a trouvé le triangle, on renvoie son numéro
        }
    }

    return -1; // Le point n'est dans aucun triangle
}


void Mesh::buildGrid(double step) {
    if (node.empty() || triangle.empty()) return;

    h_grid = step;
    grid.clear();

    // 1. Trouver les bornes de la boîte englobante (Bounding Box)
    x_min = x_max = node[0][0];
    y_min = y_max = node[0][1];
    for (const auto& n : node) {
        if (n[0] < x_min) x_min = n[0];
        if (n[0] > x_max) x_max = n[0];
        if (n[1] < y_min) y_min = n[1];
        if (n[1] > y_max) y_max = n[1];
    }

    // 2. Remplir la grille
    // Pour chaque triangle, on détermine les cases intersectées par ses sommets
    for (int t_idx = 0; t_idx < triangle.size(); ++t_idx) {
        // On récupère les indices des 3 nœuds du triangle
        int n0 = triangle[t_idx][0];
        int n1 = triangle[t_idx][1];
        int n2 = triangle[t_idx][2];

        // Pour faire simple et robuste, on trouve la bounding box du triangle 't_idx'
        double t_xmin = min({node[n0][0], node[n1][0], node[n2][0]});
        double t_xmax = max({node[n0][0], node[n1][0], node[n2][0]});
        double t_ymin = min({node[n0][1], node[n1][1], node[n2][1]});
        double t_ymax = max({node[n0][1], node[n1][1], node[n2][1]});

        // Convertir ces coordonnées physiques en indices de cases (i, j) dans la grille
        int i_start = static_cast<int>((t_xmin - x_min) / h_grid);
        int i_end   = static_cast<int>((t_xmax - x_min) / h_grid);
        int j_start = static_cast<int>((t_ymin - y_min) / h_grid);
        int j_end   = static_cast<int>((t_ymax - y_min) / h_grid);

        // Assigner le triangle à toutes les cases qu'il touche potentiellement
        for (int i = i_start; i <= i_end; ++i) {
            for (int j = j_start; j <= j_end; ++j) {
                GridKey key{i, j};
                grid[key].push_back(t_idx);
            }
        }
    }
}



int Mesh::findTriangleHash(array<double, 2> p) const {
    // 1. Trouver les indices de la case (i, j) dans la grille
    int i = static_cast<int>((p[0] - x_min) / h_grid);
    int j = static_cast<int>((p[1] - y_min) / h_grid);

    GridKey key{i, j};

    // 2. Chercher la case dans la hashmap
    auto it = grid.find(key);
    if (it != grid.end()) {
        // 3. Parcourir uniquement les triangles de cette case
        const vector<int>& potential_triangles = it->second;
        for (int t_idx : potential_triangles) {
            if (isPointInTriangle(p, t_idx)) {
                return t_idx; // On a trouvé le triangle !
            }
        }
    }

    return -1; // Le point n'est dans aucun triangle
}


#include <map>

void Mesh::findVoisins() const {
    // Table de hachage : Arête (paire triée de IDs de nœuds) -> Liste d'IDs de triangles
    unordered_map<pair<int, int>, vector<int>, PairHash> edge_to_triangles;

    // 1. Remplissage de la hashmap avec les arêtes de chaque triangle
    for (size_t t_idx = 0; t_idx < triangle.size(); ++t_idx) {
        int n0 = triangle[t_idx][0];
        int n1 = triangle[t_idx][1];
        int n2 = triangle[t_idx][2];

        // Les 3 arêtes du triangle actuel (ordonnées pour éviter les problèmes d'orientation)
        array<pair<int, int>, 3> edges = {
            make_pair(min(n0, n1), max(n0, n1)),
            make_pair(min(n1, n2), max(n1, n2)),
            make_pair(min(n2, n0), max(n2, n0))
        };

        for (const auto& edge : edges) {
            edge_to_triangles[edge].push_back(t_idx);
        }
    }

    // 2. Analyse et affichage des résultats (Preuve de bon fonctionnement)
    int frontieres = 0;
    int internes = 0;

    for (const auto& item : edge_to_triangles) {
        if (item.second.size() == 1) {
            frontieres++; // L'arête n'appartient qu'à un seul triangle
        } else if (item.second.size() == 2) {
            internes++;   // L'arête est partagée par deux triangles voisins
        }
    }

    cout << "--- RESULTATS ALGORITHME DE RECHERCHE DE VOISINS (Q3.5) ---" << endl;
    cout << "Total des aretes uniques traitees : " << edge_to_triangles.size() << endl;
    cout << "Aretes sur la frontiere externe    : " << frontieres << endl;
    cout << "Aretes internes (partagees)       : " << internes << endl;
    cout << "--------------------------------------------------------" << endl;
}



void Mesh::findVoisinsNaive() const {
    int aretes_internes_naives = 0;

    // Boucle double O(N^2) pour comparer chaque paire de triangles unique
    for (size_t i = 0; i < triangle.size(); ++i) {
        for (size_t j = i + 1; j < triangle.size(); ++j) {
            // Compter le nombre de nœuds en commun entre le triangle i et le triangle j
            int noeuds_communs = 0;
            for (int ni = 0; ni < 3; ++ni) {
                for (int nj = 0; nj < 3; ++nj) {
                    if (triangle[i][ni] == triangle[j][nj]) {
                        noeuds_communs++;
                    }
                }
            }
            // S'ils partagent exactement 2 nœuds, ils partagent une arête (ils sont voisins)
            if (noeuds_communs == 2) {
                aretes_internes_naives++;
            }
        }
    }

    cout << "--- VALIDATION METHODE NAÏVE (O(N^2)) ---" << endl;
    cout << "Aretes internes trouvees (Naive) : " << aretes_internes_naives << endl;
    cout << "--------------------------------------------------------" << endl;
}