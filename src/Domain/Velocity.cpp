#include "Velocity.h"

namespace geomtk {

Velocity::Velocity() {
    v.set_size(3);
    v.fill(0);
}

Velocity::Velocity(int numDim) {
    v.set_size(numDim);
    v.fill(0);
}

Velocity::~Velocity() {
}

void Velocity::
setNumDim(int numDim) {
    v.resize(numDim);
} // setNumDim

const Velocity Velocity::
operator+(const Velocity &other) const {
    Velocity res;
    res.v = v+other.v;
    return res;
} // operator+

const Velocity Velocity::
operator-(const Velocity &other) const {
    Velocity res;
    res.v = v-other.v;
    return res;
} // operator-

const Velocity Velocity::
operator*(double scale) const {
    Velocity res;
    res.v = v*scale;
    return res;
} // operator*

const Velocity Velocity::
operator/(double scale) const {
    Velocity res;
    res.v = v/scale;
    return res;
} // operator/

void Velocity::
print() const {
    cout << "Velocity:";
    for (uword i = 0; i < v.size(); ++i) {
        cout << setw(20) << v(i);
    }
    cout << endl;
} // print

} // geomtk