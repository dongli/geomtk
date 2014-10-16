namespace geomtk {

template <typename MeshType, typename FieldType>
StructuredVelocityField<MeshType, FieldType>::StructuredVelocityField() {
}

template <typename MeshType, typename FieldType>
StructuredVelocityField<MeshType, FieldType>::~StructuredVelocityField() {
}

template <typename MeshType, typename FieldType>
void StructuredVelocityField<MeshType, FieldType>::
create(const MeshType &mesh, bool useStagger, bool hasHalfLevel) {
    v.resize(mesh.domain().numDim());
    if (useStagger) {
        v[0].create("u", "m s-1", "x wind speed", mesh, Location::X_FACE, 2, hasHalfLevel);
        v[1].create("v", "m s-1", "y wind speed", mesh, Location::Y_FACE, 2, hasHalfLevel);
        if (mesh.domain().numDim() == 3) {
            REPORT_ERROR("Under construction!");
            v[2].create("w", "?", "z wind speed", mesh, Location::Z_FACE, 2, hasHalfLevel);
        }
    } else {
        v[0].create("u", "m s-1", "x wind speed", mesh, Location::CENTER, 2, hasHalfLevel);
        v[1].create("v", "m s-1", "y wind speed", mesh, Location::CENTER, 2, hasHalfLevel);
        if (mesh.domain().numDim() == 3) {
            REPORT_ERROR("Under construction!");
            v[2].create("w", "?", "z wind speed", mesh, Location::CENTER, 2, hasHalfLevel);
        }
    }
    div.create("div", "s-1", "divergence", mesh, Location::CENTER, 2, hasHalfLevel);
    if (mesh.domain().numDim() == 2) {
        vor.resize(1);
        vor[0].create("vor_xy", "s-1", "vorticity (x-y)", mesh, Location::CENTER, 2, hasHalfLevel);
    } else {
        REPORT_ERROR("Under construction!");
    }
}

template <typename MeshType, typename FieldType>
void StructuredVelocityField<MeshType, FieldType>::
applyBndCond(const TimeLevelIndex<2> &timeIdx, bool updateHalfLevel) {
    for (int m = 0; m < v.size(); ++m) {
        v[m].applyBndCond(timeIdx, updateHalfLevel);
    }
    calcDivergence(timeIdx);
    div.applyBndCond(timeIdx, updateHalfLevel);
    calcVorticity(timeIdx);
    for (int m = 0; m < vor.size(); ++m) {
        vor[m].applyBndCond(timeIdx);
    }
}

template <typename MeshType, typename FieldType>
void StructuredVelocityField<MeshType, FieldType>::
calcDivergence(const TimeLevelIndex<2> &timeIdx) {
    switch (mesh().domain().numDim()) {
        case 2:
            if (mesh().domain().axisStartBndType(0) == PERIODIC &&
                mesh().domain().axisStartBndType(1) == PERIODIC) {
                if (v[0].staggerLocation() == Location::CENTER &&
                    v[1].staggerLocation() == Location::CENTER) {
                    for (int j = mesh().js(GridType::FULL); j <= mesh().je(GridType::FULL); ++j) {
                        for (int i = mesh().is(GridType::FULL); i <= mesh().ie(GridType::FULL); ++i) {
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
                    for (int j = mesh().js(GridType::FULL); j <= mesh().je(GridType::FULL); ++j) {
                        for (int i = mesh().is(GridType::FULL); i <= mesh().ie(GridType::FULL); ++i) {
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
            } else {
                REPORT_ERROR("Under construction!");
            }
        case 3:
            REPORT_ERROR("Under construction!");
        default:
            REPORT_ERROR("Unexpected branch!");
    }
}

template <typename MeshType, typename FieldType>
void StructuredVelocityField<MeshType, FieldType>::
calcVorticity(const TimeLevelIndex<2> &timeIdx) {
    switch (mesh().domain().numDim()) {
        case 2:
            if (mesh().domain().axisStartBndType(0) == PERIODIC &&
                mesh().domain().axisStartBndType(1) == PERIODIC) {
                if (v[0].staggerLocation() == Location::CENTER &&
                    v[1].staggerLocation() == Location::CENTER) {
                    for (int j = mesh().js(GridType::FULL); j <= mesh().je(GridType::FULL); ++j) {
                        for (int i = mesh().is(GridType::FULL); i <= mesh().ie(GridType::FULL); ++i) {
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
                    for (int j = mesh().js(GridType::FULL); j <= mesh().je(GridType::FULL); ++j) {
                        for (int i = mesh().is(GridType::FULL); i <= mesh().ie(GridType::FULL); ++i) {
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
}

}
