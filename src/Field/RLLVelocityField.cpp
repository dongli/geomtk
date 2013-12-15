#include "RLLVelocityField.h"

namespace geomtk {

PolarRing::PolarRing() {
    vr = NULL;
}

PolarRing::~PolarRing() {
    if (vr != NULL) {
        for (int i = 0; i < mesh->getNumGrid(0, CENTER, true); ++i) {
            for (int k = 0; k < mesh->getNumGrid(2, CENTER); ++k) {
                delete vr[i][k];
            }
            delete [] vr[i];
        }
        delete [] vr;
    }
}

void PolarRing::create(const Mesh &mesh, bool hasHalfLevel) {
    assert(mesh.isSet());
    this->mesh = static_cast<const RLLMesh*>(&mesh);
    vr = new TimeLevels<SphereVelocity, 2>**[this->mesh->getNumGrid(0, CENTER, true)];
    for (int i = 0; i < this->mesh->getNumGrid(0, CENTER, true); ++i) {
        vr[i] = new TimeLevels<SphereVelocity, 2>*[this->mesh->getNumGrid(2, CENTER)];
        for (int k = 0; k < this->mesh->getNumGrid(2, CENTER); ++k) {
            vr[i][k] = new TimeLevels<SphereVelocity, 2>(hasHalfLevel);
            for (int l = 0; l < vr[i][k]->getNumLevel(INCLUDE_HALF_LEVEL); ++l) {
                vr[i][k]->getLevel(l).setNumDim(mesh.getDomain().getNumDim());
            }
        }
    }
}

void PolarRing::update(int timeLevel, Pole pole, TimeLevels<cube, 2> **data,
                       bool updateHalfLevel) {
    // ring variable is at A-grids
    int nx = mesh->getNumGrid(0, CENTER, true);
    int j = pole == SOUTH_POLE ? 1 : mesh->getNumGrid(1, CENTER)-2; // off the Pole
    for (int i = 1; i < mesh->getNumGrid(0, CENTER, true)-1; ++i) {
        for (int k = 0; k < mesh->getNumGrid(2, CENTER); ++k) {
            vr[i][k]->getLevel(timeLevel)(0) =
                (data[0]->getLevel(timeLevel)(i-1, j, k)+
                 data[0]->getLevel(timeLevel)(  i, j, k))*0.5;
        }
    }
    // periodic boundary condition
    for (int k = 0; k < mesh->getNumGrid(2, CENTER); ++k) {
        vr[0][k]->getLevel(timeLevel)(0) = vr[nx-2][k]->getLevel(timeLevel)(0);
        vr[nx-1][k]->getLevel(timeLevel)(0) = vr[1][k]->getLevel(timeLevel)(0);
    }
    j = pole == SOUTH_POLE ? 0 : mesh->getNumGrid(1, EDGE)-2;
    for (int i = 0; i < mesh->getNumGrid(0, CENTER, true); ++i) {
        for (int k = 0; k < mesh->getNumGrid(2, CENTER); ++k) {
            vr[i][k]->getLevel(timeLevel)(1) =
                (data[1]->getLevel(timeLevel)(i, j,   k)+
                 data[1]->getLevel(timeLevel)(i, j+1, k))*0.5;
        }
    }
    if (mesh->getDomain().getNumDim() == 3) {
        for (int i = 0; i < mesh->getNumGrid(0, CENTER, true); ++i) {
            for (int k = 0; k < mesh->getNumGrid(2, CENTER); ++k) {
                vr[i][k]->getLevel(timeLevel)(2) =
                    (data[2]->getLevel(timeLevel)(i, j, k)+
                     data[2]->getLevel(timeLevel)(i, j, k+1))*0.5;
            }
        }
    }
    // -------------------------------------------------------------------------
    // transform velocity
    double cosLon, sinLon, sinLat, sinLat2;
    j = pole == SOUTH_POLE ? 1 : mesh->getNumGrid(1, CENTER)-2;
    sinLat = mesh->getSinLat(CENTER, j);
    sinLat2 = mesh->getSinLat2(CENTER, j);
    for (int i = 0; i < mesh->getNumGrid(0, CENTER); ++i) {
        cosLon = mesh->getCosLon(CENTER, i);
        sinLon = mesh->getSinLon(CENTER, i);
        for (int k = 0; k < mesh->getNumGrid(2, CENTER); ++k) {
            vr[i+1][k]->getLevel(timeLevel)
                .transformToPS(sinLat, sinLat2, sinLon, cosLon);
        }
    }
    // periodic boundary condition
    for (int k = 0; k < mesh->getNumGrid(2, CENTER); ++k) {
        for (int m = 0; m < mesh->getDomain().getNumDim(); ++m) {
            vr[0][k]->getLevel(timeLevel)[m] = vr[nx-2][k]->getLevel(timeLevel)[m];
            vr[nx-1][k]->getLevel(timeLevel)[m] = vr[1][k]->getLevel(timeLevel)[m];
        }
    }
    // -------------------------------------------------------------------------
    // update half level
    if (updateHalfLevel && vr[0][0]->hasHalfLevel()) {
        for (int i = -1; i < mesh->getNumGrid(0, CENTER)+1; ++i) {
            for (int k = 0; k < mesh->getNumGrid(2, CENTER); ++k) {
                vr[i+1][k]->updateHalfLevel();
            }
        }
    }
}

double PolarRing::getOriginalData(int timeLevel, int dim, int i, int k) const {
    return vr[i+1][k]->getLevel(timeLevel)(dim);
}

double PolarRing::getTransformedData(int timeLevel, int dim, int i, int k) const {
    return vr[i+1][k]->getLevel(timeLevel)[dim];
}

void PolarRing::print() const {
    int l = 0;
    cout << "---------------------------------------------------------------" << endl;
    cout << "Reorganized original velocity:" << endl;
    cout << "dim";
    for (int i = 0; i < mesh->getNumGrid(0, CENTER); ++i) {
        cout << setw(10) << i;
    }
    cout << endl;
    for (int m = 0; m < mesh->getDomain().getNumDim(); ++m) {
        cout << setw(2) << m << ":";
        for (int i = 0; i < mesh->getNumGrid(0, CENTER); ++i) {
            for (int k = 0; k < mesh->getNumGrid(2, CENTER); ++k) {
                cout << setw(10) << setprecision(2) << getOriginalData(l, m, i, k);
            }
        }
        cout << endl;
    }
    cout << "---------------------------------------------------------------" << endl;
    cout << "Transformed velocity:" << endl;
    cout << "dim";
    for (int i = 0; i < mesh->getNumGrid(0, CENTER); ++i) {
        cout << setw(10) << i;
    }
    cout << endl;
    for (int m = 0; m < mesh->getDomain().getNumDim(); ++m) {
        cout << setw(2) << m << ":";
        for (int i = 0; i < mesh->getNumGrid(0, CENTER); ++i) {
            for (int k = 0; k < mesh->getNumGrid(2, CENTER); ++k) {
                cout << setw(10) << setprecision(2) << getTransformedData(l, m, i, k);
            }
        }
        cout << endl;
    }
}

// -----------------------------------------------------------------------------

RLLVelocityField::RLLVelocityField(const Mesh &mesh, bool hasHalfLevel)
    : RLLVectorField(mesh, hasHalfLevel) {
    this->hasHalfLevel = hasHalfLevel;
}
    
RLLVelocityField::RLLVelocityField(const string &name, const string &units,
                                   const string &longName, const Mesh &mesh,
                                   bool hasHalfLevel)
    : RLLVectorField(name, units, longName, mesh, hasHalfLevel) {
    this->hasHalfLevel = hasHalfLevel;
}

RLLVelocityField::~RLLVelocityField() {
}

void RLLVelocityField::applyBndCond(int timeLevel, bool updateHalfLevel) {
    RLLVectorField::applyBndCond(timeLevel, updateHalfLevel);
    rings[0].update(timeLevel, SOUTH_POLE, data, updateHalfLevel);
    rings[1].update(timeLevel, NORTH_POLE, data, updateHalfLevel);
}
    
void RLLVelocityField::create(ArakawaGrid gridType) {
    RLLVectorField::create(gridType);
    rings[0].create(*mesh, hasHalfLevel);
    rings[1].create(*mesh, hasHalfLevel);
}

void RLLVelocityField::create(StaggerType uLonStaggerType,
                              StaggerType uLatStaggerType,
                              StaggerType vLonStaggerType,
                              StaggerType vLatStaggerType) {
    RLLVectorField::create(uLonStaggerType, uLatStaggerType,
                           vLonStaggerType, vLatStaggerType);
    rings[0].create(*mesh, hasHalfLevel);
    rings[1].create(*mesh, hasHalfLevel);
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
    rings[0].create(*mesh, hasHalfLevel);
    rings[1].create(*mesh, hasHalfLevel);
}

const PolarRing& RLLVelocityField::getPolarRing(Pole pole) const {
    return rings[pole];
}

}
