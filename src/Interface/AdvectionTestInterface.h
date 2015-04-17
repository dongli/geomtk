#ifndef __GEOMTK_AdvectionTestInterface__
#define __GEOMTK_AdvectionTestInterface__

#include "AdvectionManagerInterface.h"

namespace geomtk {

template <int NumTimeLevel, class DomainType, class MeshType, 
          class IOManager, class VelocityFieldType>
class AdvectionTestInterface {
public:
    typedef AdvectionManagerInterface<NumTimeLevel, DomainType, MeshType, VelocityFieldType> AdvectionManager;
protected:
    DomainType *_domain;
    MeshType *_mesh;
    TimeManager *timeManager;
    IOManager io;
    int outputFileIdx;
    VelocityFieldType velocityField;
    Time _startTime;
    Time _endTime;
    double _stepSize;
public:
    AdvectionTestInterface() {
        _domain = NULL;
        _mesh = NULL;
    }
    virtual ~AdvectionTestInterface() {
        if (_domain != NULL) delete _domain;
        if (_mesh != NULL) delete _mesh;
    }

    virtual const DomainType&
    domain() const {
        return *_domain;
    }

    virtual const MeshType&
    mesh() const {
        return *_mesh;
    }

    const Time&
    startTime() const {
        return _startTime;
    }

    const Time&
    endTime() const {
        return _endTime;
    }

    double
    stepSize() const {
        return _stepSize;
    }

    virtual void
    init(const ConfigManager &configManager, TimeManager &timeManager) = 0;

    virtual void
    setInitialCondition(AdvectionManager &advectionManager) = 0;

    virtual void
    advanceDynamics(AdvectionManager &advectionManager) = 0;

    virtual void
    output(const TimeLevelIndex<NumTimeLevel> &timeIdx,
           AdvectionManager &advectionManager) = 0;
}; // AdvectionTestInterface

} // geomtk

#endif // __GEOMTK_AdvectionTestInterface__