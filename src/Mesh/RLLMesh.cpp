#include "RLLMesh.h"
#include "SphereDomain.h"
#include "RLLMeshIndex.h"

namespace geomtk {

RLLMesh::RLLMesh(Domain &domain) : StructuredMesh(domain) {
    if (dynamic_cast<SphereDomain*>(&domain) == NULL) {
        REPORT_ERROR("The domain should be SphereDomain!");
    }
    poleRadius = 18.0*RAD;
}

RLLMesh::~RLLMesh() {
}

void RLLMesh::setPoleRadius(double radius) {
    poleRadius = radius;
}

double RLLMesh::getPoleRadius() const {
    return poleRadius;
}

void RLLMesh::setGridCoords(int dim, int size, double *full, double *half) {
    StructuredMesh::setGridCoords(dim, size, full, half);
    cosLonFull.resize(fullCoords[0].size());
    sinLonFull.resize(fullCoords[0].size());
    cosLonHalf.resize(halfCoords[0].size());
    sinLonHalf.resize(halfCoords[0].size());
    cosLatFull.resize(fullCoords[1].size());
    sinLatFull.resize(fullCoords[1].size());
    sinLatFull2.resize(fullCoords[1].size());
    cosLatHalf.resize(halfCoords[1].size());
    sinLatHalf.resize(halfCoords[1].size());
    sinLatHalf2.resize(halfCoords[1].size());
    tanLatFull.resize(fullCoords[1].size());
    tanLatHalf.resize(halfCoords[1].size());
    if (dim == 0) {
        for (int i = 0; i < fullCoords[0].size(); ++i) {
            cosLonFull(i) = cos(fullCoords[0](i));
            sinLonFull(i) = sin(fullCoords[0](i));
        }
        for (int i = 0; i < halfCoords[0].size(); ++i) {
            cosLonHalf(i) = cos(halfCoords[0](i));
            sinLonHalf(i) = sin(halfCoords[0](i));
        }
    } else if (dim == 1) {
        for (int j = 0; j < fullCoords[1].size(); ++j) {
            cosLatFull(j) = cos(fullCoords[1](j));
            sinLatFull(j) = sin(fullCoords[1](j));
            sinLatFull2(j) = sinLatFull(j)*sinLatFull(j);
            tanLatFull(j) = tan(fullCoords[1](j));
        }
        for (int j = 0; j < halfCoords[1].size(); ++j) {
            cosLatHalf(j) = cos(halfCoords[1](j));
            sinLatHalf(j) = sin(halfCoords[1](j));
            sinLatHalf2(j) = sinLatHalf(j)*sinLatHalf(j);
            tanLatHalf(j) = tan(halfCoords[1](j));
        }
    }
}

double RLLMesh::getCosLon(StaggerType staggerType, int i) const {
    switch (staggerType) {
        case CENTER:
            return cosLonFull(i+1);
        case EDGE: case VERTEX:
            return cosLonHalf(i+1);
    }
}

double RLLMesh::getSinLon(StaggerType staggerType, int i) const {
    switch (staggerType) {
        case CENTER:
            return sinLonFull(i+1);
        case EDGE: case VERTEX:
            return sinLonHalf(i+1);
    }
}

double RLLMesh::getCosLat(StaggerType staggerType, int j) const {
    switch (staggerType) {
        case CENTER:
            return cosLatFull(j);
        case EDGE: case VERTEX:
            return cosLatHalf(j);
    }
}

double RLLMesh::getSinLat(StaggerType staggerType, int j) const {
    switch (staggerType) {
        case CENTER:
            return sinLatFull(j);
        case EDGE: case VERTEX:
            return sinLatHalf(j);
    }
}

double RLLMesh::getSinLat2(StaggerType staggerType, int j) const {
    switch (staggerType) {
        case CENTER:
            return sinLatFull2(j);
        case EDGE: case VERTEX:
            return sinLatHalf2(j);
    }
}

double RLLMesh::getTanLat(StaggerType staggerType, int j) const {
    switch (staggerType) {
        case CENTER:
            return tanLatFull(j);
        case EDGE: case VERTEX:
            return tanLatHalf(j);
    }
}

void RLLMesh::move(const SphereCoord &x0, double dt, const SphereVelocity &v,
                   const RLLMeshIndex &idx, SphereCoord &x1) const {
    const SphereDomain &domain = static_cast<const SphereDomain&>(*(this->domain));
    if (!idx.isOnPole()) {
        double dlon = dt*v(0)/domain.getRadius()/cos(x0(1));
        double dlat = dt*v(1)/domain.getRadius();
        x1(0) = x0(0)+dlon;
        x1(1) = x0(1)+dlat;
        if (x1(1) > M_PI_2) {
            x1(0) = M_PI+x0(0)-dlon;
            x1(1) = M_PI-x0(1)-dlat;
        }
        if (x1(1) < -M_PI_2) {
            x1(0) =  M_PI+x0(0)-dlon;
            x1(1) = -M_PI-x0(1)-dlat;
        }
        if (x1(0) < 0.0) {
            x1(0) = PI2+fmod(x1(0), PI2);
        } else if (x1(0) > PI2) {
            x1(0) = fmod(x1(0), PI2);
        }
    } else {
        x1(0) = x0(0);
        x1(1) = x0(1);
        x1.transformToPS(domain);
        x1[0] += dt*v[0];
        x1[1] += dt*v[1];
        x1.transformFromPS(domain);
    }
    if (domain.getNumDim() == 3) {
        double dlev = dt*v(2);
        x1(2) = x0(2)+dlev;
        assert(x1(2) >= domain.getAxisStart(2) && x1(2) <= domain.getAxisEnd(2));
    }
}

}
