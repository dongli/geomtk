#include "Field.h"

namespace geomtk {

Field::Field(const Mesh &mesh, bool hasHalfLevel) {
    this->name = "N/A";
    this->units = "N/A";
    this->longName = "N/A";
    this->mesh = &mesh;
}

Field::Field(const string &name, const string &units, const string &longName,
             const Mesh &mesh, bool hasHalfLevel) {
    this->name = name;
    this->units = units;
    this->longName = longName;
    this->mesh = &mesh;
}
    
Field::~Field() {
}

const Mesh& Field::getMesh() const {
    return *mesh;
}

}
