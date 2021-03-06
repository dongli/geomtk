#ifndef __GEOMTK_Cartesian__
#define __GEOMTK_Cartesian__

#include "geomtk.h"

typedef geomtk::BodyCoord BodyCoord;
typedef geomtk::ConfigManager ConfigManager;
template <class DataType, int NumTimeLevel>
using TimeLevels = geomtk::TimeLevels<DataType, NumTimeLevel>;
typedef boost::posix_time::ptime Time;
typedef boost::gregorian::date Date;
typedef boost::posix_time::seconds Seconds;
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
typedef geomtk::RegridMethod RegridMethod;
typedef geomtk::IOManager<geomtk::CartesianDataFile> IOManager;
typedef geomtk::Diagnostics<Mesh, Field, IOManager> Diagnostics;

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

typedef geomtk::DomainMetric<typename Mesh::DomainType> MetricType;
typedef mlpack::tree::CoverTree<MetricType, mlpack::tree::FirstPointIsRoot, mlpack::range::RangeSearchStat> TreeType;
typedef mlpack::range::RangeSearch<MetricType, TreeType> SearchType;

#endif // __GEOMTK_Cartesian__
