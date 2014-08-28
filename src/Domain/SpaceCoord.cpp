#include "SpaceCoord.h"

namespace geomtk {

SpaceCoord::SpaceCoord() {
}

SpaceCoord::SpaceCoord(int numDim) {
    coord.resize(numDim);
}

SpaceCoord::SpaceCoord(const SpaceCoord &other) {
    coord.resize(other.coord.size());
    *this = other;
}

SpaceCoord::~SpaceCoord() {
}

void SpaceCoord::setNumDim(int numDim) {
    coord.resize(numDim);
}

void SpaceCoord::setCoord(double x, double y) {
    coord[0] = x;
    coord[1] = y;
}

void SpaceCoord::setCoord(double x, double y, double z) {
    coord[0] = x;
    coord[1] = y;
    coord[2] = z;
}

void SpaceCoord::setCoordComp(int dim, double comp) {
    coord(dim) = comp;
}

SpaceCoord& SpaceCoord::operator=(const SpaceCoord &other) {
    if (this != &other) {
        coord = other.coord;
    }
    return *this;
}

void SpaceCoord::print() const {
    cout << "Coordinate:";
    for (int i = 0; i < coord.size(); ++i) {
        cout << setw(20) << setprecision(10) << coord(i);
    }
    cout << endl;
}

} // geomtk