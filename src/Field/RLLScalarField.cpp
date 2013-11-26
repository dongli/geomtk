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

void RLLScalarField::create(StaggerType lonStaggerType,
                            StaggerType latStaggerType) {
    if (mesh->getDomain().getNumDim() != 2) {
        REPORT_ERROR("The level stagger type should be specified!")
    }
    staggerTypes[0] = lonStaggerType;
    staggerTypes[1] = latStaggerType;
    const RLLMesh &mesh = dynamic_cast<const RLLMesh&>(*(this->mesh));
    for (int i = 0; i < data.getNumLevel(); ++i) {
        data.get(i).reshape(mesh.getNumGrid(0, lonStaggerType),
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
        data.get(i).reshape(mesh.getNumGrid(0, lonStaggerType),
                            mesh.getNumGrid(1, latStaggerType),
                            mesh.getNumGrid(2, levStaggerType));
    }
}
