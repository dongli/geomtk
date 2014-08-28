#include "RLLMeshIndex.h"

namespace geomtk {

RLLMeshIndex::RLLMeshIndex(int numDim) : StructuredMeshIndex<RLLMesh, SphereCoord>(numDim) {
    reset();
}

RLLMeshIndex::~RLLMeshIndex() {
}

void RLLMeshIndex::reset() {
    StructuredMeshIndex<RLLMesh, SphereCoord>::reset();
    pole = NOT_POLE;
    inPolarCap = false;
    onPole = false;
    moveOnPole = false;
}

RLLMeshIndex& RLLMeshIndex::operator=(const RLLMeshIndex &other) {
    StructuredMeshIndex<RLLMesh, SphereCoord>::operator=(other);
    if (this != &other) {
        pole = other.pole;
        inPolarCap = other.inPolarCap;
        onPole = other.onPole;
        moveOnPole = other.moveOnPole;
    }
    return *this;
}

void RLLMeshIndex::locate(const RLLMesh &mesh_, const SphereCoord &x) {
    StructuredMeshIndex<RLLMesh, SphereCoord>::locate(mesh_, x);
    const RLLMesh &mesh = static_cast<const RLLMesh&>(mesh_);
    if (indices[1][GridType::FULL] == 0) {
        pole = SOUTH_POLE;
        inPolarCap = true;
    } else if (indices[1][GridType::FULL] ==
               mesh.getNumGrid(1, GridType::FULL)-2) {
        pole = NORTH_POLE;
        inPolarCap = true;
    } else {
        pole = NOT_POLE;
        inPolarCap = false;
    }
    double r = M_PI_2-fabs(x(1));
    if (r < mesh.getPoleRadius()) {
        onPole = true;
        pole = x(1) < 0.0 ? SOUTH_POLE : NORTH_POLE;
    } else {
        onPole = false;
    }
}

void RLLMeshIndex::print() const {
    StructuredMeshIndex<RLLMesh, SphereCoord>::print();
    cout << "inPolarCap: " << inPolarCap << endl;
    cout << "onPole: " << onPole << endl;
    cout << "pole: ";
    if (pole == SOUTH_POLE) {
        cout << "SOUTH_POLE" << endl;
    } else if (pole == NORTH_POLE) {
        cout << "NORTH_POLE" << endl;
    } else if (pole == NOT_POLE) {
        cout << "NOT_POLE" << endl;
    }
    cout << "moveOnPole: " << moveOnPole << endl;
}

} // geomtk
