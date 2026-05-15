#include <msh.h>


int main()
{
    Mesh m("../square.msh");
    //cout << m << endl;
    m.writeGnuplot("gnu.plt");
    return 0;
}