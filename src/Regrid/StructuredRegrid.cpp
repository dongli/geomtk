#include "StructuredRegrid.h"
#include "StructuredMeshIndex.h"
#include "StructuredScalarField.h"

StructuredRegrid::StructuredRegrid(Mesh &mesh) : Regrid(mesh) {

}

StructuredRegrid::~StructuredRegrid() {

}

void StructuredRegrid::run(const Field &f, const SpaceCoord &x, double &y) {
    if (dynamic_cast<const StructuredScalarField*>(&f) == NULL) {
        REPORT_ERROR("Mesh should comply StructuredScalarField!");
    }
    StructuredMeshIndex idx(*mesh);
    idx.locate(x);
}
