namespace geomtk {

template <class MeshType>
Field<MeshType>::Field() {
    _numDim = 0;
    _mesh = NULL;
    createTag = false;
}

template <class MeshType>
void Field<MeshType>::create(const string &name, const string &units,
                             const string &longName, const MeshType &mesh,
                             int numDim, bool hasHalfLevel) {
    _name = name;
    _units = units;
    _longName = longName;
    _mesh = &mesh;
    this->_numDim = numDim;
    this->hasHalfLevel = hasHalfLevel;
    createTag = true;
}

template <class MeshType>
Field<MeshType>& Field<MeshType>::operator=(const Field<MeshType> &other) {
    if (this != &other) {
        _name = other._name;
        _longName = other._longName;
        _units = other._units;
        _numDim = other._numDim;
        _mesh = other._mesh;
        hasHalfLevel = other.hasHalfLevel;
    }
    return *this;
}

}
