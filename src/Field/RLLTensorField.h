#ifndef __Geomtk_RLLTensorField__
#define __Geomtk_RLLTensorField__

#include "StructuredTensorField.h"
#include "RLLVectorField.h"

namespace geomtk {

class RLLTensorField : public StructuredTensorField {
protected:
    
public:
    RLLTensorField(const Mesh &mesh, bool hasHalfLevel = false);
    RLLTensorField(const string &name, const string &units,
                   const string &longName, const Mesh &mesh,
                   bool hasHalfLevel = false);
    virtual ~RLLTensorField();

    virtual void applyBndCond(int timeLevel, bool updateHalfLevel = false);

    virtual void create();

    void calcFromVector(const RLLVectorField &vector, int timeLevel);
};

}

#endif