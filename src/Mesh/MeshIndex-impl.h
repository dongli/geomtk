namespace geomtk {

template <class MeshType, class CoordType>
MeshIndex<MeshType, CoordType>::MeshIndex() {
}

template <class MeshType, class CoordType>
MeshIndex<MeshType, CoordType>::MeshIndex(int numDim) {
	this->numDim = numDim;
}

template <class MeshType, class CoordType>
MeshIndex<MeshType, CoordType>::~MeshIndex() {
}

template <class MeshType, class CoordType>
void MeshIndex<MeshType, CoordType>::setNumDim(int numDim) {
    this->numDim = numDim;
}

template <class MeshType, class CoordType>
MeshIndex<MeshType, CoordType>& MeshIndex<MeshType, CoordType>::operator=(const MeshIndex &other) {
    if (this != &other) {
    	numDim = other.numDim;
    }
    return *this;
}

}
