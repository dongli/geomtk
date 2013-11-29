#include "RLLMeshIndex.h"

RLLMeshIndex::RLLMeshIndex(Mesh &mesh) : StructuredMeshIndex(mesh) {

}

RLLMeshIndex::~RLLMeshIndex() {
}

void RLLMeshIndex::locate(SpaceCoord &x) {
	StructuredMeshIndex::locate(x);
}
