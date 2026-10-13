#include "../include/module_factory.h"
#include "../include/module_hierarchy.h"
#include <memory>

// StandardModuleFactory

std::unique_ptr<Module> StandardModuleFactory::createResidentialModule(const std::string& name, int efficiency) {
    return std::make_unique<CrewCabin>(name, efficiency, ModuleState::Inactive);
}

std::unique_ptr<Module> StandardModuleFactory::createEnergyModule(const std::string& name, int efficiency) {
    return std::make_unique<SolarModule>(name, efficiency, ModuleState::Inactive);
}

std::unique_ptr<Module> StandardModuleFactory::createResearchModule(const std::string& name, int efficiency) {
    return std::make_unique<BiologyLab>(name, efficiency, ModuleState::Inactive);
}

// AdvancedModuleFactory

std::unique_ptr<Module> AdvancedModuleFactory::createResidentialModule(const std::string& name, int efficiency) {
    return std::make_unique<MedicalCabin>(name, efficiency, ModuleState::Inactive);
}

std::unique_ptr<Module> AdvancedModuleFactory::createEnergyModule(const std::string& name, int efficiency) {
    return std::make_unique<ReactorModule>(name, efficiency, ModuleState::Inactive);
}

std::unique_ptr<Module> AdvancedModuleFactory::createResearchModule(const std::string& name, int efficiency) {
    return std::make_unique<PhysicsLab>(name, efficiency, ModuleState::Inactive);
}