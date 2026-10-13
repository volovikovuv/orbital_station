#include <iostream>
#include <windows.h>
#include "../include/station.h"
#include "../include/module_factory.h"

static void printStation(Station& station, const std::string& title) {
    std::cout << std::endl << title << ":" << std::endl;

    if (station.begin() == station.end()) {
        std::cout << "[Станция пуста]" << std::endl;
    } else {
        for (auto& iter : station) {
            std::cout << "[ID: " << iter.getId() << "] "
                      << "Тип: " << iter.getType()
                      << " | Эффективность: " << iter.getEfficiency()
                      << " | Статус: " << iter.getStateString()
                      << std::endl;
        }
    }
}

static void runScenario1() {
    std::cout << std::endl
              << "--- Сценарий 1: создание станции, добавление модулей через разные фабрики, вывод списка через итератор ---"
              << std::endl;

    Station StationS1(5);

    StandardModuleFactory stdFactory;
    AdvancedModuleFactory advFactory;

    StationS1.addModule(stdFactory.createResidentialModule("Жилой отсек Альфа", 80));
    StationS1.addModule(stdFactory.createEnergyModule("Солнечная панель 1", 90));
    StationS1.addModule(advFactory.createResearchModule("Квантовая лаборатория", 100));

    printStation(StationS1, "Станция после добавления модулей");
}

static void runScenario2() {
    std::cout << std::endl
              << "--- Сценарий 2: копирование станции, изменение модулей в копии, проверка независимости оригинальной станции ---"
              << std::endl;

    Station originalStation(3);
    StandardModuleFactory factory;

    originalStation.addModule(factory.createResidentialModule("Жилой отсек 'Оригинал'", 75));

    Station copiedStation = originalStation;

    if (const auto iter = copiedStation.begin(); iter != copiedStation.end()) {
        iter->setState(ModuleState::Repair);
    }

    printStation(originalStation, "Оригинальная станция");
    printStation(copiedStation, "Скопированная станция");
}

static void runScenario3() {
    std::cout << std::endl
              << "--- Сценарий 3: перемещение станции через конструктор перемещения, проверка отсутствия копирования ---"
              << std::endl;

    Station tempStation(2);
    StandardModuleFactory factory;

    tempStation.addModule(factory.createEnergyModule("Временный генератор", 50));
    tempStation.addModule(factory.createResidentialModule("Жилой отсек 'Оригинал'", 65));

    printStation(tempStation, "Старая станция (до перемещения)");

    Station newStation = std::move(tempStation);

    printStation(tempStation, "Старая станция (после перемещения)");
    printStation(newStation, "Новая станция (после перемещения)");
}

static void runScenario4() {
    std::cout << std::endl
              << "--- Сценарий 4: итерация по модулям, активация, деактивация, смена состояния, перемещение модуля внутри станции ---"
              << std::endl;

    Station StationS4(3);
    StandardModuleFactory factory;

    StationS4.addModule(factory.createResidentialModule("Отсек 1", 80));
    StationS4.addModule(factory.createEnergyModule("Генератор 1", 95));
    StationS4.addModule(factory.createResearchModule("Лаборатория 1", 100));

    printStation(StationS4, "Исходное состояние модулей");

    auto iter = StationS4.begin();

    if (iter != StationS4.end()) {
        iter->activate();
        ++iter;
    }

    if (iter != StationS4.end()) {
        iter->deactivate();
        ++iter;
    }

    if (iter != StationS4.end()) {
        iter->setState(ModuleState::Emergency);
    }

    printStation(StationS4, "После изменения состояний");

    StationS4.moveModule(0, 2);

    printStation(StationS4, "Перемещаем модуль с индекса 0 на индекс 2");
}

static void runScenario5() {
    std::cout << std::endl
              << "--- Сценарий 5: удаление модулей по ID и индексу, проверка ограничения максимального количества ---"
              << std::endl;

    Station station(2);
    StandardModuleFactory factory;

    station.addModule(factory.createResidentialModule("Отсек A", 80));
    station.addModule(factory.createEnergyModule("Генератор B", 90));

    printStation(station, "Станция заполнена максимальным количеством модулей");

    try {
        std::cout << std::endl << "Попытка добавить модуль:" << std::endl;
        station.addModule(factory.createResearchModule("Лаборатория C", 100));
    } catch (const std::exception& e) {
        std::cout << "Перехвачено ожидаемое исключение: " << e.what() << std::endl;
    }

    if (station.begin() != station.end()) {
        const int idToRemove = station.begin()->getId();
        station.removeById(idToRemove);

        printStation(station, "Станция после удаления модуля с ID " + std::to_string(idToRemove));
    }

    station.removeByIndex(0);

    printStation(station, "Станция после удаления модуля с индексом 0");
}

int main() {
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    try {
        runScenario1();
        runScenario2();
        runScenario3();
        runScenario4();
        runScenario5();
    } catch (const std::exception& e) {
        std::cerr << std::endl << "Критическая ошибка выполнения: " << e.what() << std::endl;
    }

    return 0;
}