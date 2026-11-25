#include "../include/command.h"
#include "../include/station_facade.h"

Command::Command(StationFacade& facade)
    : facade_(facade) {}

ActivateModuleCommand::ActivateModuleCommand(StationFacade& facade, const int moduleIndex)
    : Command(facade), moduleIndex_(moduleIndex) {}

void ActivateModuleCommand::execute() {
    facade_.activateModule(moduleIndex_);
}

DeactivateModuleCommand::DeactivateModuleCommand(StationFacade& facade, const int moduleIndex)
    : Command(facade), moduleIndex_(moduleIndex) {}

void DeactivateModuleCommand::execute() {
    facade_.deactivateModule(moduleIndex_);
}

MoveModuleCommand::MoveModuleCommand(StationFacade& facade, const int fromIndex, const int toIndex)
    : Command(facade), fromIndex_(fromIndex), toIndex_(toIndex) {}

void MoveModuleCommand::execute() {
    facade_.moveModule(fromIndex_, toIndex_);
}

RepairModuleCommand::RepairModuleCommand(StationFacade& facade, const int moduleIndex)
    : Command(facade), moduleIndex_(moduleIndex) {}

void RepairModuleCommand::execute() {
    facade_.repairModule(moduleIndex_);
}

DistributeEnergyCommand::DistributeEnergyCommand(StationFacade& facade, const int moduleIndex, const int percent)
    : Command(facade), moduleIndex_(moduleIndex), percent_(percent) {}

void DistributeEnergyCommand::execute() {
    facade_.distributeEnergy(moduleIndex_, percent_);
}

StartResearchCommand::StartResearchCommand(StationFacade& facade, const int moduleIndex)
    : Command(facade), moduleIndex_(moduleIndex) {}

void StartResearchCommand::execute() {
    facade_.startResearch(moduleIndex_);
}

BuildModuleCommand::BuildModuleCommand(StationFacade& facade,
                                       const std::string& type,
                                       const std::string& name)
    : Command(facade), type_(type), name_(name) {}

void BuildModuleCommand::execute() {
    facade_.buildModule(type_, name_);
}

StatusCommand::StatusCommand(StationFacade& facade, const int moduleIndex)
    : Command(facade), moduleIndex_(moduleIndex) {}

void StatusCommand::execute() {
    facade_.getStatus(moduleIndex_);
}