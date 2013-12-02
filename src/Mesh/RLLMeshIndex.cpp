#include "RLLMeshIndex.h"

RLLMeshIndex::RLLMeshIndex(Mesh &mesh) : StructuredMeshIndex(mesh) {
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

Pole RLLMeshIndex::getPole() const {
    return pole;
}

bool RLLMeshIndex::isInPolarCap() const {
    return inPolarCap;
}

bool RLLMeshIndex::isOnPole() const {
    return onPole;
}

void RLLMeshIndex::locate(const SpaceCoord &x) {
    StructuredMeshIndex::locate(x);
    const RLLMesh &mesh = static_cast<const RLLMesh&>(*(this->mesh));
    if (indices[1][CENTER] == 0) {
        pole = SOUTH_POLE;
        inPolarCap = true;
    } else if (indices[1][CENTER] == mesh.fullCoords[1].size()-2) {
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
