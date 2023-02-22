
param N integer > 2; # Number of nodes

set Nodes ordered := {1..N};

set Arcs := {i in Nodes, j in Nodes: i <> j};

param C{(i,j) in Arcs};

var x{i in 1 ..N, j in 1 ..N} binary;

var u{i in 2 ..N}>=0;

minimize Tourlength: sum {(i,j) in Arcs} C[i,j]*x[i,j];

subject to Degree1 {i in Nodes}: sum{(i,j) in Arcs} x[i,j]=1;

subject to Degree2 {i in Nodes}: sum{(j,i) in Arcs} x[j,i]=1;

subject to NoSubtour1 {(i,j) in Arcs: i<>j and i>=2 and j>=2}:
u[i] - u[j] + N*x[i,j] <= N - 1;

subject to NoSubtour2 {i in Nodes: i >= 2}:
u[i] <= N - 1 - (N - 2)*x[1,i];

subject to NoSubtour3 {i in Nodes: i >= 2}:
u[i] >= 1 + (N - 2)*x[i,1];
