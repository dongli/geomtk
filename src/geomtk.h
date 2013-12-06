#ifndef __GEOMTK__
#define __GEOMTK__

/**
 * Main header file for the GeoMTK C++ API.
 *
 * Copyright 2013.
 *
 * Authors: Li Dong <dongli@lasg.iap.ac.cn>
 */

#include "geomtk/commons.h"
#include "geomtk/TimeLevels.h"
// Domain class hierarchy
#include "geomtk/Domain/Domain.h"
#include "geomtk/Domain/SphereDomain.h"
// Mesh class hierarchy
#include "geomtk/Mesh/Mesh.h"
#include "geomtk/Mesh/MeshIndex.h"
#include "geomtk/Mesh/StructuredMesh.h"
#include "geomtk/Mesh/StructuredMeshIndex.h"
#include "geomtk/Mesh/RLLMesh.h"
#include "geomtk/Mesh/RLLMeshIndex.h"
// Field class hierarchy
#include "geomtk/Field/Field.h"
#include "geomtk/Field/StructuredScalarField.h"
#include "geomtk/Field/StructuredVectorField.h"
#include "geomtk/Field/RLLScalarField.h"
#include "geomtk/Field/RLLVectorField.h"
#include "geomtk/Field/RLLVelocityField.h"
// Regrid class hierarchy
#include "geomtk/Regrid/Regrid.h"
#include "geomtk/Regrid/RLLRegrid.h"
#include "geomtk/Regrid/StructuredRegrid.h"

#endif
