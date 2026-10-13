#pragma once

class Durability {
private:
    int value_;
public:
    Durability() = default;
    explicit Durability(int value);

    [[nodiscard]] int getValue() const;
    void setValue(int newValue);

    void decrease(int amount);
    void repair(int amount);
};

class EnergyConsumption {
private:
    int value_;
public:
    EnergyConsumption() = default;
    explicit EnergyConsumption(int value);

    [[nodiscard]] int getValue() const;
    void setValue(int newValue);

    void increase(int amount);
    void decrease(int amount);
};

class Capacity {
private:
    int value_;
public:
    Capacity() = default;
    explicit Capacity(int value);

    [[nodiscard]] int getValue() const;
    void setValue(int newValue);
};