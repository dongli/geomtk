#ifndef __GEOMTK_Cartesian__
#define __GEOMTK_Cartesian__

#include "geomtk.h"

typedef geomtk::ConfigManager ConfigManager;
typedef geomtk::SpaceCoord SpaceCoord;
typedef geomtk::CartesianDomain Domain;
typedef geomtk::CartesianMesh Mesh;
template <class DataType, int NumTimeLevel = 1>
using Field = geomtk::CartesianField<DataType, NumTimeLevel>;
typedef geomtk::Time Time;
typedef geomtk::TimeManager TimeManager;
typedef geomtk::StampString StampString;
typedef geomtk::IOManager<geomtk::CartesianDataFile> IOManager;
template <int NumTimeLevel>
using TimeLevelIndex = geomtk::TimeLevelIndex<NumTimeLevel>;
int FULL_DIMENSION = geomtk::CartesianSpaceDimensions::FULL_DIMENSION;
int FULL = geomtk::CartesianStagger::GridType::FULL;
int HALF = geomtk::CartesianStagger::GridType::HALF;
int CENTER = geomtk::CartesianStagger::Location::CENTER;
int X_FACE = geomtk::CartesianStagger::Location::X_FACE;

#endif
