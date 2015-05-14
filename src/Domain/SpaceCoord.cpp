#include "SpaceCoord.h"

namespace geomtk {

SpaceCoord::
SpaceCoord() {
}

SpaceCoord::
SpaceCoord(int numDim) {
    coord.resize(numDim);
}

SpaceCoord::
SpaceCoord(const SpaceCoord &other) {
    coord.resize(other.coord.size());
    *this = other;
}

SpaceCoord::
~SpaceCoord() {
}

void SpaceCoord::
init(int numDim) {
    coord.resize(numDim);
} // init

void SpaceCoord::
set(double x, double y) {
    coord[0] = x;
    coord[1] = y;
} // set

void SpaceCoord::
set(double x, double y, double z) {
    coord[0] = x;
    coord[1] = y;
    coord[2] = z;
} // set

void SpaceCoord::
setComp(int i, double comp) {
#ifndef NDEBUG
    coord(i) = comp;
#else
    coord[i] = comp;
#endif
} // setComp

SpaceCoord& SpaceCoord::
operator=(const SpaceCoord &other) {
    if (this != &other) {
        coord = other.coord;
    }
    return *this;
} // operator=

void SpaceCoord::
print() const {
    cout << "Coordinate:";
    for (uword i = 0; i < coord.size(); ++i) {
        cout << setw(20) << setprecision(10) << coord(i);
    }
    cout << endl;
} // print

} // geomtk
