#include "StructuredScalarField.h"
#include "StructuredMesh.h"

StructuredScalarField::StructuredScalarField(Mesh &mesh) : Field(mesh) {
    if (dynamic_cast<StructuredMesh*>(&mesh) == NULL) {
        REPORT_ERROR("Mesh should comply StructuredMesh!");
    }
    staggerTypes = new StaggerType[mesh.getDomain().getNumDim()];
}

StructuredScalarField::~StructuredScalarField() {
    delete [] staggerTypes;
}

void StructuredScalarField::applyBndCond(int timeLevel) {
    int nx = data.get(0).n_rows;
    int ny = data.get(0).n_cols;
    int nz = data.get(0).n_slices;
    const StructuredMesh &mesh = static_cast<const StructuredMesh&>(*(this->mesh));
    if (mesh.getDomain().getAxisStartBndType(0) == PERIODIC) {
        for (int k = 0; k < nz; ++k) {
            for (int j = 0; j < ny; ++j) {
                data.get(timeLevel)(0, j, k) = data.get(timeLevel)(nx-2, j, k);
                data.get(timeLevel)(nx-1, j, k) = data.get(timeLevel)(1, j, k);
            }
        }
    } else {
        REPORT_ERROR("Under construction!");
    }
}

void StructuredScalarField::create(StaggerType xStaggerType,
                                   StaggerType yStaggerType) {
    if (mesh->getDomain().getNumDim() != 2) {
        REPORT_ERROR("The level stagger type should be specified!");
    }
    staggerTypes[0] = xStaggerType;
    staggerTypes[1] = yStaggerType;
    const StructuredMesh &mesh = dynamic_cast<const StructuredMesh&>(*(this->mesh));
    for (int i = 0; i < data.getNumLevel(); ++i) {
        data.get(i).reshape(mesh.getNumGrid(0, xStaggerType, true),
                            mesh.getNumGrid(1, yStaggerType), 1);
    }
}

void StructuredScalarField::create(StaggerType xStaggerType,
                                   StaggerType yStaggerType,
                                   StaggerType zStaggerType) {
    if (mesh->getDomain().getNumDim() != 3) {
        REPORT_ERROR("Wrong create version is invoked!");
    }
    staggerTypes[0] = xStaggerType;
    staggerTypes[1] = yStaggerType;
    staggerTypes[2] = zStaggerType;
    const StructuredMesh &mesh = dynamic_cast<const StructuredMesh&>(*(this->mesh));
    for (int i = 0; i < data.getNumLevel(); ++i) {
        data.get(i).reshape(mesh.getNumGrid(0, xStaggerType, true),
                            mesh.getNumGrid(1, yStaggerType),
                            mesh.getNumGrid(2, zStaggerType));
    }
}

double StructuredScalarField::operator()(int timeLevel, int i, int j, int k) const {
    // NOTE: Handle virtual boundary grids.
    int I, J;
    if (mesh->getDomain().getAxisStartBndType(0) == PERIODIC) {
        I = i+1;
    } else {
        I = i;
    }
    if (mesh->getDomain().getAxisStartBndType(1) == PERIODIC) {
        J = j+1;
    } else {
        J = j;
    }
    return data.get(timeLevel)(I, J, k);
}

double& StructuredScalarField::operator()(int timeLevel, int i, int j, int k) {
    // NOTE: Handle virtual boundary grids.
    int I, J;
    if (mesh->getDomain().getAxisStartBndType(0) == PERIODIC) {
        I = i+1;
    } else {
        I = i;
    }
    if (mesh->getDomain().getAxisStartBndType(1) == PERIODIC) {
        J = j+1;
    } else {
        J = j;
    }
    return data.get(timeLevel)(I, J, k);
}
