#include "../include/station_mediator.h"
#include "../include/command_center.h"
#include "../include/module_factory.h"
#include <algorithm>

namespace {

Module* getValidModule(CommandCenter& center, const int index) {
    if (index < 0 || static_cast<size_t>(index) >= center.getModuleCount()) {
        LoggingProxy::log(LogSource::CENTER, "Ошибка: некорректный индекс модуля (" + std::to_string(index) + ").");
        return nullptr;
    }
    return center.getModuleAt(static_cast<size_t>(index));
}

bool isEnergyModuleType(const std::string& type) {
    return type == "ReactorModule" || type == "SolarModule";
}

bool isResearchModuleType(const std::string& type) {
    return type == "BiologyLab" || type == "PhysicsLab";
}

}

StationMediator::StationMediator(CommandCenter& center)
    : center_(center) {}

bool StationMediator::requestEnergy(const int requesterModuleId, const int amount) {
    if (amount <= 0) {
        LoggingProxy::log(LogSource::CENTER, "Посредник: объём запрашиваемой энергии должен быть больше нуля.");
        return false;
    }

    Module* requester = center_.findModuleById(requesterModuleId);
    if (!requester) {
        LoggingProxy::log(LogSource::CENTER, "Посредник: модуль-заявитель ID " + std::to_string(requesterModuleId) + " не найден.");
        return false;
    }
    if (requester->getState() == ModuleState::Emergency) {
        LoggingProxy::log(LogSource::CENTER, "Посредник: аварийный модуль '" + requester->getName() + "' не может запрашивать энергию.");
        return false;
    }

    const Module* provider = nullptr;
    for (const auto& mod : center_.getStation()) {
        if (mod.getId() == requesterModuleId || !mod.isActive()) {
            continue;
        }
        if (mod.getType() == "ReactorModule") {
            provider = &mod;
            break;
        }
        if (mod.getType() == "SolarModule" && !provider) {
            provider = &mod;
        }
    }

    if (!provider) {
        LoggingProxy::log(LogSource::CENTER, "Посредник: нет активного энергетического модуля для выдачи энергии модулю '" +
                          requester->getName() + "'.");
        return false;
    }

    if (!center_.spendEnergy(amount)) {
        LoggingProxy::log(LogSource::CENTER, "Посредник: недостаточно энергии в запасе станции (требуется " +
                          std::to_string(amount) + " у.е., доступно " + std::to_string(center_.getEnergy()) + " у.е.).");
        return false;
    }

    LoggingProxy::log(LogSource::CENTER, "Посредник: модуль '" + requester->getName() + "' (ID: " +
                      std::to_string(requester->getId()) + ") получил " + std::to_string(amount) +
                      " у.е. энергии от '" + provider->getName() + "' (" + provider->getType() +
                      ", ID: " + std::to_string(provider->getId()) + "). Остаток запаса: " +
                      std::to_string(center_.getEnergy()) + " у.е.");
    return true;
}

void StationMediator::notifyDamage(const int moduleId, const int amount) {
    if (amount <= 0) {
        LoggingProxy::log(LogSource::CENTER, "Посредник: величина урона должна быть больше нуля.");
        return;
    }

    Module* module = center_.findModuleById(moduleId);
    if (!module) {
        LoggingProxy::log(LogSource::CENTER, "Посредник: модуль ID " + std::to_string(moduleId) + " не найден для обработки повреждения.");
        return;
    }

    LoggingProxy::log(LogSource::CENTER, "Посредник: получено уведомление о повреждении модуля '" +
                      module->getName() + "' (ID: " + std::to_string(moduleId) + ") на " +
                      std::to_string(amount) + " ед.");

    static_cast<void>(*module - Damage{.amount = amount, .type = "Physical"});

    if (module->getState() == ModuleState::Emergency) {
        center_.flushPendingRemovals();
        LoggingProxy::log(LogSource::CENTER, "Посредник: аварийный модуль ID " + std::to_string(moduleId) +
                          " отстыкован, запуск перераспределения ресурсов.");
        distributeResources();
        return;
    }

    if (module->getHealth() < 50) {
        LoggingProxy::log(LogSource::CENTER, "Посредник: критическая прочность модуля '" + module->getName() +
                          "' (" + std::to_string(module->getHealth()) +
                          "/100). Запуск автоматического перераспределения энергии.");
        distributeResources();
    }
}

void StationMediator::notifyActivation(const int moduleId, const bool isActive) {
    const Module* module = center_.findModuleById(moduleId);
    if (!module) {
        return;
    }

    if (isActive) {
        std::string extra;
        if (isEnergyModuleType(module->getType())) {
            extra = " (источник питания в сети)";
        }
        LoggingProxy::log(LogSource::CENTER, "Посредник: модуль '" + module->getName() + "' (ID: " +
                          std::to_string(moduleId) + ") активирован" + extra + ".");
    } else {
        LoggingProxy::log(LogSource::CENTER, "Посредник: модуль '" + module->getName() + "' (ID: " +
                          std::to_string(moduleId) + ") деактивирован.");
    }
}

void StationMediator::notifyResearchComplete(const int moduleId) {
    const Module* module = center_.findModuleById(moduleId);
    if (!module) {
        return;
    }

    int score = module->getEfficiency();
    std::string envName = "Нет";
    if (const auto env = center_.getEnvironment()) {
        envName = env->getName();
        if (const int hazardPenalty = static_cast<int>(env->getDamageModifier()); hazardPenalty > 0) {
            score = std::max(0, score - hazardPenalty);
        }
    }

    LoggingProxy::log(LogSource::CENTER, "Посредник: исследование в '" + module->getName() + "' (ID: " +
                      std::to_string(moduleId) + ") завершено. Результат: " + std::to_string(score) +
                      "% (окружение: " + envName + ").");
}

void StationMediator::distributeResources() {
    int activeCount = 0;
    for (const auto& mod : center_.getStation()) {
        if (mod.isActive() && mod.getState() != ModuleState::Emergency) {
            ++activeCount;
        }
    }

    if (activeCount == 0) {
        energyAllocation_.clear();
        LoggingProxy::log(LogSource::CENTER, "Посредник: нет активных модулей для автоматического распределения энергии.");
        return;
    }

    const int sharePercent = 100 / activeCount;
    energyAllocation_.clear();

    const size_t totalModules = center_.getModuleCount();
    for (size_t i = 0; i < totalModules; ++i) {
        Module* mod = center_.getModuleAt(i);
        if (!mod || !mod->isActive() || mod->getState() == ModuleState::Emergency) {
            continue;
        }

        const int baseConsumption = mod->getTypeInfo() ? mod->getTypeInfo()->baseEnergyConsumption : 10;
        int targetConsumption = (baseConsumption * sharePercent + 50) / 100;
        if (sharePercent > 0 && targetConsumption == 0) {
            targetConsumption = 1;
        }

        const int delta = targetConsumption - mod->getEnergyConsumption();
        if (delta > 0) {
            if (!center_.spendEnergy(delta)) {
                continue;
            }
        } else if (delta < 0) {
            center_.restoreEnergy(-delta);
        }

        mod->setEnergyConsumption(targetConsumption);
        energyAllocation_[mod->getId()] = sharePercent;
    }

    LoggingProxy::log(LogSource::CENTER, "Посредник: энергия автоматически распределена между " +
                      std::to_string(activeCount) + " активными модулями (по " +
                      std::to_string(sharePercent) + "%). Запас станции: " +
                      std::to_string(center_.getEnergy()) + " у.е.");
}

bool StationMediator::activateModule(const int index) {
    Module* module = getValidModule(center_, index);
    if (!module) {
        return false;
    }

    if (module->isActive()) {
        LoggingProxy::log(LogSource::CENTER, "Модуль '" + module->getName() + "' уже активен.");
        return true;
    }

    if (!module->activate()) {
        LoggingProxy::log(LogSource::CENTER, "Ошибка: нельзя активировать аварийный модуль '" +
                          module->getName() + "' (прочность: " + std::to_string(module->getHealth()) +
                          ") до проведения ремонта.");
        return false;
    }

    notifyActivation(module->getId(), true);
    return true;
}

bool StationMediator::deactivateModule(const int index) {
    Module* module = getValidModule(center_, index);
    if (!module) {
        return false;
    }

    if (module->getState() == ModuleState::Emergency) {
        LoggingProxy::log(LogSource::CENTER, "Ошибка: модуль '" + module->getName() + "' находится в аварийном состоянии.");
        return false;
    }

    if (!module->isActive()) {
        LoggingProxy::log(LogSource::CENTER, "Модуль '" + module->getName() + "' уже отключён.");
        return true;
    }

    module->deactivate();
    notifyActivation(module->getId(), false);
    return true;
}

bool StationMediator::moveModule(const int fromIndex, const int toIndex) {
    const size_t count = center_.getModuleCount();
    if (fromIndex < 0 || toIndex < 0 ||
        static_cast<size_t>(fromIndex) >= count ||
        static_cast<size_t>(toIndex) >= count) {
        LoggingProxy::log(LogSource::CENTER, "Ошибка: некорректные индексы перемещения (" +
                          std::to_string(fromIndex) + " -> " + std::to_string(toIndex) + ").");
        return false;
    }

    if (fromIndex == toIndex) {
        LoggingProxy::log(LogSource::CENTER, "Модуль уже находится на позиции " + std::to_string(toIndex) + ".");
        return true;
    }

    const int movedId = center_.getModuleAt(static_cast<size_t>(fromIndex))->getId();
    center_.moveModule(static_cast<size_t>(fromIndex), static_cast<size_t>(toIndex));

    LoggingProxy::log(LogSource::CENTER, "Модуль ID " + std::to_string(movedId) + " перемещён с позиции " +
                      std::to_string(fromIndex) + " на позицию " + std::to_string(toIndex) + ".");
    return true;
}

bool StationMediator::repairModule(const int index) {
    Module* module = getValidModule(center_, index);
    if (!module) {
        return false;
    }

    const int missingHealth = 100 - module->getHealth();
    if (missingHealth <= 0) {
        LoggingProxy::log(LogSource::CENTER, "Модуль '" + module->getName() + "' не нуждается в ремонте (прочность 100).");
        return false;
    }

    if (!center_.spendEnergy(missingHealth)) {
        LoggingProxy::log(LogSource::CENTER, "Ошибка: недостаточно энергии для ремонта модуля '" +
                          module->getName() + "' (требуется " + std::to_string(missingHealth) +
                          " у.е., доступно " + std::to_string(center_.getEnergy()) + " у.е.).");
        return false;
    }

    module->repair(missingHealth);
    LoggingProxy::log(LogSource::CENTER, "Модуль '" + module->getName() + "' отремонтирован (прочность: " +
                      std::to_string(module->getHealth()) + ", затрачено энергии: " +
                      std::to_string(missingHealth) + " у.е., остаток: " +
                      std::to_string(center_.getEnergy()) + " у.е.).");
    return true;
}

bool StationMediator::distributeEnergy(const int index, const int percent) {
    Module* module = getValidModule(center_, index);
    if (!module) {
        return false;
    }

    if (percent < 0 || percent > 100) {
        LoggingProxy::log(LogSource::CENTER, "Ошибка: процент энергии должен быть в диапазоне от 0 до 100.");
        return false;
    }

    if (module->getState() == ModuleState::Emergency) {
        LoggingProxy::log(LogSource::CENTER, "Ошибка: нельзя распределять энергию на аварийный модуль '" +
                          module->getName() + "'.");
        return false;
    }

    int allocatedByOthers = 0;
    for (const auto& [id, allocatedPercent] : energyAllocation_) {
        if (id != module->getId() && center_.findModuleById(id) != nullptr) {
            allocatedByOthers += allocatedPercent;
        }
    }

    if (allocatedByOthers + percent > 100) {
        LoggingProxy::log(LogSource::CENTER, "Ошибка: суммарное распределение энергии превышает 100% (доступно " +
                          std::to_string(100 - allocatedByOthers) + "%).");
        return false;
    }

    const int baseConsumption = module->getTypeInfo() ? module->getTypeInfo()->baseEnergyConsumption : 10;
    int targetConsumption = (baseConsumption * percent + 50) / 100;
    if (percent > 0 && targetConsumption == 0) {
        targetConsumption = 1;
    }

    const int delta = targetConsumption - module->getEnergyConsumption();
    if (delta > 0) {
        if (!center_.spendEnergy(delta)) {
            LoggingProxy::log(LogSource::CENTER, "Ошибка: недостаточно энергии для увеличения питания модуля '" +
                              module->getName() + "' (требуется " + std::to_string(delta) + " у.е.).");
            return false;
        }
    } else if (delta < 0) {
        center_.restoreEnergy(-delta);
    }

    module->setEnergyConsumption(targetConsumption);
    energyAllocation_[module->getId()] = percent;

    LoggingProxy::log(LogSource::CENTER, "Модулю '" + module->getName() + "' выделено " +
                      std::to_string(percent) + "% энергии (расход: " + std::to_string(targetConsumption) +
                      " у.е., запас станции: " + std::to_string(center_.getEnergy()) + " у.е.).");
    return true;
}

bool StationMediator::startResearch(const int index) {
    Module* module = getValidModule(center_, index);
    if (!module) {
        return false;
    }

    if (!isResearchModuleType(module->getType())) {
        LoggingProxy::log(LogSource::CENTER, "Ошибка: модуль '" + module->getName() +
                          "' (" + module->getType() + ") не является исследовательским.");
        return false;
    }

    if (module->getState() == ModuleState::Emergency) {
        LoggingProxy::log(LogSource::CENTER, "Ошибка: аварийный исследовательский модуль '" +
                          module->getName() + "' требует ремонта перед запуском исследования.");
        return false;
    }

    const int researchCost = std::max(10, module->getEnergyConsumption());

    bool hasEnergyProvider = false;
    for (const auto& mod : center_.getStation()) {
        if (mod.isActive() && isEnergyModuleType(mod.getType())) {
            hasEnergyProvider = true;
            break;
        }
    }

    const bool energyGranted = hasEnergyProvider
                                   ? requestEnergy(module->getId(), researchCost)
                                   : center_.spendEnergy(researchCost);

    if (!energyGranted) {
        LoggingProxy::log(LogSource::CENTER, "Ошибка: недостаточно энергии для запуска исследования в '" +
                          module->getName() + "'.");
        return false;
    }

    if (!module->isActive()) {
        static_cast<void>(module->activate());
        notifyActivation(module->getId(), true);
    }

    notifyResearchComplete(module->getId());
    return true;
}

bool StationMediator::buildModule(const std::string& type, const std::string& name) {
    if (name.empty()) {
        LoggingProxy::log(LogSource::CENTER, "Ошибка: имя нового модуля не может быть пустым.");
        return false;
    }

    if (center_.getModuleCount() >= center_.getMaxModules()) {
        LoggingProxy::log(LogSource::CENTER, "Ошибка: превышен лимит модулей на станции (" +
                          std::to_string(center_.getMaxModules()) + ").");
        return false;
    }

    auto module = ModuleFactory::createModule(type, name);
    if (!module) {
        LoggingProxy::log(LogSource::CENTER, "Ошибка: неизвестный тип модуля '" + type + "'.");
        return false;
    }

    const int buildCost = module->getTypeInfo() ? module->getTypeInfo()->cost : 50;
    if (!center_.spendEnergy(buildCost)) {
        LoggingProxy::log(LogSource::CENTER, "Ошибка: недостаточно энергии для строительства модуля '" +
                          name + "' (требуется " + std::to_string(buildCost) +
                          " у.е., доступно " + std::to_string(center_.getEnergy()) + " у.е.).");
        return false;
    }

    center_.addModule(std::move(module));
    LoggingProxy::log(LogSource::CENTER, "Строительство модуля '" + name + "' завершено (затрачено " +
                      std::to_string(buildCost) + " у.е., остаток энергии: " +
                      std::to_string(center_.getEnergy()) + " у.е.).");
    return true;
}

bool StationMediator::getStatus(const int index) const {
    if (index == -1) {
        center_.printStatistics("Состояние космической станции");
        return true;
    }

    if (index < 0 || static_cast<size_t>(index) >= center_.getModuleCount()) {
        LoggingProxy::log(LogSource::CENTER, "Ошибка: некорректный индекс модуля (" + std::to_string(index) + ").");
        return false;
    }

    center_.printModuleStatus(static_cast<size_t>(index));
    return true;
}