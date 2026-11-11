#include "../include/external_object.h"
#include "../include/command_center.h"
#include "../include/interaction_strategy.h"
#include "../include/module.h"
#include <algorithm>
#include <random>

ExternalObject::ExternalObject(std::string type, const int strength)
    : type_(std::move(type)), strength_(strength) {}

std::string ExternalObject::getName() const {
    return type_;
}

// SolarStorm
SolarStorm::SolarStorm(const int strength)
    : ExternalObject("SolarStorm", strength) {}

void SolarStorm::interact(CommandCenter& center, Module& module) {
    LoggingProxy::log(LogSource::OBJECT, "SolarStorm: вспышка радиации и сбой электроники модуля " + module.getType());

    const int effectiveDamage = std::max(1, strength_ - center.getSafetyLevel());
    module - Damage{.amount = effectiveDamage, .type = "Radiation"};
    module * 0.8;

    // Выбор стратегии воздействия солнечной бури по типу модуля
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

std::string SolarStorm::getName() const {
    return type_;
}

// MeteorShower
MeteorShower::MeteorShower(const int strength)
    : ExternalObject("MeteorShower", strength) {}

void MeteorShower::interact(CommandCenter& center, Module& module) {
    LoggingProxy::log(LogSource::OBJECT, "MeteorShower: физический удар метеоритов по модулю " + module.getType());
    const int effectiveDamage = std::max(1, strength_ - center.getSafetyLevel());
    module - Damage{.amount = effectiveDamage, .type = "Physical"};
}

std::string MeteorShower::getName() const {
    return type_;
}

// CargoContainer
CargoContainer::CargoContainer(const int strength)
    : ExternalObject("CargoContainer", strength) {}

void CargoContainer::interact(CommandCenter& center, Module& module) {
    (void)module;
    LoggingProxy::log(LogSource::OBJECT, "CargoContainer: пополнение ресурсов командного центра");
    center + ResourcePackage{.energy = strength_, .oxygen = strength_, .safetyLevel = 1};
}

std::string CargoContainer::getName() const {
    return type_;
}

// SpaceAnomaly
SpaceAnomaly::SpaceAnomaly(const int strength)
    : ExternalObject("SpaceAnomaly", strength) {}

void SpaceAnomaly::interact(CommandCenter& center, Module& module) {
    static std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> dist(0, 2);

    if (const int effect = dist(gen); effect == 0) {
        LoggingProxy::log(LogSource::OBJECT, "SpaceAnomaly: телепортация модуля " + module.getType());
        module.moveToIndex(0);
        center.teleportModuleTo(module.getId(), 0);
        module * 1.0;
    } else if (effect == 1) {
        LoggingProxy::log(LogSource::OBJECT, "SpaceAnomaly: удвоение характеристик модуля " + module.getType());
        module * static_cast<double>(strength_);
    } else {
        LoggingProxy::log(LogSource::OBJECT, "SpaceAnomaly: отключение модуля " + module.getType());
        module.deactivate();
        module * 0.5;
    }
}

std::string SpaceAnomaly::getName() const {
    return type_;
}