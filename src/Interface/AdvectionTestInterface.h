#ifndef __GEOMTK_AdvectionTestInterface__
#define __GEOMTK_AdvectionTestInterface__

#include "AdvectionManagerInterface.h"

namespace geomtk {

template <int NumTimeLevel, class DomainType, class MeshType, 
          class FieldType, class VelocityFieldType, class IOManager>
class AdvectionTestInterface {
public:
    typedef AdvectionManagerInterface<NumTimeLevel, DomainType, MeshType, VelocityFieldType> AdvectionManager;
protected:
    DomainType *_domain;
    MeshType *_mesh;
    TimeManager _timeManager;
    IOManager io;
    int outputIdx;
    VelocityFieldType velocityField;
    vector<FieldType> densities;
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

    virtual const TimeManager&
    timeManager() const {
        return _timeManager;
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
    init(const ConfigManager &configManager,
         AdvectionManager &advectionManager) = 0;

    virtual void
    setInitialCondition(AdvectionManager &advectionManager) = 0;

    virtual void
    advanceDynamics(const TimeLevelIndex<NumTimeLevel> &timeIdx,
                    AdvectionManager &advectionManager) {
        advectionManager.advance(stepSize(), timeIdx, velocityField);
        _timeManager.advance();
    }

    virtual void
    output(const TimeLevelIndex<NumTimeLevel> &timeIdx,
           AdvectionManager &advectionManager) = 0;
}; // AdvectionTestInterface

} // geomtk

#endif // __GEOMTK_AdvectionTestInterface__