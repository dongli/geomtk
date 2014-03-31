#include "Field.h"

namespace geomtk {
    
Field::Field() {
    mesh = NULL;
}
    
Field::~Field() {
}

void Field::common(const string &name, const string &units,
                   const string &longName, const Mesh &mesh,
                   bool hasHalfLevel) {
    this->name = name;
    this->units = units;
    this->longName = longName;
    this->mesh = &mesh;
    this->hasHalfLevel = hasHalfLevel;
}

void Field::setName(const string &name) {
    this->name = name;
}

void Field::setLongName(const string &longName) {
    this->longName = longName;
}

void Field::setUnits(const string &units) {
    this->units = units;
}
    
const string& Field::getName() const {
    return name;
}

const string& Field::getLongName() const {
    return longName;
}

const string& Field::getUnits() const {
    return units;
}
    
const Mesh& Field::getMesh() const {
    return *mesh;
}
    
Field& Field::operator=(const Field &other) {
    if (this != &other) {
        name = other.name;
        longName = other.longName;
        units = other.units;
        mesh = other.mesh;
        hasHalfLevel = other.hasHalfLevel;
    }
    return *this;
}

}
