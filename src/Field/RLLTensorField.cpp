#include "RLLTensorField.h"
#include "SphereDomain.h"
#include "RLLMesh.h"

namespace geomtk {

RLLTensorField::RLLTensorField(const Mesh &mesh, bool hasHalfLevel)
    : StructuredTensorField(mesh, hasHalfLevel) {
    if (dynamic_cast<const RLLMesh*>(&mesh) == NULL) {
        REPORT_ERROR("Mesh should be RLLMesh!");
    }
}

RLLTensorField::RLLTensorField(const string &name, const string &units,
                               const string &longName, const Mesh &mesh,
                               bool hasHalfLevel)
    : StructuredTensorField(name, units, longName, mesh, hasHalfLevel) {
    if (dynamic_cast<const RLLMesh*>(&mesh) == NULL) {
        REPORT_ERROR("Mesh should be RLLMesh!");
    }
}

RLLTensorField::~RLLTensorField() {

}

void RLLTensorField::applyBndCond(int timeLevel, bool updateHalfLevel) {
    StructuredTensorField::applyBndCond(timeLevel, updateHalfLevel);
}

void RLLTensorField::create() {
    StructuredTensorField::create();
}
    
/*
                                 u5                  u6
                |      o---------x---------o---------x---------o
                |      |i-1,j+1  |i-1,j+1  |i,j+1    |i,j+1    |i+1,j+1
                |    v2|         |       v4|         |       v6|
        | dlat2 |      *         |         *         |         *         tanLat2
        |       |      |i-1,j    |         |i,j      |         |i+1,j
        |       |      |         |u3       |         |u4       |
  dlat0 |       |  |   o---------x---------#---------x---------o         tanLat0
        |          |   |i-1,j    |i-1,j    |i,j      |i,j      |i+1,j
        |          | v1|         |       v3|         |       v5|
        |    dlat1 |   *         |         *         |         *         tanLat1
                   |   |i-1,j-1  |         |i,j-1    |         |i+1,j-1
                   |   |         |u1       |         |u2       |
                   |   o---------x---------o---------x---------o
                       i-1,j-1   i-1,j-1   i,j-1     i,j-1     i+1,j-1

                                    ---------------------
                                            dlon


                  [    1   ∂u   w   v             1 ∂u          ∂u   ]
                  [  ----- -- + - + - tanφ        - --          --   ]
                  [  Rcosφ ∂λ   R   R             R ∂𝜑          ∂r   ]
            ∇V =  [                                                  ]
                  [      1   ∂v   u             1 ∂v   w        ∂v   ]
                  [    ----- -- - - tanφ        - -- + -        --   ]
                  [    Rcosφ ∂λ   R             R ∂φ   R        ∂r   ]
                  [                                                  ]
                  [        1  ∂w   u            1 ∂w   v        ∂w   ]
                  [     ----- -- - -            - -- - -        --   ]
                  [     Rcosφ ∂λ   R            R ∂φ   R        ∂r   ]
 
                  [    1   ∂u    1 ∂u   u      ]
                  [  ----- --    - -- + - tanφ ]
                  [  Rcosφ ∂λ    R ∂𝜑   R      ]
            ∇V =  [                            ]
                  [    1   ∂v    1 ∂v   v      ]
                  [  ----- --    - -- + - tanφ ]
                  [  Rcosφ ∂λ    R ∂φ   R      ]
 */

void RLLTensorField::calcFromVector(const RLLVectorField &vector,
                                    int timeLevel) {
    if (vector.getGridType() != C_GRID) {
        REPORT_ERROR("The vector field should be in C grid!");
    }
    const RLLMesh &mesh = static_cast<const RLLMesh&>(*(this->mesh));
    double R = static_cast<const SphereDomain&>(mesh.getDomain()).getRadius();
    // -------------------------------------------------------------------------
    for (int k = 0; k < mesh.getNumGrid(2, CENTER); ++k) {
        for (int j = 1; j < mesh.getNumGrid(1, CENTER)-1; ++j) {
            double RcosLat = R*mesh.getCosLat(CENTER, j);
            double dlat0 = mesh.getGridInterval(1, EDGE, j);
            double dlat1 = mesh.getGridInterval(1, CENTER, j-1);
            double dlat2 = mesh.getGridInterval(1, CENTER,   j);
            double tanLat0 = mesh.getTanLat(CENTER, j);
            double tanLat1 = mesh.getTanLat(EDGE, j-1);
            double tanLat2 = mesh.getTanLat(EDGE,   j);
            for (int i = 0; i < mesh.getNumGrid(0, CENTER); ++i) {
                // =============================================================
                double dlon = mesh.getGridInterval(0, EDGE, i);
                double u1 = vector(timeLevel, 0, i-1, j-1, k);
                double u2 = vector(timeLevel, 0,   i, j-1, k);
                double u3 = vector(timeLevel, 0, i-1,   j, k);
                double u4 = vector(timeLevel, 0,   i,   j, k);
                double u5 = vector(timeLevel, 0, i-1, j+1, k);
                double u6 = vector(timeLevel, 0,   i, j+1, k);
                double v1 = vector(timeLevel, 1, i-1, j-1, k);
                double v2 = vector(timeLevel, 1, i-1,   j, k);
                double v3 = vector(timeLevel, 1,   i, j-1, k);
                double v4 = vector(timeLevel, 1,   i,   j, k);
                double v5 = vector(timeLevel, 1, i+1, j-1, k);
                double v6 = vector(timeLevel, 1, i+1,   j, k);
                // =============================================================
                double utanLat_R = (u3+u4)*0.5*tanLat0/R;
                double vtanLat_R = (v3*tanLat1+v4*tanLat2)*0.5/R;
                double dudlon = (u4-u3)/dlon/RcosLat;
                double dudlat = ((u5+u6)-(u1+u2))*0.5/(dlat1+dlat2)/R;
                double dvdlon = ((v5+v6)-(v1+v2))*0.25/dlon/RcosLat;
                double dvdlat = (v4-v3)/dlat0/R;
                // =============================================================
                (*this)(timeLevel, 0, 0, i, j, k) = dudlon;
                (*this)(timeLevel, 0, 1, i, j, k) = dudlat+utanLat_R;
                (*this)(timeLevel, 1, 0, i, j, k) = dvdlon;
                (*this)(timeLevel, 1, 1, i, j, k) = dvdlat+vtanLat_R;
            }
        }
    }
}

}