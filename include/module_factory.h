#pragma once
#include "module.h"
#include <memory>
#include <string>

class ModuleFactory {
public:
    virtual ~ModuleFactory() = default;

    [[nodiscard]] virtual std::unique_ptr<Module> createResidentialModule(const std::string& name, int efficiency) = 0;
    [[nodiscard]] virtual std::unique_ptr<Module> createEnergyModule(const std::string& name, int efficiency) = 0;
    [[nodiscard]] virtual std::unique_ptr<Module> createResearchModule(const std::string& name, int efficiency) = 0;

    [[nodiscard]] static std::unique_ptr<Module> createModule(const std::string& type, const std::string& name);
};

class StandardModuleFactory : public ModuleFactory {
public:
    [[nodiscard]] std::unique_ptr<Module> createResidentialModule(const std::string& name, int efficiency) override;
    [[nodiscard]] std::unique_ptr<Module> createEnergyModule(const std::string& name, int efficiency) override;
    [[nodiscard]] std::unique_ptr<Module> createResearchModule(const std::string& name, int efficiency) override;
};

class AdvancedModuleFactory : public ModuleFactory {
public:
    [[nodiscard]] std::unique_ptr<Module> createResidentialModule(const std::string& name, int efficiency) override;
    [[nodiscard]] std::unique_ptr<Module> createEnergyModule(const std::string& name, int efficiency) override;
    [[nodiscard]] std::unique_ptr<Module> createResearchModule(const std::string& name, int efficiency) override;
};