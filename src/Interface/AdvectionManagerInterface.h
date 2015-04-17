#ifndef __GEOMTK_AdvectionManagerInterface__
#define __GEOMTK_AdvectionManagerInterface__

namespace geomtk {

template <int NumTimeLevel, class DomainType, class MeshType, class VelocityFieldType>
class AdvectionManagerInterface {
protected:
    const DomainType *domain;
    const MeshType *mesh;
public:
    virtual void
    init(const ConfigManager &configManager, const MeshType &mesh) = 0;

    virtual void
    addTracer(const string &name, const string &unit, const string &comment) = 0;

    virtual void
    advance(double dt, const TimeLevelIndex<NumTimeLevel> &newIdx,
            const VelocityFieldType &velocityField) = 0;

    virtual void
    input(const TimeLevelIndex<NumTimeLevel> &timeIdx, double *q) = 0;

    virtual void
    output(const TimeLevelIndex<NumTimeLevel> &timeIdx, int ncId) = 0;
}; // AdvectionManagerInterface

} // geomtk

#endif // __GEOMTK_AdvectionManagerInterface__