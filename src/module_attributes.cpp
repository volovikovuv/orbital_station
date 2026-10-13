#include "../include/module_attributes.h"
#include <algorithm>

// Durability

Durability::Durability(const int value) {
    value_ = std::clamp(value, 0, 100);
}

int Durability::getValue() const {
    return value_;
}

void Durability::setValue(const int newValue) {
    value_ = std::clamp(newValue, 0, 100);
}

void Durability::decrease(const int amount) {
    if (amount > 0) {
        value_ = std::max(0, value_ - amount);
    }
}

void Durability::repair(const int amount) {
    if (amount > 0) {
        value_ = std::min(100, value_ + amount);
    }
}

// EnergyConsumption

EnergyConsumption::EnergyConsumption(const int value) {
    value_ = std::max(0, value);
}

int EnergyConsumption::getValue() const {
    return value_;
}

void EnergyConsumption::setValue(const int newValue) {
    value_ = std::max(0, newValue);
}

void EnergyConsumption::increase(const int amount) {
    if (amount > 0) {
        value_ += amount;
    }
}

void EnergyConsumption::decrease(const int amount) {
    if (amount > 0) {
        value_ = std::max(0, value_ - amount);
    }
}

// Capacity

Capacity::Capacity(const int value) {
    value_ = std::max(0, value);
}

int Capacity::getValue() const {
    return value_;
}

void Capacity::setValue(const int newValue) {
    value_ = std::max(0, newValue);
}