#include "Velocity.h"

namespace geomtk {

Velocity::Velocity() {
    v.set_size(3);
}

Velocity::Velocity(int numDim) {
    v.set_size(numDim);
}

Velocity::~Velocity() {
}

void Velocity::setNumDim(int numDim) {
    v.resize(numDim);
}

const Velocity Velocity::operator+(const Velocity &other) const {
    Velocity res;
    res.v = v+other.v;
    return res;
}

const Velocity Velocity::operator-(const Velocity &other) const {
    Velocity res;
    res.v = v-other.v;
    return res;
}

const Velocity Velocity::operator*(double scale) const {
    Velocity res;
    res.v = v*scale;
    return res;
}

const Velocity Velocity::operator/(double scale) const {
    Velocity res;
    res.v = v/scale;
    return res;
}

void Velocity::print() const {
    cout << "Velocity:";
    for (int i = 0; i < v.size(); ++i) {
        cout << setw(20) << v(i);
    }
    cout << endl;
}

} // geomtk