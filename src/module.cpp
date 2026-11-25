#include "../include/module.h"
#include <algorithm>

int Module::nextId_ = 1;

Module::Module(std::string name, const int efficiency, const ModuleState state,
               std::shared_ptr<const ModuleTypeInfo> typeInfo)
    : id(nextId_++),
      name(std::move(name)),
      efficiency(std::clamp(efficiency, 0, 100)),
      state(state),
      durability_(100),
      energyConsumption_(10),
      capacity_(10),
      typeInfo_(typeInfo ? std::move(typeInfo)
                         : std::make_shared<const ModuleTypeInfo>(ModuleTypeInfo{100, 10, this->efficiency, 50})) {
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

bool Module::activate() {
    if (state == ModuleState::Emergency || durability_.getValue() == 0) {
        return false;
    }
    state = ModuleState::Active;
    notify(ModuleEvent{.type = "Activated", .moduleId = id, .damage = 0, .timestamp = std::time(nullptr)});
    return true;
}

void Module::deactivate() {
    if (state != ModuleState::Emergency) {
        state = ModuleState::Inactive;
    }
}

void Module::setState(const ModuleState newState) {
    state = newState;
}

void Module::moveToIndex(const int index) {
    index_ = index;
}

void Module::repair(const int amount) {
    if (amount <= 0) {
        return;
    }
    durability_.repair(amount);
    if ((state == ModuleState::Emergency || state == ModuleState::Repair) && durability_.getValue() > 0) {
        state = ModuleState::Inactive;
    }
}

void Module::setEnergyConsumption(const int amount) {
    energyConsumption_.setValue(amount);
}

void Module::transferEnergyTo(Module& target, const int amount) {
    if (amount <= 0 || this == &target) {
        return;
    }
    energyConsumption_.increase(amount);
    target.energyConsumption_.decrease(amount);
    efficiency = std::clamp(efficiency - amount / 2, 0, 100);
    target.efficiency = std::clamp(target.efficiency + amount / 2, 0, 100);

    notify(ModuleEvent{.type = "EnergyConsumed", .moduleId = id, .damage = amount, .timestamp = std::time(nullptr)});
}

int Module::getId() const {
    return id;
}

const std::string& Module::getName() const {
    return name;
}

int Module::getEfficiency() const {
    return efficiency;
}

int Module::getHealth() const {
    return durability_.getValue();
}

int Module::getEnergyConsumption() const {
    return energyConsumption_.getValue();
}

int Module::getCapacity() const {
    return capacity_.getValue();
}

int Module::getPosition() const {
    return index_;
}

bool Module::isActive() const {
    return state == ModuleState::Active;
}

ModuleState Module::getState() const {
    return state;
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
    if (damage.amount <= 0 || module.durability_.getValue() == 0) {
        return module;
    }

    module.durability_.decrease(damage.amount);
    module.notify(ModuleEvent{.type = "Damaged", .moduleId = module.id, .damage = damage.amount, .timestamp = std::time(nullptr)});

    if (module.durability_.getValue() == 0) {
        module.setState(ModuleState::Emergency);
        module.notify(ModuleEvent{.type = "Destroyed", .moduleId = module.id, .damage = damage.amount, .timestamp = std::time(nullptr)});
    }
    return module;
}

Module& operator*(Module& module, const double modifier) {
    if (modifier < 0.0 || module.state == ModuleState::Emergency) {
        return module;
    }

    module.efficiency = std::clamp(static_cast<int>(module.efficiency * modifier), 0, 100);
    const int newEnergy = static_cast<int>(module.energyConsumption_.getValue() * modifier);
    module.energyConsumption_.setValue(newEnergy);

    module.notify(ModuleEvent{.type = "EnergyConsumed", .moduleId = module.id, .damage = newEnergy, .timestamp = std::time(nullptr)});
    return module;
}