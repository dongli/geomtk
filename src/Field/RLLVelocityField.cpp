#include "RLLVelocityField.h"

PolarRing::PolarRing() {
}

PolarRing::~PolarRing() {
}

void PolarRing::create(Mesh &mesh) {
    this->mesh = static_cast<RLLMesh*>(&mesh);
    for (int m = 0; m < this->mesh->getDomain().getNumDim(); ++m) {
        for (int l = 0; l < data[0].getNumLevel(); ++l) {
            data[m].get(l).reshape(this->mesh->getNumGrid(0, CENTER, true),
                                   this->mesh->getNumGrid(2, CENTER));
            tranData[m].get(l).reshape(this->mesh->getNumGrid(0, CENTER, true),
                                       this->mesh->getNumGrid(2, CENTER));
        }
    }
}

void PolarRing::update(int timeLevel, Pole pole, TimeLevels<cube, 2> *data) {
    // ring variable is at A-grids
    int nx = mesh->getNumGrid(0, CENTER, true);
    int j = pole == SOUTH_POLE ? 1 : mesh->getNumGrid(1, CENTER)-2; // off the Pole
    for (int k = 0; k < mesh->getNumGrid(2, CENTER); ++k) {
        for (int i = 1; i < mesh->getNumGrid(0, CENTER, true)-1; ++i) {
            this->data[0].get(timeLevel)(i, k) =
                (data[0].get(timeLevel)(i-1, j, k)+
                 data[0].get(timeLevel)(i,   j, k))*0.5;
        }
        this->data[0].get(timeLevel)(0, k) = this->data[0].get(timeLevel)(nx-2, k);
        this->data[0].get(timeLevel)(nx-1, k) = this->data[0].get(timeLevel)(1, k);
    }
    int j1 = pole == SOUTH_POLE ? 0 : mesh->getNumGrid(1, EDGE)-1;
    int j2 = pole == SOUTH_POLE ? j1+1 : j1-1;
    for (int k = 0; k < mesh->getNumGrid(2, CENTER); ++k) {
        for (int i = 0; i < mesh->getNumGrid(0, CENTER, true); ++i) {
            this->data[1].get(timeLevel)(i, k) =
                (data[1].get(timeLevel)(i, j1, k)+
                 data[1].get(timeLevel)(i, j2, k))*0.5;
        }
    }
    if (mesh->getDomain().getNumDim() == 3) {
        for (int k = 0; k < mesh->getNumGrid(2, CENTER); ++k) {
            for (int i = 0; i < mesh->getNumGrid(0, CENTER, true); ++i) {
                this->data[2].get(timeLevel)(i, k) =
                    (data[2].get(timeLevel)(i, j, k)+
                     data[2].get(timeLevel)(i, j, k+1))*0.5;
            }
        }
    }
    // transform velocity
    double sign = pole == SOUTH_POLE ? -1.0 : 1.0;
    double cosLon, sinLon, sinLat, sinLat2;
    sinLat = mesh->getSinLat(CENTER, j);
    sinLat2 = mesh->getSinLat2(CENTER, j);
    for (int k = 0; k < mesh->getNumGrid(2, CENTER); ++k) {
        for (int i = 0; i < mesh->getNumGrid(0, CENTER, true); ++i) {
            cosLon = mesh->getCosLon(CENTER, i);
            sinLon = mesh->getSinLon(CENTER, i);
            double u = this->data[0].get(timeLevel)(i, k);
            double v = this->data[1].get(timeLevel)(i, k);
            tranData[0].get(timeLevel)(i, k) = sign*(-sinLon/sinLat*u-cosLon/sinLat2*v);
            tranData[1].get(timeLevel)(i, k) = sign*( cosLon/sinLat*u-sinLon/sinLat2*v);
        }
    }
}

// -----------------------------------------------------------------------------

RLLVelocityField::RLLVelocityField(Mesh &mesh) : RLLVectorField(mesh) {
    rings = new PolarRing[2];
}

RLLVelocityField::~RLLVelocityField() {
    delete [] rings;
}

void RLLVelocityField::applyBndCond(int timeLevel) {
	RLLVectorField::applyBndCond(timeLevel);
    rings[0].update(timeLevel, SOUTH_POLE, data);
    rings[1].update(timeLevel, NORTH_POLE, data);
}

void RLLVelocityField::create(StaggerType uLonStaggerType,
                              StaggerType uLatStaggerType,
                              StaggerType vLonStaggerType,
                              StaggerType vLatStaggerType) {
    RLLVectorField::create(uLonStaggerType, uLatStaggerType,
    					   vLonStaggerType, vLatStaggerType);
    rings[0].create(*mesh); rings[1].create(*mesh);
}

void RLLVelocityField::create(StaggerType uLonStaggerType,
                              StaggerType uLatStaggerType,
                              StaggerType uLevStaggerType,
                              StaggerType vLonStaggerType,
                              StaggerType vLatStaggerType,
                              StaggerType vLevStaggerType,
                              StaggerType wLonStaggerType,
                              StaggerType wLatStaggerType,
                              StaggerType wLevStaggerType) {
    RLLVectorField::create(uLonStaggerType, uLatStaggerType, uLevStaggerType,
    					   vLonStaggerType, vLatStaggerType, vLevStaggerType,
    					   wLonStaggerType, wLatStaggerType, wLevStaggerType);
    rings[0].create(*mesh); rings[1].create(*mesh);
}
