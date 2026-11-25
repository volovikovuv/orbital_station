#include "../include/module_factory.h"
#include "../include/module_hierarchy.h"

std::unique_ptr<Module> ModuleFactory::createModule(const std::string& type, const std::string& name) {
    StandardModuleFactory stdFactory;
    AdvancedModuleFactory advFactory;

    if (type == "crew" || type == "CrewCabin") {
        return stdFactory.createResidentialModule(name, 80);
    }
    if (type == "medical" || type == "MedicalCabin") {
        return advFactory.createResidentialModule(name, 90);
    }
    if (type == "solar" || type == "SolarModule") {
        return stdFactory.createEnergyModule(name, 85);
    }
    if (type == "reactor" || type == "ReactorModule") {
        return advFactory.createEnergyModule(name, 100);
    }
    if (type == "biology" || type == "BiologyLab") {
        return stdFactory.createResearchModule(name, 85);
    }
    if (type == "physics" || type == "PhysicsLab") {
        return advFactory.createResearchModule(name, 95);
    }

    return nullptr;
}

std::unique_ptr<Module> StandardModuleFactory::createResidentialModule(const std::string& name, int efficiency) {
    return std::make_unique<CrewCabin>(name, efficiency, ModuleState::Inactive);
}

std::unique_ptr<Module> StandardModuleFactory::createEnergyModule(const std::string& name, int efficiency) {
    return std::make_unique<SolarModule>(name, efficiency, ModuleState::Inactive);
}

std::unique_ptr<Module> StandardModuleFactory::createResearchModule(const std::string& name, int efficiency) {
    return std::make_unique<BiologyLab>(name, efficiency, ModuleState::Inactive);
}

std::unique_ptr<Module> AdvancedModuleFactory::createResidentialModule(const std::string& name, int efficiency) {
    return std::make_unique<MedicalCabin>(name, efficiency, ModuleState::Inactive);
}

std::unique_ptr<Module> AdvancedModuleFactory::createEnergyModule(const std::string& name, int efficiency) {
    return std::make_unique<ReactorModule>(name, efficiency, ModuleState::Inactive);
}

std::unique_ptr<Module> AdvancedModuleFactory::createResearchModule(const std::string& name, int efficiency) {
    return std::make_unique<PhysicsLab>(name, efficiency, ModuleState::Inactive);
}