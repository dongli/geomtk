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

#include "geomtk_commons.h"
#include "TimeLevels.h"
#include "TimeManager.h"
#include "StampString.h"
#include "SystemTools.h"
#include "IOManager.h"
#include "ConfigManager.h"
// Domain class hierarchy
#include "Domain.h"
#include "SphereDomain.h"
// Mesh class hierarchy
#include "Mesh.h"
#include "MeshIndex.h"
#include "StructuredMesh.h"
#include "StructuredMeshIndex.h"
#include "CartesianMesh.h"
#include "CartesianMeshIndex.h"
#include "RLLMesh.h"
#include "RLLMeshIndex.h"
// Field class hierarchy
#include "Field.h"
#include "CartesianField.h"
#include "RLLField.h"
#include "RLLVelocityField.h"
// Regrid class hierarchy
#include "Regrid.h"
#include "RLLRegrid.h"
#include "CartesianRegrid.h"

#endif
