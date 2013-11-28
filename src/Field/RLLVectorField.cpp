#include "RLLVectorField.h"

RLLVectorField::RLLVectorField(Mesh &mesh) : Field(mesh) {
    if (dynamic_cast<RLLMesh*>(&mesh) == NULL) {
        REPORT_ERROR("Mesh should be RLLMesh!")
    }
    staggerTypes = new StaggerType*[mesh.getDomain().getNumDim()];
    for (int i = 0; i < mesh.getDomain().getNumDim(); ++i) {
        staggerTypes[i] = new StaggerType[mesh.getDomain().getNumDim()];
    }
}

RLLVectorField::~RLLVectorField() {
    for (int i = 0; i < mesh->getDomain().getNumDim(); ++i) {
        delete [] staggerTypes[i];
    }
    delete staggerTypes;
}

void RLLVectorField::applyBndCond(int timeLevel) {
    // zonal periodic boundary condition
    for (int l = 0; l < mesh->getDomain().getNumDim(); ++l) {
        int nx = data[l].get(0).n_rows;
        int ny = data[l].get(0).n_cols;
        int nz = data[l].get(0).n_slices;
        for (int k = 0; k < nz; ++k) {
            for (int j = 0; j < ny; ++j) {
                data[l].get(timeLevel)(0, j, k) = data[l].get(timeLevel)(nx-2, j, k);
                data[l].get(timeLevel)(nx-1, j, k) = data[l].get(timeLevel)(1, j, k);
            }
        }
    }
}

void RLLVectorField::create(StaggerType uLonStaggerType,
                            StaggerType uLatStaggerType,
                            StaggerType vLonStaggerType,
                            StaggerType vLatStaggerType) {
    staggerTypes[0][0] = uLonStaggerType;
    staggerTypes[0][1] = uLatStaggerType;
    staggerTypes[1][0] = vLonStaggerType;
    staggerTypes[1][1] = vLatStaggerType;
    const RLLMesh &mesh = dynamic_cast<const RLLMesh&>(*(this->mesh));
    for (int l = 0; l < data[0].getNumLevel(); ++l) {
        for (int m = 0; m < mesh.getDomain().getNumDim(); ++m) {
            data[m].get(l).reshape(mesh.getNumGrid(0, staggerTypes[m][0], true),
                                   mesh.getNumGrid(1, staggerTypes[m][1]), 1);
        }
    }
}

void RLLVectorField::create(StaggerType uLonStaggerType,
                            StaggerType uLatStaggerType,
                            StaggerType uLevStaggerType,
                            StaggerType vLonStaggerType,
                            StaggerType vLatStaggerType,
                            StaggerType vLevStaggerType,
                            StaggerType wLonStaggerType,
                            StaggerType wLatStaggerType,
                            StaggerType wLevStaggerType) {
    if (mesh->getDomain().getNumDim() != 3) {
        REPORT_ERROR("The domain is not 3D!")
    }
    staggerTypes[0][0] = uLonStaggerType;
    staggerTypes[0][1] = uLatStaggerType;
    staggerTypes[0][2] = uLevStaggerType;
    staggerTypes[1][0] = vLonStaggerType;
    staggerTypes[1][1] = vLatStaggerType;
    staggerTypes[1][2] = vLevStaggerType;
    staggerTypes[2][0] = wLonStaggerType;
    staggerTypes[2][1] = wLatStaggerType;
    staggerTypes[2][2] = wLevStaggerType;
    const RLLMesh &mesh = dynamic_cast<const RLLMesh&>(*(this->mesh));
    for (int l = 0; l < data[0].getNumLevel(); ++l) {
        for (int m = 0; m < mesh.getDomain().getNumDim(); ++m) {
            data[m].get(l).reshape(mesh.getNumGrid(0, staggerTypes[m][0], true),
                                   mesh.getNumGrid(1, staggerTypes[m][1]),
                                   mesh.getNumGrid(2, staggerTypes[m][2]));
        }
    }
}

double RLLVectorField::operator()(int timeLevel, int dim, int i, int j, int k) const {
    return data[dim].get(timeLevel)(i+1, j, k);
}

double& RLLVectorField::operator()(int timeLevel, int dim, int i, int j, int k) {
    return data[dim].get(timeLevel)(i+1, j, k);
}
