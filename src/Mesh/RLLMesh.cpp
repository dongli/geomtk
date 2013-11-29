#include "RLLMesh.h"
#include "SphereDomain.h"

RLLMesh::RLLMesh(Domain &domain) : StructuredMesh(domain) {
    if (dynamic_cast<SphereDomain*>(&domain) == NULL) {
        REPORT_ERROR("The domain should be SphereDomain!");
    }
}

RLLMesh::~RLLMesh() {
}

void RLLMesh::setPoleRadius(double radius) {
    poleRadius = radius;
}

void RLLMesh::setGridCoords(int dim, int size, double *full, double *half) {
	StructuredMesh::setGridCoords(dim, size, full, half);
	cosLonFull.resize(fullCoords[0].size());
	sinLonFull.resize(fullCoords[0].size());
	cosLonHalf.resize(halfCoords[0].size());
	sinLonHalf.resize(halfCoords[0].size());
	cosLatFull.resize(fullCoords[1].size());
	sinLatFull.resize(fullCoords[1].size());
	sinLatFull2.resize(fullCoords[1].size());
	cosLatHalf.resize(halfCoords[1].size());
	sinLatHalf.resize(halfCoords[1].size());
	sinLatHalf2.resize(halfCoords[1].size());
	for (int i = 0; i < fullCoords[0].size(); ++i) {
		cosLonFull(i) = cos(fullCoords[0](i));
		sinLonFull(i) = sin(fullCoords[0](i));
	}
	for (int i = 0; i < halfCoords[0].size(); ++i) {
		cosLonHalf(i) = cos(halfCoords[0](i));
		sinLonHalf(i) = sin(halfCoords[0](i));
	}
	for (int j = 0; j < fullCoords[1].size(); ++j) {
		cosLatFull(j) = cos(fullCoords[1](j));
		sinLatFull(j) = sin(fullCoords[1](j));
		sinLatFull2(j) = sinLatFull(j)*sinLatFull(j);
	}
	for (int j = 0; j < halfCoords[1].size(); ++j) {
		cosLatHalf(j) = cos(halfCoords[1](j));
		sinLatHalf(j) = sin(halfCoords[1](j));
		sinLatHalf2(j) = sinLatHalf(j)*sinLatHalf(j);
	}
}

double RLLMesh::getCosLon(StaggerType staggerType, int i) {
	switch (staggerType) {
		case CENTER:
			return cosLonFull(i);
		case EDGE: case VERTEX:
			return cosLonHalf(i);
	}
}

double RLLMesh::getSinLon(StaggerType staggerType, int i) {
	switch (staggerType) {
		case CENTER:
			return sinLonFull(i);
		case EDGE: case VERTEX:
			return sinLonHalf(i);
	}
}

double RLLMesh::getCosLat(StaggerType staggerType, int i) {
	switch (staggerType) {
		case CENTER:
			return cosLatFull(i);
		case EDGE: case VERTEX:
			return cosLatHalf(i);
	}
}

double RLLMesh::getSinLat(StaggerType staggerType, int i) {
	switch (staggerType) {
		case CENTER:
			return sinLatFull(i);
		case EDGE: case VERTEX:
			return sinLatHalf(i);
	}
}

double RLLMesh::getSinLat2(StaggerType staggerType, int i) {
	switch (staggerType) {
		case CENTER:
			return sinLatFull2(i);
		case EDGE: case VERTEX:
			return sinLatHalf2(i);
	}
}
