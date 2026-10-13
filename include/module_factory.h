#pragma once
#include "module.h"
#include <memory>
#include <string>

// Абстрактная фабрика
class ModuleFactory {
public:
    virtual ~ModuleFactory() = default;

    [[nodiscard]] virtual std::unique_ptr<Module> createResidentialModule(const std::string& name, int efficiency) = 0;
    [[nodiscard]] virtual std::unique_ptr<Module> createEnergyModule(const std::string& name, int efficiency) = 0;
    [[nodiscard]] virtual std::unique_ptr<Module> createResearchModule(const std::string& name, int efficiency) = 0;
};

// Фабрика стандартных модулей
class StandardModuleFactory : public ModuleFactory {
public:
    [[nodiscard]] std::unique_ptr<Module> createResidentialModule(const std::string& name, int efficiency) override;
    [[nodiscard]] std::unique_ptr<Module> createEnergyModule(const std::string& name, int efficiency) override;
    [[nodiscard]] std::unique_ptr<Module> createResearchModule(const std::string& name, int efficiency) override;
};

// Фабрика улучшенных модулей
class AdvancedModuleFactory : public ModuleFactory {
public:
    [[nodiscard]] std::unique_ptr<Module> createResidentialModule(const std::string& name, int efficiency) override;
    [[nodiscard]] std::unique_ptr<Module> createEnergyModule(const std::string& name, int efficiency) override;
    [[nodiscard]] std::unique_ptr<Module> createResearchModule(const std::string& name, int efficiency) override;
};