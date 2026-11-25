#include "../include/command_center.h"
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <utility>

namespace {

size_t utf8Length(const std::string& str) {
    size_t len = 0;
    for (const unsigned char c : str) {
        if ((c & 0xC0) != 0x80) {
            ++len;
        }
    }
    return len;
}

std::string padRight(const std::string& str, const size_t width) {
    const size_t len = utf8Length(str);
    if (len >= width) {
        return str;
    }
    return str + std::string(width - len, ' ');
}

std::string padCenter(const std::string& str, const size_t width) {
    const size_t len = utf8Length(str);
    if (len >= width) {
        return str;
    }
    const size_t left = (width - len) / 2;
    const size_t right = width - len - left;
    return std::string(left, ' ') + str + std::string(right, ' ');
}

std::string repeatStr(const std::string& str, const size_t count) {
    std::string result;
    result.reserve(str.size() * count);
    for (size_t i = 0; i < count; ++i) {
        result += str;
    }
    return result;
}

}

void LoggingProxy::log(const LogSource source, const std::string& message) {
    std::string prefix;
    switch (source) {
        case LogSource::ENV:    prefix = "[ENV]    "; break;
        case LogSource::OBJECT: prefix = "[OBJECT] "; break;
        case LogSource::MODULE: prefix = "[MODULE] "; break;
        case LogSource::CENTER: prefix = "[CENTER] "; break;
    }
    std::cout << prefix << message << std::endl;
}

CommandCenter::CommandCenter(const size_t maxModules, const int energy, const int oxygen, const int safetyLevel)
    : energy_(std::max(0, energy)),
      oxygen_(std::max(0, oxygen)),
      safetyLevel_(std::max(0, safetyLevel)),
      station_(maxModules) {
    LoggingProxy::log(LogSource::CENTER, "Командный центр инициализирован.");
}

CommandCenter::CommandCenter(const CommandCenter& other)
    : energy_(other.energy_),
      oxygen_(other.oxygen_),
      safetyLevel_(other.safetyLevel_),
      station_(other.station_) {
    for (auto& module : station_) {
        module.subscribe(this);
    }
}

CommandCenter& CommandCenter::operator=(const CommandCenter& other) {
    if (this != &other) {
        CommandCenter temp(other);
        std::swap(energy_, temp.energy_);
        std::swap(oxygen_, temp.oxygen_);
        std::swap(safetyLevel_, temp.safetyLevel_);
        std::swap(station_, temp.station_);
        std::swap(currentEnvironment_, temp.currentEnvironment_);
        std::swap(pendingDestroyIds_, temp.pendingDestroyIds_);
        for (auto& module : station_) {
            module.unsubscribe(&temp);
            module.subscribe(this);
        }
    }
    return *this;
}

CommandCenter::CommandCenter(CommandCenter&& other) noexcept
    : energy_(other.energy_),
      oxygen_(other.oxygen_),
      safetyLevel_(other.safetyLevel_),
      station_(std::move(other.station_)),
      currentEnvironment_(std::move(other.currentEnvironment_)),
      pendingDestroyIds_(std::move(other.pendingDestroyIds_)) {
    for (auto& module : station_) {
        module.unsubscribe(&other);
        module.subscribe(this);
    }
    other.energy_ = 0;
    other.oxygen_ = 0;
    other.safetyLevel_ = 0;
}

CommandCenter& CommandCenter::operator=(CommandCenter&& other) noexcept {
    if (this != &other) {
        energy_ = other.energy_;
        oxygen_ = other.oxygen_;
        safetyLevel_ = other.safetyLevel_;
        station_ = std::move(other.station_);
        currentEnvironment_ = std::move(other.currentEnvironment_);
        pendingDestroyIds_ = std::move(other.pendingDestroyIds_);

        for (auto& module : station_) {
            module.unsubscribe(&other);
            module.subscribe(this);
        }
        other.energy_ = 0;
        other.oxygen_ = 0;
        other.safetyLevel_ = 0;
    }
    return *this;
}

void CommandCenter::addModule(std::unique_ptr<Module> module) {
    if (!module) {
        throw std::invalid_argument("Ошибка: передан пустой модуль.");
    }
    if (station_.getModuleCount() >= station_.getMaxModules()) {
        throw std::length_error("Ошибка: превышен лимит модулей в командном центре.");
    }

    LoggingProxy::log(LogSource::CENTER, "Добавлен модуль типа " + module->getType() +
                      " (ID: " + std::to_string(module->getId()) + ")");
    module->subscribe(this);
    station_.addModule(std::move(module));
}

void CommandCenter::removeModuleById(const int id) {
    if (auto* module = station_.findById(id)) {
        module->unsubscribe(this);
        station_.removeById(id);
    }
}

void CommandCenter::moveModule(const size_t fromIndex, const size_t toIndex) {
    station_.moveModule(fromIndex, toIndex);
}

void CommandCenter::teleportModuleTo(const int moduleId, const size_t targetIndex) {
    const size_t count = station_.getModuleCount();
    if (targetIndex >= count) {
        return;
    }

    for (size_t i = 0; i < count; ++i) {
        if (const auto* module = station_.getModuleAt(i); module && module->getId() == moduleId) {
            if (i != targetIndex) {
                station_.moveModule(i, targetIndex);
            }
            return;
        }
    }
}

void CommandCenter::flushPendingRemovals() {
    for (const int id : pendingDestroyIds_) {
        removeModuleById(id);
    }
    pendingDestroyIds_.clear();
}

Module* CommandCenter::getModuleAt(const size_t index) const {
    return station_.getModuleAt(index);
}

Module* CommandCenter::findModuleById(const int id) {
    return station_.findById(id);
}

size_t CommandCenter::getModuleCount() const {
    return station_.getModuleCount();
}

size_t CommandCenter::getMaxModules() const {
    return station_.getMaxModules();
}

const Station& CommandCenter::getStation() const {
    return station_;
}

bool CommandCenter::spendEnergy(const int amount) {
    if (amount < 0 || energy_ < amount) {
        return false;
    }
    energy_ -= amount;
    return true;
}

void CommandCenter::restoreEnergy(const int amount) {
    if (amount > 0) {
        energy_ += amount;
    }
}

int CommandCenter::getEnergy() const {
    return energy_;
}

int CommandCenter::getOxygen() const {
    return oxygen_;
}

int CommandCenter::getSafetyLevel() const {
    return safetyLevel_;
}

void CommandCenter::setEnvironment(std::shared_ptr<Environment> env) {
    if (env) {
        LoggingProxy::log(LogSource::ENV, "Установка окружения: " + env->getName());
    }
    currentEnvironment_ = std::make_shared<EnvironmentProxy>(std::move(env));
}

std::shared_ptr<EnvironmentProxy> CommandCenter::getEnvironment() const {
    return currentEnvironment_;
}

void CommandCenter::applyEnvironmentEffects() {
    if (!currentEnvironment_) {
        return;
    }
    LoggingProxy::log(LogSource::ENV, "Применение эффектов окружения через EnvironmentProxy (" +
                      currentEnvironment_->getName() + ")");

    std::vector<int> moduleIds;
    moduleIds.reserve(station_.getModuleCount());
    for (const auto& module : station_) {
        moduleIds.push_back(module.getId());
    }

    for (const int id : moduleIds) {
        if (auto* module = station_.findById(id)) {
            if (const std::string type = module->getType();
                (type == "CrewCabin" || type == "MedicalCabin") && module->isActive()) {
                oxygen_ = std::max(0, oxygen_ - 5);
            }
            currentEnvironment_->applyEffect(*module);
        }
    }

    flushPendingRemovals();
}

void CommandCenter::interactWith(ExternalObject& object, const size_t moduleIndex) {
    auto* module = station_.getModuleAt(moduleIndex);
    if (!module) {
        return;
    }
    LoggingProxy::log(LogSource::OBJECT, "Взаимодействие с внешним объектом: " + object.getName());
    object.interact(*this, *module);
    flushPendingRemovals();
}

void CommandCenter::printStatistics(const std::string& title) const {
    const std::string fullTop = "┌" + repeatStr("─", 100) + "┐";
    const std::string fullMid = "├" + repeatStr("─", 100) + "┤";
    const std::string colsTop = "├─────┬──────┬──────────────────────────┬────────────────┬────────────┬─────────┬──────────┬─────────┤";
    const std::string colsMid = "├─────┼──────┼──────────────────────────┼────────────────┼────────────┼─────────┼──────────┼─────────┤";
    const std::string colsBot = "└─────┴──────┴──────────────────────────┴────────────────┴────────────┴─────────┴──────────┴─────────┘";
    const std::string fullBot = "└" + repeatStr("─", 100) + "┘";

    const std::string summary =
            "Энергия: " + std::to_string(energy_) + " у.е." +
            " • Кислород: " + std::to_string(oxygen_) +
            " • Безопасность: " + std::to_string(safetyLevel_) +
            " • Модулей: " + std::to_string(station_.getModuleCount()) + "/" + std::to_string(station_.getMaxModules()) +
            " • Окружение: " + (currentEnvironment_ ? currentEnvironment_->getName() : "Нет");

    std::cout << "\n" << fullTop << "\n";
    std::cout << "│ " << padCenter(title, 98) << " │\n";
    std::cout << fullMid << "\n";
    std::cout << "│ " << padCenter(summary, 98) << " │\n";

    if (station_.getModuleCount() == 0) {
        std::cout << fullMid << "\n";
        std::cout << "│ " << padCenter("Модули на станции отсутствуют", 98) << " │\n";
        std::cout << fullBot << std::endl;
        return;
    }

    std::cout << colsTop << "\n";
    std::cout << "│ " << padRight("#", 3)
              << " │ " << padRight("ID", 4)
              << " │ " << padRight("Название", 24)
              << " │ " << padRight("Тип", 14)
              << " │ " << padRight("Статус", 10)
              << " │ " << padRight("Прочн.", 7)
              << " │ " << padRight("Эффект.", 8)
              << " │ " << padRight("Расход", 7) << " │\n";
    std::cout << colsMid << "\n";

    size_t idx = 0;
    for (const auto& module : station_) {
        std::cout << "│ " << padRight(std::to_string(idx++), 3)
                  << " │ " << padRight(std::to_string(module.getId()), 4)
                  << " │ " << padRight(module.getName(), 24)
                  << " │ " << padRight(module.getType(), 14)
                  << " │ " << padRight(module.getStateString(), 10)
                  << " │ " << padRight(std::to_string(module.getHealth()), 7)
                  << " │ " << padRight(std::to_string(module.getEfficiency()) + "%", 8)
                  << " │ " << padRight(std::to_string(module.getEnergyConsumption()), 7) << " │\n";
    }

    std::cout << colsBot << std::endl;
}

void CommandCenter::printModuleStatus(const size_t index) const {
    const auto* module = station_.getModuleAt(index);
    if (!module) {
        return;
    }

    const std::string top = "┌──────────────────┬──────────────────────────┐";
    const std::string mid = "├──────────────────┼──────────────────────────┤";
    const std::string bot = "└──────────────────┴──────────────────────────┘";

    std::cout << "\n" << top << "\n";
    std::cout << "│ " << padRight("Параметр", 16) << " │ " << padRight("Значение", 24) << " │\n";
    std::cout << mid << "\n";
    std::cout << "│ " << padRight("Индекс", 16) << " │ " << padRight(std::to_string(index), 24) << " │\n";
    std::cout << "│ " << padRight("ID", 16) << " │ " << padRight(std::to_string(module->getId()), 24) << " │\n";
    std::cout << "│ " << padRight("Название", 16) << " │ " << padRight(module->getName(), 24) << " │\n";
    std::cout << "│ " << padRight("Тип", 16) << " │ " << padRight(module->getType(), 24) << " │\n";
    std::cout << "│ " << padRight("Статус", 16) << " │ " << padRight(module->getStateString(), 24) << " │\n";
    std::cout << "│ " << padRight("Прочность", 16) << " │ " << padRight(std::to_string(module->getHealth()) + " / 100", 24) << " │\n";
    std::cout << "│ " << padRight("Эффективность", 16) << " │ " << padRight(std::to_string(module->getEfficiency()) + "%", 24) << " │\n";
    std::cout << "│ " << padRight("Расход энергии", 16) << " │ " << padRight(std::to_string(module->getEnergyConsumption()) + " у.е.", 24) << " │\n";
    std::cout << "│ " << padRight("Позиция", 16) << " │ " << padRight(std::to_string(module->getPosition()), 24) << " │\n";
    std::cout << bot << std::endl;
}

void CommandCenter::onModuleEvent(const ModuleEvent& event) {
    LoggingProxy::log(LogSource::MODULE, "Событие '" + event.type +
                      "' от модуля ID " + std::to_string(event.moduleId));

    if (event.type == "Activated") {
        LoggingProxy::log(LogSource::CENTER, "Модуль ID " + std::to_string(event.moduleId) + " переведён в активный режим.");
    } else if (event.type == "Damaged") {
        safetyLevel_ = std::max(0, safetyLevel_ - 1);
        LoggingProxy::log(LogSource::CENTER, "Модуль ID " + std::to_string(event.moduleId) +
                          " повреждён на " + std::to_string(event.damage) +
                          " ед. Безопасность снижена до " + std::to_string(safetyLevel_) + ".");
    } else if (event.type == "Destroyed") {
        if (station_.findById(event.moduleId) != nullptr &&
            std::ranges::find(pendingDestroyIds_, event.moduleId) == pendingDestroyIds_.end()) {
            LoggingProxy::log(LogSource::CENTER, "Модуль ID " + std::to_string(event.moduleId) +
                              " разрушен и помечен к отстыковке от станции!");
            pendingDestroyIds_.push_back(event.moduleId);
        }
    } else if (event.type == "EnergyConsumed") {
        energy_ = std::max(0, energy_ - event.damage);
        LoggingProxy::log(LogSource::CENTER, "Модуль ID " + std::to_string(event.moduleId) +
                          " потребил " + std::to_string(event.damage) +
                          " ед. энергии. Остаток: " + std::to_string(energy_) + ".");
    }
}

CommandCenter& operator+(CommandCenter& center, const ResourcePackage& package) {
    center.energy_ = std::max(0, center.energy_ + package.energy);
    center.oxygen_ = std::max(0, center.oxygen_ + package.oxygen);
    center.safetyLevel_ = std::max(0, center.safetyLevel_ + package.safetyLevel);
    LoggingProxy::log(LogSource::CENTER, "Получены ресурсы (+Энергия: " + std::to_string(package.energy) +
                      ", +Кислород: " + std::to_string(package.oxygen) +
                      ", +Безопасность: " + std::to_string(package.safetyLevel) + ")");
    return center;
}