#include "../include/command_center.h"
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <utility>

void LoggingProxy::log(const LogSource source, const std::string& message) {
    std::string prefix;
    switch (source) {
        case LogSource::ENV:    prefix = "[ENV] ";    break;
        case LogSource::OBJECT: prefix = "[OBJECT] "; break;
        case LogSource::MODULE: prefix = "[MODULE] "; break;
        case LogSource::CENTER: prefix = "[CENTER] "; break;
    }
    std::cout << prefix << message << std::endl;
}

CommandCenter::CommandCenter(const size_t maxModules, const int energy, const int oxygen, const int safetyLevel)
    : energy_(energy), oxygen_(oxygen), safetyLevel_(safetyLevel), maxModules_(maxModules) {
    LoggingProxy::log(LogSource::CENTER, "Командный центр инициализирован.");
}

CommandCenter::CommandCenter(const CommandCenter& other)
    : energy_(other.energy_),
      oxygen_(other.oxygen_),
      safetyLevel_(other.safetyLevel_),
      maxModules_(other.maxModules_),
      currentEnvironment_(other.currentEnvironment_) {
    modules_.reserve(other.modules_.size());
    for (const auto& module : other.modules_) {
        auto cloned = module->clone();
        cloned->subscribe(this);
        modules_.push_back(std::move(cloned));
    }
}

CommandCenter& CommandCenter::operator=(const CommandCenter& other) {
    if (this != &other) {
        CommandCenter temp(other);
        std::swap(energy_, temp.energy_);
        std::swap(oxygen_, temp.oxygen_);
        std::swap(safetyLevel_, temp.safetyLevel_);
        std::swap(maxModules_, temp.maxModules_);
        std::swap(modules_, temp.modules_);
        std::swap(currentEnvironment_, temp.currentEnvironment_);
        for (auto& module : modules_) {
            module->unsubscribe(&temp);
            module->subscribe(this);
        }
    }
    return *this;
}

CommandCenter::CommandCenter(CommandCenter&& other) noexcept
    : energy_(other.energy_),
      oxygen_(other.oxygen_),
      safetyLevel_(other.safetyLevel_),
      maxModules_(other.maxModules_),
      modules_(std::move(other.modules_)),
      currentEnvironment_(std::move(other.currentEnvironment_)) {
    for (auto& module : modules_) {
        module->unsubscribe(&other);
        module->subscribe(this);
    }
    other.energy_ = 0;
    other.oxygen_ = 0;
    other.safetyLevel_ = 0;
    other.maxModules_ = 0;
}

CommandCenter& CommandCenter::operator=(CommandCenter&& other) noexcept {
    if (this != &other) {
        energy_ = other.energy_;
        oxygen_ = other.oxygen_;
        safetyLevel_ = other.safetyLevel_;
        maxModules_ = other.maxModules_;
        modules_ = std::move(other.modules_);
        currentEnvironment_ = std::move(other.currentEnvironment_);

        for (auto& module : modules_) {
            module->unsubscribe(&other);
            module->subscribe(this);
        }
        other.energy_ = 0;
        other.oxygen_ = 0;
        other.safetyLevel_ = 0;
        other.maxModules_ = 0;
    }
    return *this;
}

void CommandCenter::addModule(std::unique_ptr<Module> module) {
    if (!module || modules_.size() >= maxModules_) {
        throw std::length_error("Ошибка: превышен лимит модулей в командном центре.");
    }
    LoggingProxy::log(LogSource::CENTER, "Добавлен модуль типа " + module->getType() +
                      " (ID: " + std::to_string(module->getId()) + ")");
    module->subscribe(this);
    modules_.push_back(std::move(module));
}

void CommandCenter::removeModuleById(const int id) {
    std::erase_if(modules_, [id](const std::unique_ptr<Module>& m) {
        return m->getId() == id;
    });
}

void CommandCenter::teleportModuleTo(const int moduleId, const size_t targetIndex) {
    const auto it = std::ranges::find_if(modules_, [moduleId](const std::unique_ptr<Module>& m) {
        return m->getId() == moduleId;
    });

    if (it == modules_.end()) {
        return;
    }

    if (const auto currentIndex = static_cast<size_t>(std::distance(modules_.begin(), it)); currentIndex == targetIndex || targetIndex >= modules_.size()) {
        return;
    }

    auto moduleToMove = std::move(*it);
    modules_.erase(it);
    modules_.insert(modules_.begin() + static_cast<std::ptrdiff_t>(targetIndex), std::move(moduleToMove));
}

Module* CommandCenter::getModuleAt(const size_t index) const {
    if (index >= modules_.size()) {
        return nullptr;
    }
    return modules_[index].get();
}

size_t CommandCenter::getModuleCount() const {
    return modules_.size();
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

void CommandCenter::applyEnvironmentEffects() {
    if (!currentEnvironment_) {
        return;
    }
    LoggingProxy::log(LogSource::ENV, "Применение эффектов окружения через EnvironmentProxy (" +
                      currentEnvironment_->getName() + ")");

    std::vector<int> moduleIds;
    moduleIds.reserve(modules_.size());
    for (const auto& module : modules_) {
        moduleIds.push_back(module->getId());
    }

    for (const int id : moduleIds) {
        const auto it = std::ranges::find_if(modules_, [id](const std::unique_ptr<Module>& m) {
            return m->getId() == id;
        });
        if (it != modules_.end()) {
            if (const std::string type = (*it)->getType();
                (type == "CrewCabin" || type == "MedicalCabin") && (*it)->getStateString() == "Включён") {
                oxygen_ = std::max(0, oxygen_ - 5);
            }
            currentEnvironment_->applyEffect(*(*it));
        }
    }
}

void CommandCenter::interactWith(ExternalObject& object, const size_t moduleIndex) {
    if (moduleIndex >= modules_.size()) {
        return;
    }
    LoggingProxy::log(LogSource::OBJECT, "Взаимодействие с внешним объектом: " + object.getName());
    object.interact(*this, *modules_[moduleIndex]);
}

void CommandCenter::printStatistics(const std::string& title) const {
    std::cout << "\n=== " << title << " ===" << std::endl;
    std::cout << "Энергия: " << energy_
              << " | Кислород: " << oxygen_
              << " | Безопасность: " << safetyLevel_
              << " | Модулей: " << modules_.size() << "/" << maxModules_
              << " | Окружение: " << (currentEnvironment_ ? currentEnvironment_->getName() : "Нет")
              << std::endl;

    for (const auto& module : modules_) {
        std::cout << "  -> [ID: " << module->getId() << "] "
                  << "Тип: " << module->getType()
                  << " | Эффективность: " << module->getEfficiency()
                  << " | Статус: " << module->getStateString()
                  << std::endl;
    }
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
        LoggingProxy::log(LogSource::CENTER, "Модуль ID " + std::to_string(event.moduleId) +
                          " разрушен и отстыкован от станции!");
        removeModuleById(event.moduleId);
    } else if (event.type == "EnergyConsumed") {
        energy_ = std::max(0, energy_ - event.damage);
        LoggingProxy::log(LogSource::CENTER, "Модуль ID " + std::to_string(event.moduleId) +
                          " потребил " + std::to_string(event.damage) +
                          " ед. энергии. Остаток: " + std::to_string(energy_) + ".");
    }
}

CommandCenter& operator+(CommandCenter& center, const ResourcePackage& package) {
    center.energy_ += package.energy;
    center.oxygen_ += package.oxygen;
    center.safetyLevel_ += package.safetyLevel;
    LoggingProxy::log(LogSource::CENTER, "Получены ресурсы (+Энергия: " + std::to_string(package.energy) +
                      ", +Кислород: " + std::to_string(package.oxygen) +
                      ", +Безопасность: " + std::to_string(package.safetyLevel) + ")");
    return center;
}