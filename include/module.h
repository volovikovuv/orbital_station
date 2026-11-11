#pragma once
#include "module_attributes.h"
#include <string>
#include <vector>
#include <memory>
#include <ctime>

// Событие модуля для паттерна «Наблюдатель»
struct ModuleEvent {
    std::string type;
    int moduleId{0};
    int damage{0};
    std::time_t timestamp{0};
};

// Интерфейс наблюдателя
class IObserver {
public:
    virtual ~IObserver() = default;

    virtual void onModuleEvent(const ModuleEvent& event) = 0;
};

// Интерфейс наблюдаемого объекта
class IObservable {
public:
    virtual ~IObservable() = default;

    virtual void subscribe(IObserver* observer) = 0;
    virtual void unsubscribe(IObserver* observer) = 0;
    virtual void notify(const ModuleEvent& event) = 0;
};

enum class ModuleState {
    Active,
    Inactive,
    Repair,
    Emergency
};

class Module : public IObservable {
protected:
    static int nextId_;
    int id{0};
    std::string name;
    int efficiency{0};
    ModuleState state;
    int index_{0};

    Durability durability_;
    EnergyConsumption energyConsumption_;
    Capacity capacity_;

    std::shared_ptr<const ModuleTypeInfo> typeInfo_;
    std::vector<IObserver*> observers_;
public:
    Module(std::string name, int efficiency, ModuleState state,
           std::shared_ptr<const ModuleTypeInfo> typeInfo = nullptr);
    Module(const Module& other);
    ~Module() override = default;

    [[nodiscard]] virtual std::unique_ptr<Module> clone() = 0;
    [[nodiscard]] virtual std::string getType() const = 0;

    void subscribe(IObserver* observer) override;
    void unsubscribe(IObserver* observer) override;
    void notify(const ModuleEvent& event) override;

    void activate();
    void deactivate();
    void setState(ModuleState newState);
    void moveToIndex(int index);

    void transferEnergyTo(Module& target, int amount);

    [[nodiscard]] int getEfficiency() const;
    [[nodiscard]] int getId() const;
    [[nodiscard]] std::shared_ptr<const ModuleTypeInfo> getTypeInfo() const;

    [[nodiscard]] std::string getStateString() const;

    friend Module& operator-(Module& module, const Damage& damage);
    friend Module& operator*(Module& module, double modifier);
};

Module& operator-(Module& module, const Damage& damage);
Module& operator*(Module& module, double modifier);