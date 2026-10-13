#include "../include/module.h"

int Module::nextId_ = 1;

Module::Module(std::string name, const int efficiency, const ModuleState state)
    : id(nextId_++), name(std::move(name)), efficiency(efficiency), state(state),
      durability_(0), energyConsumption_(0), capacity_(0) {
}

void Module::activate() {
    state = ModuleState::Active;
}

void Module::deactivate() {
    state = ModuleState::Inactive;
}

void Module::setState(const ModuleState newState) {
    state = newState;
}

void Module::moveToIndex(const int index) {
    index_ = index;
}

int Module::getEfficiency() const {
    return efficiency;
}

int Module::getId() const {
    return id;
}

std::string Module::getStateString() const {
    switch (state) {
        case ModuleState::Active:
            return "Включён";
        case ModuleState::Inactive:
            return "Отключён";
        case ModuleState::Repair:
            return "Ремонт";
        case ModuleState::Emergency:
            return "Авария";
        default:
            return "Неизвестно";
    }
}