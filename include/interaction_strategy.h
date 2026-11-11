#pragma once

class Module;

// Интерфейс паттерна «Стратегия»
class IInteractionStrategy {
public:
    virtual ~IInteractionStrategy() = default;

    virtual void apply(Module& module) = 0;
};

class EnergyModuleStrategy : public IInteractionStrategy {
public:
    void apply(Module& module) override;
};

class CrewModuleStrategy : public IInteractionStrategy {
public:
    void apply(Module& module) override;
};

class ResearchModuleStrategy : public IInteractionStrategy {
public:
    void apply(Module& module) override;
};

class ReactorModuleStrategy : public IInteractionStrategy {
public:
    void apply(Module& module) override;
};