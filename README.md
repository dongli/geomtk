GeoMTK
======

Introduction
------------

This toolkit (Geofluid Modeling Toolkit) is used to assist the development of
my Lagrangian dynamics program, but it may be useful for other projects.

The following items are enclosed:

- Domain class hierarchies;
- Mesh class hierarchies;
- Field class hierarchies;

Basic usage
-----------

```C++
#include "geomtk.h"

using geomtk::SphereDomain;
using geomtk::RLLMesh;
using geomtk::RLLScalarField;
using geomtk::RLLVectorField;

...

SphereDomain sphere(3);
RLLMesh mesh(sphere);
RLLScalarField q(mesh);
RLLVectorField v(mesh);

sphere.setRadius(6371.299e3);

// zonal virtual grids are added, which is apparent to user
q.create(CENTER, CENTER, CENTER);
// three components are included in v
v.create(EDGE, CENTER, CENTER, CENTER, EDGE, CENTER, CENTER, CENTER, EDGE);

for (int k = 0; k < mesh.getNumGrid(2, CENTER); ++k) {
	for (int j = 0; j < mesh.getNumGrid(1, CENTER); ++j) {
		for (int i = 0; i < mesh.getNumGrid(0, CENTER); ++i) {
			q(0, i, j, k) = ...;
		}
	}
}

```

Author
------

- Li Dong <dongli@lasg.iap.ac.cn>
