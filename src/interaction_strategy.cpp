#include "../include/interaction_strategy.h"
#include "../include/module.h"

void EnergyModuleStrategy::apply(Module& module) {
    module * 0.9;
}

void CrewModuleStrategy::apply(Module& module) {
    module - Damage{.amount = 10, .type = "Physical"};
}

void ResearchModuleStrategy::apply(Module& module) {
    module.setState(ModuleState::Inactive);
}

void ReactorModuleStrategy::apply(Module& module) {
    module - Damage{.amount = 15, .type = "Radiation"};
}