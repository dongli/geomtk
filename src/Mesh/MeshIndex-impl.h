namespace geomtk {

template <class MeshType, class CoordType>
MeshIndex<MeshType, CoordType>::
MeshIndex() {
}

template <class MeshType, class CoordType>
MeshIndex<MeshType, CoordType>::
MeshIndex(uword numDim) {
	this->numDim = numDim;
}

template <class MeshType, class CoordType>
MeshIndex<MeshType, CoordType>::
~MeshIndex() {
}

template <class MeshType, class CoordType>
void MeshIndex<MeshType, CoordType>::
init(uword numDim) {
    this->numDim = numDim;
} // init

template <class MeshType, class CoordType>
MeshIndex<MeshType, CoordType>& MeshIndex<MeshType, CoordType>::
operator=(const MeshIndex &other) {
    if (this != &other) {
    	numDim = other.numDim;
    }
    return *this;
} // operator=

} // geomtk
