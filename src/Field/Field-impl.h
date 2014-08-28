namespace geomtk {

template <class MeshType>
Field<MeshType>::Field() {
    numDim = 0;
    mesh = NULL;
    createTag = false;
}

template <class MeshType>
void Field<MeshType>::create(const string &name, const string &units,
                             const string &longName, const MeshType &mesh,
                             int numDim, bool hasHalfLevel) {
    this->name = name;
    this->units = units;
    this->longName = longName;
    this->mesh = &mesh;
    this->numDim = numDim;
    this->hasHalfLevel = hasHalfLevel;
    createTag = true;
}

template <class MeshType>
Field<MeshType>& Field<MeshType>::operator=(const Field<MeshType> &other) {
    if (this != &other) {
        name = other.name;
        longName = other.longName;
        units = other.units;
        numDim = other.numDim;
        mesh = other.mesh;
        hasHalfLevel = other.hasHalfLevel;
    }
    return *this;
}

}
