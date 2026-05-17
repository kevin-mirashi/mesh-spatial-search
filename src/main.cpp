#include <msh.h>

int main()
{
    Mesh m("../square.msh");
    
    // Test 1 : Un point au milieu du domaine
    double px1 = 0.5, py1 = 0.5;
    int t1 = m.findTriangleNaive(px1, py1);
    cout << "Recherche du point (" << px1 << ", " << py1 << ") : ";
    if (t1 != -1)
        cout << "Trouve dans le triangle n°" << t1 << endl;
    else
        cout << "Non trouve (HORS MAILLAGE)" << endl;

    // Test 2 : Un point en dehors du maillage
    double px2 = 1.5, py2 = 1.5;
    int t2 = m.findTriangleNaive(px2, py2);
    cout << "Recherche du point (" << px2 << ", " << py2 << ") : ";
    if (t2 != -1)
        cout << "Trouve dans le triangle n°" << t2 << endl;
    else
        cout << "Non trouve (HORS MAILLAGE) -> Correct !" << endl;

    m.writeGnuplot("gnu.plt");
    return 0;
}