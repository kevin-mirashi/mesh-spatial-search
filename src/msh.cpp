#include "msh.h"
#include <cassert>

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
