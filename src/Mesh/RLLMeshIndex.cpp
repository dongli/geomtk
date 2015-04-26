#include "RLLMeshIndex.h"

namespace geomtk {

RLLMeshIndex::RLLMeshIndex()
: StructuredMeshIndex<RLLMesh, SphereCoord>() {
}

RLLMeshIndex::RLLMeshIndex(uword numDim)
: StructuredMeshIndex<RLLMesh, SphereCoord>(numDim) {
    reset();
}

RLLMeshIndex::~RLLMeshIndex() {
}

void RLLMeshIndex::
reset() {
    StructuredMeshIndex<RLLMesh, SphereCoord>::reset();
    _pole = NOT_POLE;
    inPolarCap = false;
    onPole = false;
    moveOnPole = false;
} // reset

RLLMeshIndex& RLLMeshIndex::
operator=(const RLLMeshIndex &other) {
    StructuredMeshIndex<RLLMesh, SphereCoord>::operator=(other);
    if (this != &other) {
        _pole = other._pole;
        inPolarCap = other.inPolarCap;
        onPole = other.onPole;
        moveOnPole = other.moveOnPole;
    }
    return *this;
} // operator=

void RLLMeshIndex::
locate(const RLLMesh &mesh_, const SphereCoord &x) {
    StructuredMeshIndex<RLLMesh, SphereCoord>::locate(mesh_, x);
    const RLLMesh &mesh = static_cast<const RLLMesh&>(mesh_);
    if (indices[1][GridType::FULL] == 0) {
        _pole = SOUTH_POLE;
        inPolarCap = true;
    } else if (indices[1][GridType::FULL] ==
               static_cast<int>(mesh.numGrid(1, GridType::FULL))-2) {
        _pole = NORTH_POLE;
        inPolarCap = true;
    } else {
        _pole = NOT_POLE;
        inPolarCap = false;
    }
    double r = M_PI_2-fabs(x(1));
    if (r < mesh.poleRadius()) {
        onPole = true;
        _pole = x(1) < 0.0 ? SOUTH_POLE : NORTH_POLE;
    } else {
        onPole = false;
    }
} // locate

void RLLMeshIndex::
print() const {
    StructuredMeshIndex<RLLMesh, SphereCoord>::print();
    cout << "inPolarCap: " << inPolarCap << endl;
    cout << "onPole: " << onPole << endl;
    cout << "pole: ";
    if (_pole == SOUTH_POLE) {
        cout << "SOUTH_POLE" << endl;
    } else if (_pole == NORTH_POLE) {
        cout << "NORTH_POLE" << endl;
    } else if (_pole == NOT_POLE) {
        cout << "NOT_POLE" << endl;
    }
    cout << "moveOnPole: " << moveOnPole << endl;
} // print

} // geomtk
