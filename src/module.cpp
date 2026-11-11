#include "../include/module.h"
#include <algorithm>
#include <vector>

int Module::nextId_ = 1;

Module::Module(std::string name, const int efficiency, const ModuleState state,
               std::shared_ptr<const ModuleTypeInfo> typeInfo)
    : id(nextId_++), name(std::move(name)), efficiency(efficiency), state(state),
      durability_(100), energyConsumption_(10), capacity_(10),
      typeInfo_(typeInfo ? std::move(typeInfo)
                         : std::make_shared<const ModuleTypeInfo>(ModuleTypeInfo{100, 10, efficiency, 50})) {
}

Module::Module(const Module& other)
    : id(nextId_++),
      name(other.name),
      efficiency(other.efficiency),
      state(other.state),
      index_(other.index_),
      durability_(other.durability_),
      energyConsumption_(other.energyConsumption_),
      capacity_(other.capacity_),
      typeInfo_(other.typeInfo_) {
}

void Module::subscribe(IObserver* observer) {
    if (observer && std::ranges::find(observers_, observer) == observers_.end()) {
        observers_.push_back(observer);
    }
}

void Module::unsubscribe(IObserver* observer) {
    std::erase(observers_, observer);
}

void Module::notify(const ModuleEvent& event) {
    for (const auto observersCopy = observers_; auto* observer : observersCopy) {
        if (observer) {
            observer->onModuleEvent(event);
        }
    }
}

void Module::activate() {
    state = ModuleState::Active;
    notify(ModuleEvent{.type = "Activated", .moduleId = id, .damage = 0, .timestamp = std::time(nullptr)});
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

void Module::transferEnergyTo(Module& target, const int amount) {
    if (amount <= 0 || this == &target) {
        return;
    }
    energyConsumption_.increase(amount);
    target.energyConsumption_.decrease(amount);
    efficiency = std::max(0, efficiency - amount / 2);
    target.efficiency += amount / 2;

    notify(ModuleEvent{.type = "EnergyConsumed", .moduleId = id, .damage = amount, .timestamp = std::time(nullptr)});
}

int Module::getEfficiency() const {
    return efficiency;
}

int Module::getId() const {
    return id;
}

std::shared_ptr<const ModuleTypeInfo> Module::getTypeInfo() const {
    return typeInfo_;
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

Module& operator-(Module& module, const Damage& damage) {
    module.durability_.decrease(damage.amount);
    module.notify(ModuleEvent{.type = "Damaged", .moduleId = module.id, .damage = damage.amount, .timestamp = std::time(nullptr)});

    if (module.durability_.getValue() == 0) {
        module.setState(ModuleState::Emergency);
        module.notify(ModuleEvent{.type = "Destroyed", .moduleId = module.id, .damage = damage.amount, .timestamp = std::time(nullptr)});
    }
    return module;
}

Module& operator*(Module& module, const double modifier) {
    module.efficiency = static_cast<int>(module.efficiency * modifier);
    const int newEnergy = static_cast<int>(module.energyConsumption_.getValue() * modifier);
    module.energyConsumption_.setValue(newEnergy);

    module.notify(ModuleEvent{.type = "EnergyConsumed", .moduleId = module.id, .damage = newEnergy, .timestamp = std::time(nullptr)});
    return module;
}