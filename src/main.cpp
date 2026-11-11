#include <iostream>
#include <memory>
#include <windows.h>
#include "../include/command_center.h"
#include "../include/module_factory.h"
#include "../include/environment.h"
#include "../include/external_object.h"

static void runScenario1(CommandCenter& center) {
    std::cout << "\n--- Сценарий 1: создание командного центра, добавление модулей, установка EarthOrbit ---" << std::endl;

    StandardModuleFactory stdFactory;
    AdvancedModuleFactory advFactory;

    center.addModule(stdFactory.createResidentialModule("Жилой отсек Альфа", 80));
    center.addModule(stdFactory.createEnergyModule("Солнечная панель 1", 90));
    center.addModule(advFactory.createResearchModule("Физическая лаборатория", 100));

    for (size_t i = 0; i < center.getModuleCount(); ++i) {
        if (auto* mod = center.getModuleAt(i)) {
            mod->activate();
        }
    }

    if (auto* energyMod = center.getModuleAt(1); auto* crewMod = center.getModuleAt(0)) {
        if (energyMod && crewMod) {
            energyMod->transferEnergyTo(*crewMod, 10);
        }
    }

    center.setEnvironment(std::make_shared<EarthOrbit>());
    center.applyEnvironmentEffects();
    center.printStatistics("Статистика после Сценария 1 (EarthOrbit)");
}

static void runScenario2(CommandCenter& center) {
    std::cout << "\n--- Сценарий 2: смена окружения на DeepSpace, работа EnvironmentProxy и уведомления ---" << std::endl;

    center.setEnvironment(std::make_shared<DeepSpace>());
    center.applyEnvironmentEffects();

    std::cout << "\n[Повторный вызов для демонстрации кеширования в EnvironmentProxy]:" << std::endl;
    center.applyEnvironmentEffects();

    center.printStatistics("Статистика после Сценария 2 (DeepSpace)");
}

static void runScenario3(CommandCenter& center) {
    std::cout << "\n--- Сценарий 3: появление SolarStorm, урон через operator-, уведомление CommandCenter ---" << std::endl;

    SolarStorm storm(25);
    center.interactWith(storm, 0);

    center.printStatistics("Статистика после Сценария 3 (SolarStorm)");
}

static void runScenario4(CommandCenter& center) {
    std::cout << "\n--- Сценарий 4: появление CargoContainer, пополнение ресурсов через operator+ ---" << std::endl;

    CargoContainer cargo(120);
    center.interactWith(cargo, 0);

    center.printStatistics("Статистика после Сценария 4 (CargoContainer)");
}

static void runScenario5(CommandCenter& center) {
    std::cout << "\n--- Сценарий 5: появление SpaceAnomaly, случайный эффект через operator* ---" << std::endl;

    SpaceAnomaly anomaly(2);
    center.interactWith(anomaly, 1);

    center.printStatistics("Статистика после Сценария 5 (SpaceAnomaly)");
}

int main() {
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    try {
        CommandCenter stationCenter(5, 500, 300, 10);

        runScenario1(stationCenter);
        runScenario2(stationCenter);
        runScenario3(stationCenter);
        runScenario4(stationCenter);
        runScenario5(stationCenter);
    } catch (const std::exception& e) {
        std::cerr << "\nКритическая ошибка выполнения: " << e.what() << std::endl;
    }

    return 0;
}