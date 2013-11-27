#include "RLLScalarField.h"

RLLScalarField::RLLScalarField(Mesh &mesh) : Field(mesh) {
        if (dynamic_cast<RLLMesh*>(&mesh) == NULL) {
            REPORT_ERROR("Mesh should be RLLMesh!")
        }
    staggerTypes = new StaggerType[mesh.getDomain().getNumDim()];
}

RLLScalarField::~RLLScalarField() {
    delete [] staggerTypes;
}

void RLLScalarField::applyBndCond() {
    // zonal periodic boundary condition
    int nx = data.get(0).n_rows;
    int ny = data.get(0).n_cols;
    int nz = data.get(0).n_slices;
    for (int l = 0; l < data.getNumLevel(); ++l) {
        for (int k = 0; k < nz; ++k) {
            for (int j = 0; j < ny; ++j) {
                data.get(l)(0, j, k) = data.get(l)(nx-2, j, k);
                data.get(l)(nx-1, j, k) = data.get(l)(1, j, k);
            }
        }
    }
}

void RLLScalarField::create(StaggerType lonStaggerType,
                            StaggerType latStaggerType) {
    if (mesh->getDomain().getNumDim() != 2) {
        REPORT_ERROR("The level stagger type should be specified!")
    }
    staggerTypes[0] = lonStaggerType;
    staggerTypes[1] = latStaggerType;
    const RLLMesh &mesh = dynamic_cast<const RLLMesh&>(*(this->mesh));
    for (int i = 0; i < data.getNumLevel(); ++i) {
        data.get(i).reshape(mesh.getNumGrid(0, lonStaggerType, true),
                            mesh.getNumGrid(1, latStaggerType), 1);
    }
}

void RLLScalarField::create(StaggerType lonStaggerType,
                            StaggerType latStaggerType,
                            StaggerType levStaggerType) {
    if (mesh->getDomain().getNumDim() != 3) {
        REPORT_ERROR("Wrong create version is invoked!")
    }
    staggerTypes[0] = lonStaggerType;
    staggerTypes[1] = latStaggerType;
    staggerTypes[2] = levStaggerType;
    const RLLMesh &mesh = dynamic_cast<const RLLMesh&>(*(this->mesh));
    for (int i = 0; i < data.getNumLevel(); ++i) {
        data.get(i).reshape(mesh.getNumGrid(0, lonStaggerType, true),
                            mesh.getNumGrid(1, latStaggerType),
                            mesh.getNumGrid(2, levStaggerType));
    }
}

double RLLScalarField::operator()(int timeLevel, int i, int j, int k) const {
    // NOTE: Increase i by 1 to handle virtual boundary grids.
    return data.get(timeLevel)(i+1, j, k);
}

double& RLLScalarField::operator()(int timeLevel, int i, int j, int k) {
    // NOTE: Increase i by 1 to handle virtual boundary grids.
    return data.get(timeLevel)(i+1, j, k);
}
