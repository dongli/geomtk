#include "RLLMesh.h"
#include "RLLMeshIndex.h"
#include "IOManager.h"
#include "SphereDomain.h"

namespace geomtk {

RLLMesh::RLLMesh(SphereDomain &domain) : StructuredMesh<SphereDomain, SphereCoord>(domain) {
    _type = RLL_MESH;
    _poleRadius = 18.0*RAD;
}

RLLMesh::~RLLMesh() {
}

void RLLMesh::
init(const string &filePath) {
    IOManager<RLLDataFile> io;
    int fileIdx = io.addInputFile(*this, filePath);
    io.open(fileIdx);
    io.file(fileIdx).inputMesh();
    io.close(fileIdx);
    StructuredMesh<SphereDomain, SphereCoord>::init(filePath);
    set = true;
} // init

void RLLMesh::
init(const string &filePathH, const string &filePathV) {
    IOManager<RLLDataFile> io;
    int fileIdx = io.addInputFile(*this, filePathH);
    io.open(fileIdx);
    io.file(fileIdx).inputHorizontalMesh();
    io.close(fileIdx);
    fileIdx = io.addInputFile(*this, filePathV);
    io.open(fileIdx);
    io.file(fileIdx).inputVerticalMesh();
    io.close(fileIdx);
    StructuredMesh<SphereDomain, SphereCoord>::init(filePathH, filePathV);
    set = true;
} // init

void RLLMesh::
init(uword nx, uword ny, uword nz) {
    StructuredMesh<SphereDomain, SphereCoord>::init(nx, ny, nz);
} // init

void RLLMesh::
setGridCoordComps(uword axisIdx, uword size, const vec &full, const vec &half) {
    StructuredMesh<SphereDomain, SphereCoord>::
    setGridCoordComps(axisIdx, size, full, half);
    if (axisIdx == 0) {
        cosLonFull.set_size(fullCoords[0].size());
        sinLonFull.set_size(fullCoords[0].size());
        cosLonHalf.set_size(halfCoords[0].size());
        sinLonHalf.set_size(halfCoords[0].size());
        for (uword i = 0; i < fullCoords[0].size(); ++i) {
            cosLonFull(i) = cos(fullCoords[0](i));
            sinLonFull(i) = sin(fullCoords[0](i));
        }
        for (uword i = 0; i < halfCoords[0].size(); ++i) {
            cosLonHalf(i) = cos(halfCoords[0](i));
            sinLonHalf(i) = sin(halfCoords[0](i));
        }
    } else if (axisIdx == 1) {
        cosLatFull.set_size(fullCoords[1].size());
        sinLatFull.set_size(fullCoords[1].size());
        sinLatFull2.set_size(fullCoords[1].size());
        cosLatHalf.set_size(halfCoords[1].size());
        sinLatHalf.set_size(halfCoords[1].size());
        sinLatHalf2.set_size(halfCoords[1].size());
        tanLatFull.set_size(fullCoords[1].size());
        tanLatHalf.set_size(halfCoords[1].size());
        for (uword j = 0; j < fullCoords[1].size(); ++j) {
            cosLatFull(j) = cos(fullCoords[1](j));
            sinLatFull(j) = sin(fullCoords[1](j));
            sinLatFull2(j) = sinLatFull(j)*sinLatFull(j);
            tanLatFull(j) = tan(fullCoords[1](j));
        }
        for (uword j = 0; j < halfCoords[1].size(); ++j) {
            cosLatHalf(j) = cos(halfCoords[1](j));
            sinLatHalf(j) = sin(halfCoords[1](j));
            sinLatHalf2(j) = sinLatHalf(j)*sinLatHalf(j);
            tanLatHalf(j) = tan(halfCoords[1](j));
        }
    }
    for (uword m = 0; m < this->domain().numDim(); ++m) {
        if (fullCoords[m].size() == 0) {
            return;
        }
    }
    setCellVolumes();
} // setGridCoords

void RLLMesh::
setGridCoordComps(uword axisIdx, uword size, const vec &full) {
    StructuredMesh<SphereDomain, SphereCoord>::
    setGridCoordComps(axisIdx, size, full);
    if (axisIdx == 0) {
        cosLonFull.set_size(fullCoords[0].size());
        sinLonFull.set_size(fullCoords[0].size());
        cosLonHalf.set_size(halfCoords[0].size());
        sinLonHalf.set_size(halfCoords[0].size());
        for (uword i = 0; i < fullCoords[0].size(); ++i) {
            cosLonFull(i) = cos(fullCoords[0](i));
            sinLonFull(i) = sin(fullCoords[0](i));
        }
        for (uword i = 0; i < halfCoords[0].size(); ++i) {
            cosLonHalf(i) = cos(halfCoords[0](i));
            sinLonHalf(i) = sin(halfCoords[0](i));
        }
    } else if (axisIdx == 1) {
        cosLatFull.set_size(fullCoords[1].size());
        sinLatFull.set_size(fullCoords[1].size());
        sinLatFull2.set_size(fullCoords[1].size());
        cosLatHalf.set_size(halfCoords[1].size());
        sinLatHalf.set_size(halfCoords[1].size());
        sinLatHalf2.set_size(halfCoords[1].size());
        tanLatFull.set_size(fullCoords[1].size());
        tanLatHalf.set_size(halfCoords[1].size());
        for (uword j = 0; j < fullCoords[1].size(); ++j) {
            cosLatFull(j) = cos(fullCoords[1](j));
            sinLatFull(j) = sin(fullCoords[1](j));
            sinLatFull2(j) = sinLatFull(j)*sinLatFull(j);
            tanLatFull(j) = tan(fullCoords[1](j));
        }
        for (uword j = 0; j < halfCoords[1].size(); ++j) {
            cosLatHalf(j) = cos(halfCoords[1](j));
            sinLatHalf(j) = sin(halfCoords[1](j));
            sinLatHalf2(j) = sinLatHalf(j)*sinLatHalf(j);
            tanLatHalf(j) = tan(halfCoords[1](j));
        }
    }
    for (uword m = 0; m < this->domain().numDim(); ++m) {
        if (fullCoords[m].size() == 0) {
            return;
        }
    }
    setCellVolumes();
} // setGridCoordComps

void RLLMesh::
setCellVolumes() {
    if (sinLatHalf.size() == 0) {
        // This is a kluge to avoid index overflow when sin and cos are not set.
        return;
    }
    volumes.set_size(numGrid(0, GridType::FULL),
                     numGrid(1, GridType::FULL),
                     numGrid(2, GridType::FULL));
    double R2 = domain().radius()*domain().radius();
    for (uword k = 0; k < volumes.n_slices; ++k) {
        for (uword j = 1; j < volumes.n_cols-1; ++j) {
            double dsinLat = sinLatHalf(j)-sinLatHalf(j-1);
            for (uword i = 0; i < volumes.n_rows; ++i) {
                volumes(i, j, k) = R2*halfIntervals[0](i)*dsinLat;
            }
        }
        for (uword i = 0; i < volumes.n_rows; ++i) {
            volumes(i, 0, k) = R2*halfIntervals[0](i)*
                (sinLatHalf(0)+1.0);
            volumes(i, volumes.n_cols-1, k) = R2*halfIntervals[0](i)*
                (1.0-sinLatHalf(volumes.n_cols-2));
        }
    }
#ifndef NDEBUG
    double totalArea = 0.0;
    for (uword j = 0; j < volumes.n_cols; ++j) {
        for (uword i = 0; i < volumes.n_rows; ++i) {
            totalArea += volumes(i, j, 0);
        }
    }
    assert(fabs(totalArea/R2-4*M_PI) < 1.0e-10);
#endif
} // setCellVolumes

double RLLMesh::
cosLon(int gridType, int i) const {
    switch (gridType) {
        case GridType::FULL:
            return cosLonFull[i];
        case GridType::HALF:
            return cosLonHalf[i];
        default:
            REPORT_ERROR("Unknown grid type!");
    }
} // cosLon

double RLLMesh::
sinLon(int gridType, int i) const {
    switch (gridType) {
        case GridType::FULL:
            return sinLonFull[i];
        case GridType::HALF:
            return sinLonHalf[i];
        default:
            REPORT_ERROR("Unknown grid type!");
    }
} // sinLon

double RLLMesh::
cosLat(int gridType, int j) const {
    switch (gridType) {
        case GridType::FULL:
            return cosLatFull[j];
        case GridType::HALF:
            return cosLatHalf[j];
        default:
            REPORT_ERROR("Unknown grid type!");
    }
} // cosLat

double RLLMesh::
sinLat(int gridType, int j) const {
    switch (gridType) {
        case GridType::FULL:
            return sinLatFull[j];
        case GridType::HALF:
            return sinLatHalf[j];
        default:
            REPORT_ERROR("Unknown grid type!");
    }
} // sinLat

double RLLMesh::
sinLat2(int gridType, int j) const {
    switch (gridType) {
        case GridType::FULL:
            return sinLatFull2[j];
        case GridType::HALF:
            return sinLatHalf2[j];
        default:
            REPORT_ERROR("Unknown grid type!");
    }
} // sinLat2

double RLLMesh::
tanLat(int gridType, int j) const {
    switch (gridType) {
        case GridType::FULL:
            return tanLatFull[j];
        case GridType::HALF:
            return tanLatHalf[j];
        default:
            REPORT_ERROR("Unknown grid type!");
    }
} // tanLat

void RLLMesh::
move(const SphereCoord &x0, double dt, const SphereVelocity &v,
     const RLLMeshIndex &idx, SphereCoord &x1) const {
    if (!idx.isOnPole()) {
        double dlon = dt*v(0)/domain().radius()/cos(x0(1));
        double dlat = dt*v(1)/domain().radius();
        double lon = x0(0)+dlon;
        double lat = x0(1)+dlat;
        if (lat > M_PI_2) {
            lon = M_PI+x0(0)-dlon;
            lat = M_PI-x0(1)-dlat;
        }
        if (lat < -M_PI_2) {
            lon =  M_PI+x0(0)-dlon;
            lat = -M_PI-x0(1)-dlat;
        }
        if (lon < 0.0) {
            lon = PI2+fmod(lon, PI2);
        } else if (lon > PI2) {
            lon = fmod(lon, PI2);
        }
        x1.setCoord(lon, lat);
    } else {
        x1.psCoord()[0] = x0.psCoord()[0]+dt*v.psVelocity()[0];
        x1.psCoord()[1] = x0.psCoord()[1]+dt*v.psVelocity()[1];
        x1.transformFromPS(domain(), idx.pole());
    }
    if (domain().numDim() == 3) {
        double dlev = dt*v(2);
        x1(2) = x0(2)+dlev;
#ifndef NDEBUG
        assert(x1(2) >= domain().axisStart(2) && x1(2) <= domain().axisEnd(2));
#endif
    }
} // move

void RLLMesh::
setGridCoords() {
    StructuredMesh<SphereDomain, SphereCoord>::setGridCoords();
    for (int loc = 0; loc < 5; ++loc) {
        for (uword cellIdx = 0; cellIdx < gridCoords[loc].size(); ++cellIdx) {
            gridCoords[loc][cellIdx].transformToCart(domain());
        }
    }
} // setGridCoords

vec RLLMesh::
cellSize(int loc, int cellIdx) const {
    vec res(this->domain().numDim());
    uvec spanIdx = unwrapIndex(loc, cellIdx);
    for (uword m = 0; m < this->domain().numDim(); ++m) {
        if (this->domain().axisStartBndType(m) == PERIODIC &&
            ((gridTypes(1, m, loc) == GridType::HALF && gridStyles[m] == FULL_LEAD) ||
             (gridTypes(1, m, loc) == GridType::FULL && gridStyles[m] == HALF_LEAD))) {
            res[m] = gridInterval(m, gridTypes[m], spanIdx[m]-1);
        } else {
            res[m] = gridInterval(m, gridTypes[m], spanIdx[m]);
        }
    }
    res[0] *= this->domain().radius()*cosLat(gridTypes(0, 1, loc), spanIdx[1]);
    res[1] *= this->domain().radius();
    return res;
} // cellSize

} // geomtk
