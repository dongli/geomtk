#ifndef __GEOMTK_AdvectionManagerInterface__
#define __GEOMTK_AdvectionManagerInterface__

namespace geomtk {

template <int NumTimeLevel, class DomainType, class MeshType,
          template<typename, int> class FieldType, class VelocityFieldType>
class AdvectionManagerInterface {
protected:
    const DomainType *domain;
    const MeshType *mesh;
public:
    virtual void
    init(const MeshType &mesh) = 0;

    virtual void
    addTracer(const string &name, const string &unit, const string &comment) = 0;

    virtual void
    advance(double dt, const TimeLevelIndex<NumTimeLevel> &newIdx,
            const VelocityFieldType &velocityField) = 0;

    virtual void
    input(const TimeLevelIndex<NumTimeLevel> &timeIdx, double *q) = 0;

    virtual void
    output(const TimeLevelIndex<NumTimeLevel> &timeIdx, int ncId) const = 0;

    virtual double&
    density(const TimeLevelIndex<NumTimeLevel> &timeIdx,
            int tracerIdx, int cellIdx) = 0;

    virtual const FieldType<double, NumTimeLevel>&
    density(int tracerIdx) const = 0;

    virtual FieldType<double, NumTimeLevel>&
    density(int tracerIdx) = 0;

    virtual double&
    tendency(int tracerIdx, int cellIdx) = 0;

    virtual const FieldType<double, 1>&
    tendency(int tracerIdx) const = 0;

    virtual FieldType<double, 1>&
    tendency(int tracerIdx) = 0;
}; // AdvectionManagerInterface

} // geomtk

#endif // __GEOMTK_AdvectionManagerInterface__