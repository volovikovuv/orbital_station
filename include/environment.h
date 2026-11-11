#pragma once
#include <string>
#include <memory>
#include <unordered_map>

class Module;

// Интерфейс космического окружения
class Environment {
public:
    virtual ~Environment() = default;

    virtual void applyEffect(Module& module) = 0;
    [[nodiscard]] virtual std::string getName() const = 0;
    [[nodiscard]] virtual double getEnergyModifier() const = 0;
    [[nodiscard]] virtual double getDamageModifier() const = 0;
};

class EarthOrbit : public Environment {
public:
    void applyEffect(Module& module) override;
    [[nodiscard]] std::string getName() const override;
    [[nodiscard]] double getEnergyModifier() const override;
    [[nodiscard]] double getDamageModifier() const override;
};

class DeepSpace : public Environment {
public:
    void applyEffect(Module& module) override;
    [[nodiscard]] std::string getName() const override;
    [[nodiscard]] double getEnergyModifier() const override;
    [[nodiscard]] double getDamageModifier() const override;
};

class AsteroidField : public Environment {
public:
    void applyEffect(Module& module) override;
    [[nodiscard]] std::string getName() const override;
    [[nodiscard]] double getEnergyModifier() const override;
    [[nodiscard]] double getDamageModifier() const override;
};

// Паттерн «Прокси» для контроля доступа окружения к модулям и кеширования
class EnvironmentProxy : public Environment {
private:
    std::shared_ptr<Environment> realEnvironment_;
    std::unordered_map<int, std::string> effectCache_;
public:
    explicit EnvironmentProxy(std::shared_ptr<Environment> realEnvironment);

    void applyEffect(Module& module) override;
    [[nodiscard]] std::string getName() const override;
    [[nodiscard]] double getEnergyModifier() const override;
    [[nodiscard]] double getDamageModifier() const override;
};