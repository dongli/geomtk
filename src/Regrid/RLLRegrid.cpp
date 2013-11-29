#include "RLLRegrid.h"
#include "RLLMesh.h"

RLLRegrid::RLLRegrid(Mesh &mesh) : Regrid(mesh) {
}

RLLRegrid::~RLLRegrid() {
}

void RLLRegrid::run(const Field &f, const SpaceCoord &x, double &y) {
}
