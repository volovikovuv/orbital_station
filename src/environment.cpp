#include "../include/environment.h"
#include "../include/interaction_strategy.h"
#include "../include/command_center.h"
#include "../include/module.h"

// EarthOrbit
void EarthOrbit::applyEffect(Module& module) {
    LoggingProxy::log(LogSource::ENV, "EarthOrbit: минимальное влияние на модуль " + module.getType());
}

std::string EarthOrbit::getName() const {
    return "EarthOrbit";
}

double EarthOrbit::getEnergyModifier() const {
    return 0.0;
}

double EarthOrbit::getDamageModifier() const {
    return 0.0;
}

// DeepSpace
void DeepSpace::applyEffect(Module& module) {
    module * getEnergyModifier();
    module - Damage{.amount = static_cast<int>(getDamageModifier()), .type = "Radiation"};

    if (const std::string type = module.getType(); type == "SolarModule") {
        EnergyModuleStrategy().apply(module);
    } else if (type == "CrewCabin" || type == "MedicalCabin") {
        CrewModuleStrategy().apply(module);
    } else if (type == "BiologyLab" || type == "PhysicsLab") {
        ResearchModuleStrategy().apply(module);
    } else if (type == "ReactorModule") {
        ReactorModuleStrategy().apply(module);
    }
}

std::string DeepSpace::getName() const {
    return "DeepSpace";
}

double DeepSpace::getEnergyModifier() const {
    return 1.5;
}

double DeepSpace::getDamageModifier() const {
    return 5.0;
}

// AsteroidField
void AsteroidField::applyEffect(Module& module) {
    module - Damage{.amount = static_cast<int>(getDamageModifier()), .type = "Physical"};

    if (const std::string type = module.getType(); type == "BiologyLab" || type == "PhysicsLab") {
        ResearchModuleStrategy().apply(module);
    } else if (type == "CrewCabin" || type == "MedicalCabin") {
        CrewModuleStrategy().apply(module);
    } else if (type == "SolarModule") {
        EnergyModuleStrategy().apply(module);
    } else if (type == "ReactorModule") {
        ReactorModuleStrategy().apply(module);
    }
}

std::string AsteroidField::getName() const {
    return "AsteroidField";
}

double AsteroidField::getEnergyModifier() const {
    return 1.2;
}

double AsteroidField::getDamageModifier() const {
    return 20.0;
}

// EnvironmentProxy
EnvironmentProxy::EnvironmentProxy(std::shared_ptr<Environment> realEnvironment)
    : realEnvironment_(std::move(realEnvironment)) {}

void EnvironmentProxy::applyEffect(Module& module) {
    if (!realEnvironment_) {
        return;
    }

    const int moduleId = module.getId();
    const std::string envName = realEnvironment_->getName();

    LoggingProxy::log(LogSource::ENV, "Proxy: запрос применения окружения '" + envName +
                      "' к модулю ID " + std::to_string(moduleId) + " (" + module.getType() + ").");

    if (const auto it = effectCache_.find(moduleId); it != effectCache_.end() && it->second == envName) {
        LoggingProxy::log(LogSource::ENV, "Proxy: эффект окружения '" + envName +
                          "' для модуля ID " + std::to_string(moduleId) + " взят из кеша.");
        return;
    }

    if (const std::string state = module.getStateString(); state == "Ремонт" || state == "Авария") {
        LoggingProxy::log(LogSource::ENV, "Proxy: модуль в состоянии '" + state + "'. Пропуск.");
        return;
    }

    if (const std::string type = module.getType();
        (type == "BiologyLab" || type == "PhysicsLab") && realEnvironment_->getDamageModifier() > 0.0) {
        LoggingProxy::log(LogSource::ENV, "Proxy: исследовательский модуль (" + type +
                          ") защищён прокси от опасного окружения '" + envName + "'.");
        effectCache_[moduleId] = envName;
        return;
    }

    realEnvironment_->applyEffect(module);
    effectCache_[moduleId] = envName;
}

std::string EnvironmentProxy::getName() const {
    return realEnvironment_ ? realEnvironment_->getName() : "None";
}

double EnvironmentProxy::getEnergyModifier() const {
    return realEnvironment_ ? realEnvironment_->getEnergyModifier() : 0.0;
}

double EnvironmentProxy::getDamageModifier() const {
    return realEnvironment_ ? realEnvironment_->getDamageModifier() : 0.0;
}