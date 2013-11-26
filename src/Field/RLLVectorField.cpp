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

void RLLVectorField::create(StaggerType uLonStaggerType,
                            StaggerType uLatStaggerType,
                            StaggerType vLonStaggerType,
                            StaggerType vLatStaggerType) {
    if (mesh->getDomain().getNumDim() != 2) {
        REPORT_ERROR("The level stagger type should be specified!")
    }
    staggerTypes[0][0] = uLonStaggerType;
    staggerTypes[0][1] = uLatStaggerType;
    staggerTypes[1][0] = vLonStaggerType;
    staggerTypes[1][1] = vLatStaggerType;
    const RLLMesh &mesh = dynamic_cast<const RLLMesh&>(*(this->mesh));
    for (int i = 0; i < data[0].getNumLevel(); ++i) {
        data[0].get(i).reshape(mesh.getNumGrid(0, uLonStaggerType),
                               mesh.getNumGrid(1, uLatStaggerType), 1);
        data[1].get(i).reshape(mesh.getNumGrid(0, vLonStaggerType),
                               mesh.getNumGrid(1, vLatStaggerType), 1);
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
    if (mesh->getDomain().getNumDim() != 2) {
        REPORT_ERROR("The level stagger type should be specified!")
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
    for (int i = 0; i < data[0].getNumLevel(); ++i) {
        data[0].get(i).reshape(mesh.getNumGrid(0, uLonStaggerType),
                               mesh.getNumGrid(1, uLatStaggerType),
                               mesh.getNumGrid(2, uLevStaggerType));
        data[1].get(i).reshape(mesh.getNumGrid(0, vLonStaggerType),
                               mesh.getNumGrid(1, vLatStaggerType),
                               mesh.getNumGrid(2, vLevStaggerType));
        data[2].get(i).reshape(mesh.getNumGrid(0, wLonStaggerType),
                               mesh.getNumGrid(1, wLatStaggerType),
                               mesh.getNumGrid(2, wLevStaggerType));
    }
}
