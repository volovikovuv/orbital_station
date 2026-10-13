#pragma once
#include "module_attributes.h"
#include <string>
#include <memory>

enum class ModuleState {
    Active,
    Inactive,
    Repair,
    Emergency
};

class Module {
protected:
    static int nextId_;
    int id{};
    std::string name;
    int efficiency{};
    ModuleState state;
    int index_{};

    Durability durability_;
    EnergyConsumption energyConsumption_;
    Capacity capacity_;
public:
    Module(std::string  name, int efficiency, ModuleState state);
    virtual ~Module() = default;

    virtual std::unique_ptr<Module> clone() = 0;
    virtual std::string getType() = 0;

    void activate();
    void deactivate();
    void setState(ModuleState newState);
    void moveToIndex(int index);

    [[nodiscard]] int getEfficiency() const;
    [[nodiscard]] int getId() const;

    [[nodiscard]] std::string getStateString() const;
};