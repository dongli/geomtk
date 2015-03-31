#ifndef __GEOMTK_Cartesian__
#define __GEOMTK_Cartesian__

#include "geomtk.h"

using geomtk::ConfigManager;
using geomtk::SpaceCoord;
using geomtk::Velocity;
typedef geomtk::CartesianDomain Domain;
typedef geomtk::CartesianMesh Mesh;
typedef geomtk::CartesianMeshIndex MeshIndex;
typedef geomtk::CartesianRegrid Regrid;
using geomtk::RegridMethod;
template <class DataType, int NumTimeLevel = 1>
using Field = geomtk::CartesianField<DataType, NumTimeLevel>;
using geomtk::Time;
using geomtk::TimeManager;
using geomtk::StampString;
typedef geomtk::IOManager<geomtk::CartesianDataFile> IOManager;
template <int NumTimeLevel>
using TimeLevelIndex = geomtk::TimeLevelIndex<NumTimeLevel>;
int FULL_DIMENSION = geomtk::CartesianSpaceDimensions::FULL_DIMENSION;
int FULL = geomtk::CartesianStagger::GridType::FULL;
int HALF = geomtk::CartesianStagger::GridType::HALF;
int CENTER = geomtk::CartesianStagger::Location::CENTER;
int X_FACE = geomtk::CartesianStagger::Location::X_FACE;

using geomtk::PI;
using geomtk::PI2;
using geomtk::RAD;

#endif
