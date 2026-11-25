#pragma once
#include <string>
#include <unordered_map>

class CommandCenter;

class StationMediator {
private:
    CommandCenter& center_;
    std::unordered_map<int, int> energyAllocation_;

public:
    explicit StationMediator(CommandCenter& center);
    ~StationMediator() = default;

    bool requestEnergy(int requesterModuleId, int amount);
    void notifyDamage(int moduleId, int amount);
    void notifyActivation(int moduleId, bool isActive);
    void notifyResearchComplete(int moduleId);
    void distributeResources();

    bool activateModule(int index);
    bool deactivateModule(int index);
    bool moveModule(int fromIndex, int toIndex);
    bool repairModule(int index);
    bool distributeEnergy(int index, int percent);
    bool startResearch(int index);
    bool buildModule(const std::string& type, const std::string& name);
    bool getStatus(int index = -1) const;
};