#pragma once
#include "station.h"
#include "environment.h"
#include "external_object.h"
#include <vector>
#include <memory>
#include <string>

enum class LogSource {
    ENV,
    OBJECT,
    MODULE,
    CENTER
};

class LoggingProxy {
public:
    static void log(LogSource source, const std::string& message);
};

class ResourcePackage {
public:
    int energy{0};
    int oxygen{0};
    int safetyLevel{0};
};

class CommandCenter : public IObserver {
private:
    int energy_{0};
    int oxygen_{0};
    int safetyLevel_{0};
    Station station_;
    std::shared_ptr<EnvironmentProxy> currentEnvironment_;
    std::vector<int> pendingDestroyIds_;

public:
    explicit CommandCenter(size_t maxModules, int energy = 500, int oxygen = 300, int safetyLevel = 10);

    CommandCenter(const CommandCenter& other);
    CommandCenter& operator=(const CommandCenter& other);
    CommandCenter(CommandCenter&& other) noexcept;
    CommandCenter& operator=(CommandCenter&& other) noexcept;
    ~CommandCenter() override = default;

    void addModule(std::unique_ptr<Module> module);
    void removeModuleById(int id);
    void moveModule(size_t fromIndex, size_t toIndex);
    void teleportModuleTo(int moduleId, size_t targetIndex);
    void flushPendingRemovals();

    [[nodiscard]] Module* getModuleAt(size_t index) const;
    [[nodiscard]] Module* findModuleById(int id);
    [[nodiscard]] size_t getModuleCount() const;
    [[nodiscard]] size_t getMaxModules() const;
    [[nodiscard]] const Station& getStation() const;

    bool spendEnergy(int amount);
    void restoreEnergy(int amount);
    [[nodiscard]] int getEnergy() const;
    [[nodiscard]] int getOxygen() const;
    [[nodiscard]] int getSafetyLevel() const;

    void setEnvironment(std::shared_ptr<Environment> env);
    [[nodiscard]] std::shared_ptr<EnvironmentProxy> getEnvironment() const;
    void applyEnvironmentEffects();
    void interactWith(ExternalObject& object, size_t moduleIndex = 0);

    void printStatistics(const std::string& title) const;
    void printModuleStatus(size_t index) const;

    void onModuleEvent(const ModuleEvent& event) override;

    friend CommandCenter& operator+(CommandCenter& center, const ResourcePackage& package);
};

CommandCenter& operator+(CommandCenter& center, const ResourcePackage& package);