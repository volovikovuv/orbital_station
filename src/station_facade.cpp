#include "../include/station_facade.h"
#include "../include/command_center.h"
#include <utility>

void StationFacade::initHandlerChain() {
    handlerChain_ = std::make_unique<ActivateHandler>(*this);
    Handler* current = handlerChain_.get();

    current = current->setNext(std::make_unique<DeactivateHandler>(*this));
    current = current->setNext(std::make_unique<MoveHandler>(*this));
    current = current->setNext(std::make_unique<RepairHandler>(*this));
    current = current->setNext(std::make_unique<EnergyHandler>(*this));
    current = current->setNext(std::make_unique<ResearchHandler>(*this));
    current = current->setNext(std::make_unique<BuildHandler>(*this));
    current = current->setNext(std::make_unique<StatusHandler>(*this));
    current = current->setNext(std::make_unique<HelpHandler>(*this));
    current->setNext(std::make_unique<ExitHandler>(*this));
}

StationFacade::StationFacade(StationMediator& mediator)
    : mediator_(mediator) {
    initHandlerChain();
}

bool StationFacade::activateModule(const int index) {
    return mediator_.activateModule(index);
}

bool StationFacade::deactivateModule(const int index) {
    return mediator_.deactivateModule(index);
}

bool StationFacade::moveModule(const int fromIndex, const int toIndex) {
    return mediator_.moveModule(fromIndex, toIndex);
}

bool StationFacade::repairModule(const int index) {
    return mediator_.repairModule(index);
}

bool StationFacade::distributeEnergy(const int index, const int percent) {
    return mediator_.distributeEnergy(index, percent);
}

bool StationFacade::startResearch(const int index) {
    return mediator_.startResearch(index);
}

bool StationFacade::buildModule(const std::string& type, const std::string& name) {
    return mediator_.buildModule(type, name);
}

bool StationFacade::getStatus(const int index) const {
    return mediator_.getStatus(index);
}

void StationFacade::executeCommand(std::unique_ptr<Command> cmd) {
    if (cmd) {
        cmd->execute();
    }
}

bool StationFacade::processInput(const std::string& input) {
    if (input.find_first_not_of(" \t\r\n") == std::string::npos) {
        return false;
    }

    if (handlerChain_ && handlerChain_->handle(input)) {
        return true;
    }

    LoggingProxy::log(LogSource::CENTER, "Ошибка: неизвестная команда '" + input +
                      "'. Введите 'help' для просмотра списка доступных команд.");
    return false;
}

void StationFacade::requestExit() {
    exitRequested_ = true;
}

bool StationFacade::isExitRequested() const {
    return exitRequested_;
}