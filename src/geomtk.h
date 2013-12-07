#ifndef __GEOMTK__
#define __GEOMTK__

/**
 * Main header file for the GeoMTK C++ API.
 *
 * Copyright 2013.
 *
 * Authors:
 *   - Li Dong <dongli@lasg.iap.ac.cn>
 */

#include "geomtk/commons.h"
#include "geomtk/TimeLevels.h"
// Domain class hierarchy
#include "geomtk/Domain.h"
#include "geomtk/SphereDomain.h"
// Mesh class hierarchy
#include "geomtk/Mesh.h"
#include "geomtk/MeshIndex.h"
#include "geomtk/StructuredMesh.h"
#include "geomtk/StructuredMeshIndex.h"
#include "geomtk/RLLMesh.h"
#include "geomtk/RLLMeshIndex.h"
// Field class hierarchy
#include "geomtk/Field.h"
#include "geomtk/StructuredScalarField.h"
#include "geomtk/StructuredVectorField.h"
#include "geomtk/RLLScalarField.h"
#include "geomtk/RLLVectorField.h"
#include "geomtk/RLLVelocityField.h"
// Regrid class hierarchy
#include "geomtk/Regrid.h"
#include "geomtk/RLLRegrid.h"
#include "geomtk/StructuredRegrid.h"

#endif
