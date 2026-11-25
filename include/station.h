#pragma once
#include "module.h"
#include "station_iterator.h"
#include <vector>
#include <memory>

class Station {
private:
    std::vector<std::unique_ptr<Module>> modules_;
    size_t maxModules_;

    void syncPositions();

public:
    explicit Station(size_t maxModules);

    Station(const Station& other);
    Station& operator=(const Station& other);
    Station(Station&& other) noexcept;
    Station& operator=(Station&& other) noexcept;
    ~Station() = default;

    void addModule(std::unique_ptr<Module> module);
    Module* findById(int id);
    [[nodiscard]] const Module* findById(int id) const;
    [[nodiscard]] Module* getModuleAt(size_t index) const;
    [[nodiscard]] size_t getModuleCount() const;
    [[nodiscard]] size_t getMaxModules() const;

    void removeById(int id);
    void removeByIndex(size_t index);
    void moveModule(size_t fromIndex, size_t toIndex);

    void sortByEfficiency();

    [[nodiscard]] StationIterator begin();
    [[nodiscard]] StationIterator end();
    [[nodiscard]] ConstStationIterator begin() const;
    [[nodiscard]] ConstStationIterator end() const;

    [[nodiscard]] StationIterator getIterator();
};