#pragma once
#include <string>

class Durability {
private:
    int value_{0};
public:
    Durability() = default;
    explicit Durability(int value);

    [[nodiscard]] int getValue() const;
    void setValue(int newValue);

    void decrease(int amount);
    void repair(int amount);
};

class EnergyConsumption {
private:
    int value_{0};
public:
    EnergyConsumption() = default;
    explicit EnergyConsumption(int value);

    [[nodiscard]] int getValue() const;
    void setValue(int newValue);

    void increase(int amount);
    void decrease(int amount);
};

class Capacity {
private:
    int value_{0};
public:
    Capacity() = default;
    explicit Capacity(int value);

    [[nodiscard]] int getValue() const;
    void setValue(int newValue);
};

// Структура урона для перегрузки operator-
class Damage {
public:
    int amount{0};
    std::string type;
};

// Паттерн «Легковес»: общие неизменяемые характеристики типа модуля
class ModuleTypeInfo {
public:
    int maxHealth{0};
    int baseEnergyConsumption{0};
    int baseEfficiency{0};
    int cost{0};
};