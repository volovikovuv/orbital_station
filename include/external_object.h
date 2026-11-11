#pragma once
#include <string>

class CommandCenter;
class Module;

// Абстрактный класс внешнего объекта / космической аномалии
class ExternalObject {
protected:
    std::string type_;
    int strength_{0};
public:
    ExternalObject(std::string type, int strength);
    virtual ~ExternalObject() = default;

    virtual void interact(CommandCenter& center, Module& module) = 0;
    [[nodiscard]] virtual std::string getName() const = 0;
};

class SolarStorm : public ExternalObject {
public:
    explicit SolarStorm(int strength = 15);

    void interact(CommandCenter& center, Module& module) override;
    [[nodiscard]] std::string getName() const override;
};

class MeteorShower : public ExternalObject {
public:
    explicit MeteorShower(int strength = 25);

    void interact(CommandCenter& center, Module& module) override;
    [[nodiscard]] std::string getName() const override;
};

class CargoContainer : public ExternalObject {
public:
    explicit CargoContainer(int strength = 50);

    void interact(CommandCenter& center, Module& module) override;
    [[nodiscard]] std::string getName() const override;
};

class SpaceAnomaly : public ExternalObject {
public:
    explicit SpaceAnomaly(int strength = 2);

    void interact(CommandCenter& center, Module& module) override;
    [[nodiscard]] std::string getName() const override;
};