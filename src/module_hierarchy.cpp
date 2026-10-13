#include "../include/module_hierarchy.h"
#include <memory>

// Residential Modules

std::unique_ptr<Module> CrewCabin::clone() {
    return std::make_unique<CrewCabin>(*this);
}

std::string CrewCabin::getType() {
    return "CrewCabin";
}

std::unique_ptr<Module> MedicalCabin::clone() {
    return std::make_unique<MedicalCabin>(*this);
}

std::string MedicalCabin::getType() {
    return "MedicalCabin";
}

// Energy Modules

std::unique_ptr<Module> SolarModule::clone() {
    return std::make_unique<SolarModule>(*this);
}

std::string SolarModule::getType() {
    return "SolarModule";
}

std::unique_ptr<Module> ReactorModule::clone() {
    return std::make_unique<ReactorModule>(*this);
}

std::string ReactorModule::getType() {
    return "ReactorModule";
}

// Research Modules

std::unique_ptr<Module> BiologyLab::clone() {
    return std::make_unique<BiologyLab>(*this);
}

std::string BiologyLab::getType() {
    return "BiologyLab";
}

std::unique_ptr<Module> PhysicsLab::clone() {
    return std::make_unique<PhysicsLab>(*this);
}

std::string PhysicsLab::getType() {
    return "PhysicsLab";
}