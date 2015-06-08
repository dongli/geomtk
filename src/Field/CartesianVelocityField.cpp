#include "CartesianVelocityField.h"

namespace geomtk {

void CartesianVelocityField::
calcDivergence(const TimeLevelIndex<2> &timeIdx) {
    switch (mesh().domain().numDim()) {
    case 2:
        if (v[0].staggerLocation() == Location::CENTER &&
            v[1].staggerLocation() == Location::CENTER) {
            for (uword j = mesh().js(GridType::FULL); j <= mesh().je(GridType::FULL); ++j) {
                for (uword i = mesh().is(GridType::FULL); i <= mesh().ie(GridType::FULL); ++i) {
                    double u1 = v[0](timeIdx, i-1, j);
                    double u2 = v[0](timeIdx, i+1, j);
                    double dx = (mesh().gridInterval(0, GridType::FULL, i-1)+
                                 mesh().gridInterval(0, GridType::FULL, i));
                    double dudx = (u2-u1)/dx;
                    double v1 = v[1](timeIdx, i, j-1);
                    double v2 = v[1](timeIdx, i, j+1);
                    double dy = (mesh().gridInterval(1, GridType::FULL, j-1)+
                                 mesh().gridInterval(1, GridType::FULL, j));
                    double dvdy = (v2-v1)/dy;
                    div(timeIdx, i, j) = dudx+dvdy;
                }
            }
        } else if (v[0].staggerLocation() == Location::X_FACE &&
                   v[1].staggerLocation() == Location::Y_FACE) {
            for (uword j = mesh().js(GridType::FULL); j <= mesh().je(GridType::FULL); ++j) {
                for (uword i = mesh().is(GridType::FULL); i <= mesh().ie(GridType::FULL); ++i) {
                    double u1 = v[0](timeIdx, i-1, j);
                    double u2 = v[0](timeIdx, i, j);
                    double dx = mesh().gridInterval(0, GridType::HALF, i);
                    double dudx = (u2-u1)/dx;
                    double v1 = v[1](timeIdx, i, j-1);
                    double v2 = v[1](timeIdx, i, j);
                    double dy = mesh().gridInterval(1, GridType::HALF, j);
                    double dvdy = (v2-v1)/dy;
                    div(timeIdx, i, j) = dudx+dvdy;
                }
            }
        } else {
            REPORT_ERROR("Under construction!");
        }
        break;
    case 3:
        if (v[0].staggerLocation() == Location::CENTER &&
            v[1].staggerLocation() == Location::CENTER &&
            v[2].staggerLocation() == Location::CENTER) {
            for (uword k = mesh().ks(GridType::FULL); k <= mesh().ks(GridType::FULL); ++k) {
                for (uword j = mesh().js(GridType::FULL); j <= mesh().je(GridType::FULL); ++j) {
                    for (uword i = mesh().is(GridType::FULL); i <= mesh().ie(GridType::FULL); ++i) {
                        double u1 = v[0](timeIdx, i-1, j, k);
                        double u2 = v[0](timeIdx, i+1, j, k);
                        double dx = (mesh().gridInterval(0, GridType::FULL, i-1)+
                                     mesh().gridInterval(0, GridType::FULL, i));
                        double dudx = (u2-u1)/dx;
                        double v1 = v[1](timeIdx, i, j-1, k);
                        double v2 = v[1](timeIdx, i, j+1, k);
                        double dy = (mesh().gridInterval(1, GridType::FULL, j-1)+
                                     mesh().gridInterval(1, GridType::FULL, j));
                        double dvdy = (v2-v1)/dy;
                        double w1 = v[2](timeIdx, i, j, k-1);
                        double w2 = v[2](timeIdx, i, j, k+1);
                        double dz = (mesh().gridInterval(2, GridType::FULL, j-1)+
                                     mesh().gridInterval(2, GridType::FULL, j));
                        double dwdz = (w2-w1)/dz;
                        div(timeIdx, i, j) = dudx+dvdy+dwdz;
                    }
                }
            }
        } else if (v[0].staggerLocation() == Location::X_FACE &&
                   v[1].staggerLocation() == Location::Y_FACE &&
                   v[2].staggerLocation() == Location::Z_FACE) {
            for (uword k = mesh().ks(GridType::FULL); k <= mesh().ks(GridType::FULL); ++k) {
                for (uword j = mesh().js(GridType::FULL); j <= mesh().je(GridType::FULL); ++j) {
                    for (uword i = mesh().is(GridType::FULL); i <= mesh().ie(GridType::FULL); ++i) {
                        double u1 = v[0](timeIdx, i-1, j, k);
                        double u2 = v[0](timeIdx, i,   j, k);
                        double dx = mesh().gridInterval(0, GridType::HALF, i);
                        double dudx = (u2-u1)/dx;
                        double v1 = v[1](timeIdx, i, j-1, k);
                        double v2 = v[1](timeIdx, i, j,   k);
                        double dy = mesh().gridInterval(1, GridType::HALF, j);
                        double dvdy = (v2-v1)/dy;
                        double w1 = v[2](timeIdx, i, j, k);
                        double w2 = v[2](timeIdx, i, j, k+1);
                        double dz = mesh().gridInterval(2, GridType::HALF, k);
                        double dwdz = (w2-w1)/dz;
                        div(timeIdx, i, j) = dudx+dvdy+dwdz;
                    }
                }
            }
        } else {
            REPORT_ERROR("Under construction!");
        }
        break;
    default:
        REPORT_ERROR("Unexpected branch!");
    }
} // calcDivergence

void CartesianVelocityField::
calcVorticity(const TimeLevelIndex<2> &timeIdx) {
    switch (mesh().domain().numDim()) {
        case 2:
            if (mesh().domain().axisStartBndType(0) == PERIODIC &&
                mesh().domain().axisStartBndType(1) == PERIODIC) {
                if (v[0].staggerLocation() == Location::CENTER &&
                    v[1].staggerLocation() == Location::CENTER) {
                    for (uword j = mesh().js(GridType::FULL); j <= mesh().je(GridType::FULL); ++j) {
                        for (uword i = mesh().is(GridType::FULL); i <= mesh().ie(GridType::FULL); ++i) {
                            double v1 = v[1](timeIdx, i-1, j);
                            double v2 = v[1](timeIdx, i, j);
                            double dx = (mesh().gridInterval(0, GridType::FULL, i-1)+
                                         mesh().gridInterval(0, GridType::FULL, i));
                            double dvdx = (v2-v1)/dx;
                            double u1 = v[0](timeIdx, i, j-1);
                            double u2 = v[0](timeIdx, i, j+1);
                            double dy = (mesh().gridInterval(1, GridType::FULL, j-1)+
                                         mesh().gridInterval(1, GridType::FULL, j));
                            double dudy = (u2-u1)/dy;
                            vor[0](timeIdx, i, j) = dvdx-dudy;
                        }
                    }
                } else if (v[0].staggerLocation() == Location::X_FACE &&
                           v[1].staggerLocation() == Location::Y_FACE) {
                    for (uword j = mesh().js(GridType::FULL); j <= mesh().je(GridType::FULL); ++j) {
                        for (uword i = mesh().is(GridType::FULL); i <= mesh().ie(GridType::FULL); ++i) {
                            double v1 = v[1](timeIdx, i-1, j-1);
                            double v2 = v[1](timeIdx, i-1, j );
                            double v3 = v[1](timeIdx, i+1, j-1);
                            double v4 = v[1](timeIdx, i+1, j  );
                            double dx = (mesh().gridInterval(0, GridType::HALF, i-1)+
                                         mesh().gridInterval(0, GridType::HALF, i));
                            double dvdx = (v3+v4-v1-v2)*0.5/dx;
                            double u1 = v[0](timeIdx, i-1, j-1);
                            double u2 = v[0](timeIdx, i,   j-1);
                            double u3 = v[0](timeIdx, i-1, j+1);
                            double u4 = v[0](timeIdx, i,   j+1);
                            double dy = (mesh().gridInterval(1, GridType::FULL, j-1)+
                                         mesh().gridInterval(1, GridType::FULL, j));
                            double dudy = (u4+u3-u2-u1)*0.5/dy;
                            vor[0](timeIdx, i, j) = dvdx-dudy;
                        }
                    }
                } else {
                    REPORT_ERROR("Under construction!");
                }
            } else {
                REPORT_ERROR("Under construction!");
            }
        case 3:
            REPORT_ERROR("Under construction!");
        default:
            REPORT_ERROR("Unexpected branch!");
    }
} // calcVorticity

} // geomtk