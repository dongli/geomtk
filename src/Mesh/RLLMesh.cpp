#include "RLLMesh.h"
#include "RLLMeshIndex.h"
#include "IOManager.h"
#include "SphereDomain.h"

namespace geomtk {

RLLMesh::RLLMesh(SphereDomain &domain) : StructuredMesh<SphereDomain, SphereCoord>(domain) {
    type = RLL_MESH;
    poleRadius = 18.0*RAD;
}

RLLMesh::~RLLMesh() {
}

void RLLMesh::init(const string &fileName) {
    IOManager<RLLDataFile> io;
    int fileIdx = io.registerInputFile(*this, fileName);
    io.open(fileIdx);
    io.file(fileIdx).inputGrids();
    io.close(fileIdx);
    StructuredMesh<SphereDomain, SphereCoord>::init(fileName);
    set = true;
}

void RLLMesh::init(const string &fileNameH, const string &fileNameV) {
    IOManager<RLLDataFile> io;
    int fileIdx = io.registerInputFile(*this, fileNameH);
    io.open(fileIdx);
    io.file(fileIdx).inputHorizontalGrids();
    io.close(fileIdx);
    fileIdx = io.registerInputFile(*this, fileNameV);
    io.open(fileIdx);
    io.file(fileIdx).inputVerticalGrids();
    io.close(fileIdx);
    StructuredMesh<SphereDomain, SphereCoord>::init(fileNameH, fileNameV);
    set = true;
}

void RLLMesh::init(int nx, int ny, int nz) {
    StructuredMesh<SphereDomain, SphereCoord>::init(nx, ny, nz);
}

void RLLMesh::setGridCoordComps(int axisIdx, int size, const vec &full,
                                const vec &half) {
    StructuredMesh<SphereDomain, SphereCoord>::
    setGridCoordComps(axisIdx, size, full, half);
    if (axisIdx == 0) {
        cosLonFull.set_size(fullCoords[0].size());
        sinLonFull.set_size(fullCoords[0].size());
        cosLonHalf.set_size(halfCoords[0].size());
        sinLonHalf.set_size(halfCoords[0].size());
        for (int i = 0; i < fullCoords[0].size(); ++i) {
            cosLonFull(i) = cos(fullCoords[0](i));
            sinLonFull(i) = sin(fullCoords[0](i));
        }
        for (int i = 0; i < halfCoords[0].size(); ++i) {
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

void RLLMesh::setGridCoordComps(int axisIdx, int size, const vec &full) {
    StructuredMesh<SphereDomain, SphereCoord>::
    setGridCoordComps(axisIdx, size, full);
    if (axisIdx == 0) {
        cosLonFull.set_size(fullCoords[0].size());
        sinLonFull.set_size(fullCoords[0].size());
        cosLonHalf.set_size(halfCoords[0].size());
        sinLonHalf.set_size(halfCoords[0].size());
        for (int i = 0; i < fullCoords[0].size(); ++i) {
            cosLonFull(i) = cos(fullCoords[0](i));
            sinLonFull(i) = sin(fullCoords[0](i));
        }
        for (int i = 0; i < halfCoords[0].size(); ++i) {
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

void RLLMesh::setCellVolumes() {
    volumes.set_size(getNumGrid(0, GridType::FULL),
                     getNumGrid(1, GridType::FULL),
                     getNumGrid(2, GridType::FULL));
    double R2 = domain->getRadius()*domain->getRadius();
    for (int k = 0; k < volumes.n_slices; ++k) {
        for (int j = 1; j < volumes.n_cols-1; ++j) {
            double dsinLat = sinLatHalf(j)-sinLatHalf(j-1);
            for (int i = 0; i < volumes.n_rows; ++i) {
                volumes(i, j, k) = R2*halfIntervals[0](i)*dsinLat;
            }
        }
        for (int i = 0; i < volumes.n_rows; ++i) {
            volumes(i, 0, k) = R2*halfIntervals[0](i)*
                (sinLatHalf(0)+1.0);
            volumes(i, volumes.n_cols-1, k) = R2*halfIntervals[0](i)*
                (1.0-sinLatHalf(volumes.n_cols-2));
        }
    }
#ifndef NDEBUG
    double totalArea = 0.0;
    for (int j = 0; j < volumes.n_cols; ++j) {
        for (int i = 0; i < volumes.n_rows; ++i) {
            totalArea += volumes(i, j, 0);
        }
    }
    assert(fabs(totalArea/R2-4*M_PI) < 1.0e-10);
#endif
}

double RLLMesh::getCosLon(int gridType, int i) const {
    switch (gridType) {
        case GridType::FULL:
            return cosLonFull(i+1);
        case GridType::HALF:
            return cosLonHalf(i+1);
        default:
            REPORT_ERROR("Unknown grid type!");
    }
}

double RLLMesh::getSinLon(int gridType, int i) const {
    switch (gridType) {
        case GridType::FULL:
            return sinLonFull(i+1);
        case GridType::HALF:
            return sinLonHalf(i+1);
        default:
            REPORT_ERROR("Unknown grid type!");
    }
}

double RLLMesh::getCosLat(int gridType, int j) const {
    switch (gridType) {
        case GridType::FULL:
            return cosLatFull(j);
        case GridType::HALF:
            return cosLatHalf(j);
        default:
            REPORT_ERROR("Unknown grid type!");
    }
}

double RLLMesh::getSinLat(int gridType, int j) const {
    switch (gridType) {
        case GridType::FULL:
            return sinLatFull(j);
        case GridType::HALF:
            return sinLatHalf(j);
        default:
            REPORT_ERROR("Unknown grid type!");
    }
}

double RLLMesh::getSinLat2(int gridType, int j) const {
    switch (gridType) {
        case GridType::FULL:
            return sinLatFull2(j);
        case GridType::HALF:
            return sinLatHalf2(j);
        default:
            REPORT_ERROR("Unknown grid type!");
    }
}

double RLLMesh::getTanLat(int gridType, int j) const {
    switch (gridType) {
        case GridType::FULL:
            return tanLatFull(j);
        case GridType::HALF:
            return tanLatHalf(j);
        default:
            REPORT_ERROR("Unknown grid type!");
    }
}

void RLLMesh::move(const SphereCoord &x0, double dt, const SphereVelocity &v,
                   const RLLMeshIndex &idx, SphereCoord &x1) const {
    if (!idx.isOnPole()) {
        double dlon = dt*v(0)/domain->getRadius()/cos(x0(1));
        double dlat = dt*v(1)/domain->getRadius();
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
        x1[0] = x0[0]+dt*v[0];
        x1[1] = x0[1]+dt*v[1];
        x1.transformFromPS(*domain, idx.getPole());
    }
    if (domain->getNumDim() == 3) {
        double dlev = dt*v(2);
        x1(2) = x0(2)+dlev;
#ifndef NDEBUG
        assert(x1(2) >= domain->getAxisStart(2) && x1(2) <= domain->getAxisEnd(2));
#endif
    }
}

void RLLMesh::setGridCoords() {
    StructuredMesh<SphereDomain, SphereCoord>::setGridCoords();
    for (int loc = 0; loc < 5; ++loc) {
        for (int cellIdx = 0; cellIdx < gridCoords[loc].size(); ++cellIdx) {
            gridCoords[loc][cellIdx].transformToCart(*domain);
        }
    }
}

} // geomtk
