#pragma once
#include <string>

class StationFacade;

class Command {
protected:
    StationFacade& facade_;

public:
    explicit Command(StationFacade& facade);
    virtual ~Command() = default;

    virtual void execute() = 0;
};

class ActivateModuleCommand : public Command {
private:
    int moduleIndex_;

public:
    ActivateModuleCommand(StationFacade& facade, int moduleIndex);
    void execute() override;
};

class DeactivateModuleCommand : public Command {
private:
    int moduleIndex_;

public:
    DeactivateModuleCommand(StationFacade& facade, int moduleIndex);
    void execute() override;
};

class MoveModuleCommand : public Command {
private:
    int fromIndex_;
    int toIndex_;

public:
    MoveModuleCommand(StationFacade& facade, int fromIndex, int toIndex);
    void execute() override;
};

class RepairModuleCommand : public Command {
private:
    int moduleIndex_;

public:
    RepairModuleCommand(StationFacade& facade, int moduleIndex);
    void execute() override;
};

class DistributeEnergyCommand : public Command {
private:
    int moduleIndex_;
    int percent_;

public:
    DistributeEnergyCommand(StationFacade& facade, int moduleIndex, int percent);
    void execute() override;
};

class StartResearchCommand : public Command {
private:
    int moduleIndex_;

public:
    StartResearchCommand(StationFacade& facade, int moduleIndex);
    void execute() override;
};

class BuildModuleCommand : public Command {
private:
    std::string type_;
    std::string name_;

public:
    BuildModuleCommand(StationFacade& facade,
                       const std::string& type,
                       const std::string& name);
    void execute() override;
};

class StatusCommand : public Command {
private:
    int moduleIndex_;

public:
    explicit StatusCommand(StationFacade& facade, int moduleIndex = -1);
    void execute() override;
};