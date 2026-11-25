#include "../include/station.h"
#include <stdexcept>
#include <algorithm>
#include <utility>

void Station::syncPositions() {
    for (size_t i = 0; i < modules_.size(); ++i) {
        modules_[i]->moveToIndex(static_cast<int>(i));
    }
}

Station::Station(const size_t maxModules) {
    if (maxModules == 0) {
        throw std::invalid_argument("Ошибка: максимальное количество модулей должно быть больше нуля.");
    }
    maxModules_ = maxModules;
}

Station::Station(const Station& other) : maxModules_(other.maxModules_) {
    modules_.reserve(other.modules_.size());
    for (const auto& module : other.modules_) {
        modules_.push_back(module->clone());
    }
    syncPositions();
}

Station& Station::operator=(const Station& other) {
    if (this != &other) {
        Station temp(other);
        std::swap(modules_, temp.modules_);
        std::swap(maxModules_, temp.maxModules_);
    }
    return *this;
}

Station::Station(Station&& other) noexcept
    : modules_(std::move(other.modules_)), maxModules_(other.maxModules_) {
    other.maxModules_ = 0;
}

Station& Station::operator=(Station&& other) noexcept {
    if (this != &other) {
        modules_ = std::move(other.modules_);
        maxModules_ = other.maxModules_;
        other.maxModules_ = 0;
    }
    return *this;
}

void Station::addModule(std::unique_ptr<Module> module) {
    if (!module) {
        throw std::invalid_argument("Ошибка: передан пустой модуль.");
    }

    if (modules_.size() >= maxModules_) {
        throw std::length_error("Ошибка: превышен лимит модулей на станции.");
    }

    modules_.push_back(std::move(module));
    syncPositions();
}

Module* Station::findById(const int id) {
    const auto iter = std::ranges::find_if(modules_,
        [id](const std::unique_ptr<Module>& module) {
            return module->getId() == id;
        });

    return (iter != modules_.end()) ? iter->get() : nullptr;
}

const Module* Station::findById(const int id) const {
    const auto iter = std::ranges::find_if(modules_,
        [id](const std::unique_ptr<Module>& module) {
            return module->getId() == id;
        });

    return (iter != modules_.end()) ? iter->get() : nullptr;
}

Module* Station::getModuleAt(const size_t index) const {
    if (index >= modules_.size()) {
        return nullptr;
    }
    return modules_[index].get();
}

size_t Station::getModuleCount() const {
    return modules_.size();
}

size_t Station::getMaxModules() const {
    return maxModules_;
}

void Station::removeById(const int id) {
    const auto iter = std::ranges::find_if(modules_,
        [id](const std::unique_ptr<Module>& module) {
            return module->getId() == id;
        });

    if (iter != modules_.end()) {
        modules_.erase(iter);
        syncPositions();
    }
}

void Station::removeByIndex(const size_t index) {
    if (index >= modules_.size()) {
        throw std::out_of_range("Ошибка: индекс выходит за границы вектора модулей.");
    }

    modules_.erase(modules_.begin() + static_cast<std::ptrdiff_t>(index));
    syncPositions();
}

void Station::moveModule(const size_t fromIndex, const size_t toIndex) {
    if (fromIndex >= modules_.size() || toIndex >= modules_.size()) {
        throw std::out_of_range("Ошибка: индекс выходит за границы вектора.");
    }

    if (fromIndex == toIndex) {
        return;
    }

    auto moduleToMove = std::move(modules_[fromIndex]);
    modules_.erase(modules_.begin() + static_cast<std::ptrdiff_t>(fromIndex));
    modules_.insert(modules_.begin() + static_cast<std::ptrdiff_t>(toIndex), std::move(moduleToMove));
    syncPositions();
}

void Station::sortByEfficiency() {
    std::ranges::sort(modules_.begin(), modules_.end(),
        [](const std::unique_ptr<Module>& a, const std::unique_ptr<Module>& b) {
            return a->getEfficiency() > b->getEfficiency();
        });
    syncPositions();
}

StationIterator Station::begin() {
    return {modules_.begin(), modules_.end()};
}

StationIterator Station::end() {
    return {modules_.end(), modules_.end()};
}

ConstStationIterator Station::begin() const {
    return {modules_.begin(), modules_.end()};
}

ConstStationIterator Station::end() const {
    return {modules_.end(), modules_.end()};
}

StationIterator Station::getIterator() {
    return begin();
}