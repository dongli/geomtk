#ifndef __GEOMTK_AdvectionTestInterface__
#define __GEOMTK_AdvectionTestInterface__

#include "AdvectionManagerInterface.h"

namespace geomtk {

template <int NumTimeLevel, class DomainType, class MeshType, 
          template<typename, int> class FieldType,
          class VelocityFieldType, class IOManager>
class AdvectionTestInterface {
public:
    typedef AdvectionManagerInterface<NumTimeLevel, DomainType, MeshType,
                                      FieldType, VelocityFieldType> AdvectionManager;
protected:
    DomainType *_domain;
    MeshType *_mesh;
    TimeManager _timeManager;
    IOManager io;
    uword outputIdx;
    VelocityFieldType velocityField;
    ptime _startTime;
    ptime _endTime;
    double _stepSize;
    int numTracer;
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

    const ptime&
    startTime() const {
        return _startTime;
    }

    const ptime&
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
        _timeManager.advance();
        setVelocityField(timeIdx);
        advectionManager.advance(stepSize(), timeIdx, velocityField);
    }

    virtual void
    advancePhysics(const TimeLevelIndex<NumTimeLevel> &timeIdx,
                   AdvectionManager &advectionManager) {}

    virtual void
    advance(const TimeLevelIndex<NumTimeLevel> &timeIdx,
            AdvectionManager &advectionManager) {
        advancePhysics(timeIdx-1, advectionManager);
        advanceDynamics(timeIdx, advectionManager);
    }

    virtual void
    output(const TimeLevelIndex<NumTimeLevel> &timeIdx,
           const AdvectionManager &advectionManager) = 0;
protected:
    virtual void
    setVelocityField(const TimeLevelIndex<NumTimeLevel> &timeIdx) = 0;
}; // AdvectionTestInterface

} // geomtk

#endif // __GEOMTK_AdvectionTestInterface__