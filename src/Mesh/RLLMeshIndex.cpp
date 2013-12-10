#include "RLLMeshIndex.h"

namespace geomtk {

RLLMeshIndex::RLLMeshIndex(int numDim) : StructuredMeshIndex(numDim) {
    reset();
}

RLLMeshIndex::~RLLMeshIndex() {
}

void RLLMeshIndex::reset() {
    StructuredMeshIndex::reset();
    pole = NOT_POLE;
    inPolarCap = false;
    onPole = false;
}

RLLMeshIndex& RLLMeshIndex::operator=(const RLLMeshIndex &other) {
    StructuredMeshIndex::operator=(other);
    if (this != &other) {
        pole = other.pole;
        inPolarCap = other.inPolarCap;
        onPole = other.onPole;
    }
    return *this;
}

Pole RLLMeshIndex::getPole() const {
    return pole;
}

bool RLLMeshIndex::isInPolarCap() const {
    return inPolarCap;
}

bool RLLMeshIndex::isOnPole() const {
    return onPole;
}

void RLLMeshIndex::locate(const Mesh &mesh_, const SpaceCoord &x) {
    StructuredMeshIndex::locate(mesh_, x);
    const RLLMesh &mesh = static_cast<const RLLMesh&>(mesh_);
    if (indices[1][CENTER] == 0) {
        pole = SOUTH_POLE;
        inPolarCap = true;
    } else if (indices[1][CENTER] == mesh.getNumGrid(1, CENTER)-2) {
        
        pole = NORTH_POLE;
        inPolarCap = true;
    } else {
        pole = NOT_POLE;
        inPolarCap = false;
    }
    double r = M_PI_2-abs(x(1));
    if (r < mesh.getPoleRadius()) {
        onPole = true;
        pole = x(1) < 0.0 ? SOUTH_POLE : NORTH_POLE;
    } else {
        onPole = false;
    }
}

void RLLMeshIndex::print() const {
    StructuredMeshIndex::print();
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
}

}
