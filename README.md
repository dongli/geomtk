Introduction
------------

This toolkit (Geofluid Modeling Toolkit) is used to assist the development of
my Lagrangian dynamics program, but it may be useful for other projects.

The following items are enclosed:

- SpaceCoord class hierarchy;
- Domain class hierarchy;
- Mesh class hierarchy;
- Field class hierarchy;
- Regrid class hierarchy;

The Lagrangian advection scheme that I am developing should be able to be
applied to models with different mesh systems. The above `Mesh`, `Field` and
`Regrid` class hierarchies are designed for this purpose by hiding the mesh
details from the scheme, so the switching among models will be easy and the
core of the scheme will not be changed.

Basic usage
-----------

```C++
#include "geomtk.h"

// Choose the specific classes for use.
typedef geomtk::SphereDomain Domain;
typedef geomtk::RLLMesh Mesh;
typedef geomtk::RLLField<double, 2> Field;
typedef geomtk::TimeManager TimeManager;
typedef geomtk::TimeLevelIndex<2> TimeLevelIndex;
typedef geomtk::IOManager<geomtk::RLLDataFile> IOManager;
typedef geomtk::ConfigManager ConfigManager;

int CENTER = geomtk::RLLStagger::Location::CENTER;
...

Domain sphere(3);
Mesh mesh(sphere);
Field q(); // Field with two time levels.
TimeLevelIndex timeIdx; // Time level index.

sphere.setRadius(6371.299e3);

// Initialize the regular lat-lon mesh.
mesh.init(128, 64);

// zonal virtual grids are added, which is apparent to user
q.create("q", "kg m-3", "moisture density", mesh, CENTER, sphere.getNumDim());

for (int k = mesh.ks(FULL); k <= mesh.ke(FULL); ++k) {
	for (int j = mesh.js(FULL); j <= mesh.je(FULL); ++j) {
		for (int i = mesh.is(FULL); i <= mesh.ie(FULL); ++i) {
			q(timeIdx, i, j, k) = ...;
		}
	}
}

// Shift time level index.
timeIdx.shift();

...

```

Author
------

- Li Dong <dongli@lasg.iap.ac.cn>
