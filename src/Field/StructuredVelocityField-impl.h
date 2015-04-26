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
        v[0].create("u", mesh.domain().axisName(0)+" s-1", "x wind speed", mesh, Location::X_FACE, mesh.domain().numDim(), hasHalfLevel);
        v[1].create("v", mesh.domain().axisName(1)+" s-1", "y wind speed", mesh, Location::Y_FACE, mesh.domain().numDim(), hasHalfLevel);
        if (mesh.domain().numDim() == 3) {
            v[2].create("w", mesh.domain().axisName(2)+" s-1", "z wind speed", mesh, Location::Z_FACE, mesh.domain().numDim(), hasHalfLevel);
        }
    } else {
        v[0].create("u", mesh.domain().axisName(0)+" s-1", "x wind speed", mesh, Location::CENTER, mesh.domain().numDim(), hasHalfLevel);
        v[1].create("v", mesh.domain().axisName(1)+" s-1", "y wind speed", mesh, Location::CENTER, mesh.domain().numDim(), hasHalfLevel);
        if (mesh.domain().numDim() == 3) {
            v[2].create("w", mesh.domain().axisName(2)+" s-1", "z wind speed", mesh, Location::CENTER, mesh.domain().numDim(), hasHalfLevel);
        }
    }
    div.create("div", "s-1", "divergence", mesh, Location::CENTER, mesh.domain().numDim(), hasHalfLevel);
    if (mesh.domain().numDim() == 2) {
        vor.resize(1);
        vor[0].create("vor_xy", "s-1", "vorticity (x-y)", mesh, Location::CENTER, mesh.domain().numDim(), hasHalfLevel);
    } else {
        REPORT_WARNING("Under construction!");
    }
} // create

template <typename MeshType, typename FieldType>
void StructuredVelocityField<MeshType, FieldType>::
applyBndCond(const TimeLevelIndex<2> &timeIdx, bool updateHalfLevel) {
    for (uword m = 0; m < v.size(); ++m) {
        v[m].applyBndCond(timeIdx, updateHalfLevel);
    }
    calcDivergence(timeIdx);
    div.applyBndCond(timeIdx, updateHalfLevel);
    calcVorticity(timeIdx);
    for (uword m = 0; m < vor.size(); ++m) {
        vor[m].applyBndCond(timeIdx);
    }
} // applyBndCond

} // geomtk
