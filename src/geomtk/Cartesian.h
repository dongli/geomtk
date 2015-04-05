#ifndef __GEOMTK_Cartesian__
#define __GEOMTK_Cartesian__

#include "geomtk.h"

typedef geomtk::BodyCoord BodyCoord;
typedef geomtk::ConfigManager ConfigManager;
typedef geomtk::Time Time;
typedef geomtk::TimeUnit TimeUnit;
typedef geomtk::TimeStepUnit TimeStepUnit;
template <class DataType, int NumTimeLevel>
using TimeLevels = geomtk::TimeLevels<DataType, NumTimeLevel>;
typedef geomtk::TimeManager TimeManager;
typedef geomtk::StampString StampString;
template <int NumTimeLevel>
using TimeLevelIndex = geomtk::TimeLevelIndex<NumTimeLevel>;

typedef geomtk::SpaceCoord SpaceCoord;
typedef geomtk::Velocity Velocity;
typedef geomtk::CartesianDomain Domain;
typedef geomtk::CartesianMesh Mesh;
typedef geomtk::CartesianMeshIndex MeshIndex;
template <class DataType, int NumTimeLevel = 1>
using Field = geomtk::CartesianField<DataType, NumTimeLevel>;
typedef geomtk::CartesianVelocityField VelocityField;
typedef geomtk::CartesianRegrid Regrid;
typedef geomtk::IOManager<geomtk::CartesianDataFile> IOManager;

const int FULL_DIMENSION = geomtk::CartesianSpaceDimensions::FULL_DIMENSION;
const int FULL = geomtk::CartesianStagger::GridType::FULL;
const int HALF = geomtk::CartesianStagger::GridType::HALF;
const int CENTER = geomtk::CartesianStagger::Location::CENTER;
const int X_FACE = geomtk::CartesianStagger::Location::X_FACE;
const int Y_FACE = geomtk::CartesianStagger::Location::Y_FACE;
const int Z_FACE = geomtk::CartesianStagger::Location::Z_FACE;
const int XY_VERTEX = geomtk::CartesianStagger::Location::XY_VERTEX;
using geomtk::PI;
using geomtk::PI2;
using geomtk::RAD;

#endif
