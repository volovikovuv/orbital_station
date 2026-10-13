#pragma once
#include "module.h"
#include "station_iterator.h"
#include <vector>
#include <memory>

class Station {
private:
    std::vector<std::unique_ptr<Module>> modules_;
    size_t maxModules_;
public:
    explicit Station(size_t maxModules);

    Station(const Station& other); // конструктор копирования
    Station& operator=(const Station& other); // оператор присваивания копированием
    Station(Station&& other) noexcept ; // конструктор перемещения
    Station& operator=(Station&& other) noexcept ; // оператор присваивания перемещением
    ~Station() = default;

    void addModule(std::unique_ptr<Module> module);
    Module* findById(int id);
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