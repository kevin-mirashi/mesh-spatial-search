// maillage avec gmsh square.geo -2 -format msh2
raf= 0.09;
L=1;
Point(1)= {0,0,0,raf};
Point(2)= {L,0,0,raf};
Point(3)= {L,L,0,raf};
Point(4)= {0,L,0,raf};

Line(1)= {1,2};
Line(2)= {2,3};
Line(3)= {3,4};
Line(4)= {4,1};

Line Loop(1)= {1,2,3,4};
Plane Surface(1)= {1};

Mesh.ElementOrder=1 ;
Mesh.SecondOrderIncomplete=1;

