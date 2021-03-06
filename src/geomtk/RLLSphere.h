#ifndef __GEOMTK_RLLSphere__
#define __GEOMTK_RLLSphere__

#include "geomtk.h"

typedef geomtk::BodyCoord BodyCoord;
typedef geomtk::ConfigManager ConfigManager;
template <class DataType, int NumTimeLevel>
using TimeLevels = geomtk::TimeLevels<DataType, NumTimeLevel>;
typedef geomtk::TimeManager TimeManager;
using boost::posix_time::ptime;
using boost::posix_time::hours;
using boost::posix_time::minutes;
using boost::posix_time::seconds;
using boost::gregorian::date;
using boost::gregorian::years;
using boost::gregorian::months;
using boost::gregorian::days;
using geomtk::durationFromString;
typedef geomtk::StampString StampString;
template <int NumTimeLevel>
using TimeLevelIndex = geomtk::TimeLevelIndex<NumTimeLevel>;

typedef geomtk::SphereCoord SpaceCoord;
typedef geomtk::SphereVelocity Velocity;
typedef geomtk::SphereDomain Domain;
typedef geomtk::RLLMesh Mesh;
typedef geomtk::RLLMeshIndex MeshIndex;
template <class DataType, int NumTimeLevel = 1>
using Field = geomtk::RLLField<DataType, NumTimeLevel>;
typedef geomtk::RLLVelocityField VelocityField;
typedef geomtk::RLLRegrid Regrid;
typedef geomtk::IOManager<geomtk::RLLDataFile> IOManager;
typedef geomtk::RLLFilter<Mesh> Filter;
typedef geomtk::Diagnostics<Mesh, Field, IOManager> Diagnostics;

const int FULL_DIMENSION = geomtk::RLLSpaceDimensions::FULL_DIMENSION;
const int FULL = geomtk::RLLStagger::GridType::FULL;
const int HALF = geomtk::RLLStagger::GridType::HALF;
const int CENTER = geomtk::RLLStagger::Location::CENTER;
const int X_FACE = geomtk::RLLStagger::Location::X_FACE;
const int Y_FACE = geomtk::RLLStagger::Location::Y_FACE;
const int Z_FACE = geomtk::RLLStagger::Location::Z_FACE;
const int XY_VERTEX = geomtk::RLLStagger::Location::XY_VERTEX;
using geomtk::PI;
using geomtk::PI2;
using geomtk::RAD;

typedef geomtk::DomainMetric<typename Mesh::DomainType> MetricType;
typedef mlpack::tree::CoverTree<MetricType, mlpack::tree::FirstPointIsRoot, mlpack::range::RangeSearchStat> TreeType;
typedef mlpack::range::RangeSearch<MetricType, TreeType> SearchType;

#endif // __GEOMTK_RLLSphere__
