#pragma once
#include "module.h"

// Промежуточные классы

class ResidentialModule : public Module {
public:
    using Module::Module;
};

class EnergyModule : public Module {
public:
    using Module::Module;
};

class ResearchModule : public Module {
public:
    using Module::Module;
};

// Конкретные классы

class CrewCabin : public ResidentialModule {
public:
    using ResidentialModule::ResidentialModule;

    [[nodiscard]] std::unique_ptr<Module> clone() override;
    [[nodiscard]] std::string getType() override;
};

class MedicalCabin : public ResidentialModule {
public:
    using ResidentialModule::ResidentialModule;

    [[nodiscard]] std::unique_ptr<Module> clone() override;
    [[nodiscard]] std::string getType() override;
};

class SolarModule : public EnergyModule {
public:
    using EnergyModule::EnergyModule;

    [[nodiscard]] std::unique_ptr<Module> clone() override;
    [[nodiscard]] std::string getType() override;
};

class ReactorModule : public EnergyModule {
public:
    using EnergyModule::EnergyModule;

    [[nodiscard]] std::unique_ptr<Module> clone() override;
    [[nodiscard]] std::string getType() override;
};

class BiologyLab : public ResearchModule {
public:
    using ResearchModule::ResearchModule;

    [[nodiscard]] std::unique_ptr<Module> clone() override;
    [[nodiscard]] std::string getType() override;
};

class PhysicsLab : public ResearchModule {
public:
    using ResearchModule::ResearchModule;

    [[nodiscard]] std::unique_ptr<Module> clone() override;
    [[nodiscard]] std::string getType() override;
};