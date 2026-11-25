#include <iostream>
#include <memory>
#include <string>
#include <windows.h>
#include "../include/command_center.h"
#include "../include/environment.h"
#include "../include/external_object.h"
#include "../include/module_factory.h"
#include "../include/station_facade.h"
#include "../include/station_mediator.h"

namespace {

void exec(StationFacade& facade, const std::string& cmd) {
    std::cout << "\n> " << cmd << std::endl;
    static_cast<void>(facade.processInput(cmd));
}

void runLegacyScenarios() {
    std::cout << "\n=== ПОВТОР СЦЕНАРИЕВ ЛР1-2 ===\n";

    CommandCenter center(5, 500, 300, 10);
    StandardModuleFactory stdFactory;
    AdvancedModuleFactory advFactory;

    center.addModule(stdFactory.createResidentialModule("Жилой отсек Альфа", 80));
    center.addModule(stdFactory.createEnergyModule("Солнечная панель 1", 90));
    center.addModule(advFactory.createResearchModule("Физическая лаборатория", 100));

    for (size_t i = 0; i < center.getModuleCount(); ++i) {
        if (auto* mod = center.getModuleAt(i)) {
            static_cast<void>(mod->activate());
        }
    }

    center.setEnvironment(std::make_shared<EarthOrbit>());
    center.applyEnvironmentEffects();

    center.setEnvironment(std::make_shared<DeepSpace>());
    center.applyEnvironmentEffects();
    center.applyEnvironmentEffects();

    SolarStorm storm(25);
    center.interactWith(storm, 0);

    CargoContainer cargo(120);
    center.interactWith(cargo, 0);

    SpaceAnomaly anomaly(2);
    center.interactWith(anomaly, 1);

    center.printStatistics("Итог сценариев ЛР1-2");
}

void runStationScenarios() {
    std::cout << "\n=== СЦЕНАРИИ УПРАВЛЕНИЯ СТАНЦИЕЙ (ЛР3) ===\n";

    CommandCenter center(4, 500, 100, 10);
    center.setEnvironment(std::make_shared<EarthOrbit>());
    StationMediator mediator(center);
    StationFacade facade(mediator);

    std::cout << "\n--- Сценарий 1 ---";
    exec(facade, "build solar \"Солнечная панель\"");
    exec(facade, "build crew \"Жилой отсек А\"");
    exec(facade, "activate 0");
    exec(facade, "activate 1");
    exec(facade, "status");

    std::cout << "\n--- Сценарий 2 ---\n";
    MeteorShower shower(25);
    center.interactWith(shower, 0);
    exec(facade, "repair 0");
    exec(facade, "energy 1 80");
    exec(facade, "status 1");

    std::cout << "\n--- Сценарий 3 ---";
    exec(facade, "build biology \"Биолаборатория\"");
    exec(facade, "move 2 0");
    exec(facade, "research 0");
    exec(facade, "status");

    std::cout << "\n--- Сценарий 4 ---";
    exec(facade, "build reactor \"Реактор Альфа\"");
    exec(facade, "activate 3");

    const Module* lab = center.getModuleAt(0);
    const Module* crew = center.getModuleAt(2);
    if (lab && crew) {
        static_cast<void>(mediator.requestEnergy(lab->getId(), 30));
        mediator.notifyDamage(crew->getId(), 60);
    }
    exec(facade, "status");

    std::cout << "\n--- Проверка отказов ---";
    exec(facade, "unknown_cmd 0");
    exec(facade, "activate abc");
    exec(facade, "energy 0 150");
    exec(facade, "build physics \"Лишняя лаборатория\"");
}

void runInteractiveLoop() {
    std::cout << "\n=== ИНТЕРАКТИВНЫЙ РЕЖИМ ===\n"
              << "Введите команду (help - справка, exit - выход):" << std::endl;

    CommandCenter center(8, 500, 100, 70);
    center.setEnvironment(std::make_shared<EarthOrbit>());
    StationMediator mediator(center);
    StationFacade facade(mediator);

    std::string input;
    while (!facade.isExitRequested()) {
        std::cout << "> " << std::flush;
        if (!std::getline(std::cin, input)) {
            break;
        }
        static_cast<void>(facade.processInput(input));
    }
}

}

int main() {
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    try {
        runLegacyScenarios();
        runStationScenarios();
        runInteractiveLoop();
    } catch (const std::exception& e) {
        std::cerr << "\nКритическая ошибка: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}