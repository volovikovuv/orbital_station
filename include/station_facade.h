#pragma once
#include "station_mediator.h"
#include "command.h"
#include "command_handler.h"
#include <memory>
#include <string>

class StationFacade {
private:
    StationMediator& mediator_;
    std::unique_ptr<Handler> handlerChain_;
    bool exitRequested_{false};

    void initHandlerChain();

public:
    explicit StationFacade(StationMediator& mediator);
    ~StationFacade() = default;

    bool activateModule(int index);
    bool deactivateModule(int index);
    bool moveModule(int fromIndex, int toIndex);
    bool repairModule(int index);
    bool distributeEnergy(int index, int percent);
    bool startResearch(int index);
    bool buildModule(const std::string& type, const std::string& name);
    bool getStatus(int index = -1) const;

    void executeCommand(std::unique_ptr<Command> cmd);
    bool processInput(const std::string& input);

    void requestExit();
    [[nodiscard]] bool isExitRequested() const;
};