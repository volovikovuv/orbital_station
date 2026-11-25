#include "../include/command_handler.h"
#include "../include/station_facade.h"
#include "../include/command_center.h"
#include <iostream>
#include <sstream>
#include <utility>

namespace {

bool parseInt(const std::string& token, int& value) {
    try {
        size_t pos = 0;
        const int parsed = std::stoi(token, &pos);
        if (pos != token.size()) {
            return false;
        }
        value = parsed;
        return true;
    } catch (...) {
        return false;
    }
}

bool hasExtraTokens(std::istringstream& iss) {
    std::string extra;
    return static_cast<bool>(iss >> extra);
}

}

Handler::Handler(StationFacade& facade)
    : facade_(facade) {}

Handler* Handler::setNext(std::unique_ptr<Handler> next) {
    next_ = std::move(next);
    return next_.get();
}

bool Handler::passToNext(const std::string& cmd) const {
    return next_ ? next_->handle(cmd) : false;
}

bool ActivateHandler::handle(const std::string& cmd) {
    std::istringstream iss(cmd);
    std::string keyword;
    if (!(iss >> keyword) || keyword != "activate") {
        return passToNext(cmd);
    }

    std::string indexToken;
    int index = 0;
    if (!(iss >> indexToken) || !parseInt(indexToken, index) || hasExtraTokens(iss)) {
        LoggingProxy::log(LogSource::CENTER, "Ошибка аргументов. Использование: activate <индекс>");
        return true;
    }

    facade_.executeCommand(std::make_unique<ActivateModuleCommand>(facade_, index));
    return true;
}

bool DeactivateHandler::handle(const std::string& cmd) {
    std::istringstream iss(cmd);
    std::string keyword;
    if (!(iss >> keyword) || keyword != "deactivate") {
        return passToNext(cmd);
    }

    std::string indexToken;
    int index = 0;
    if (!(iss >> indexToken) || !parseInt(indexToken, index) || hasExtraTokens(iss)) {
        LoggingProxy::log(LogSource::CENTER, "Ошибка аргументов. Использование: deactivate <индекс>");
        return true;
    }

    facade_.executeCommand(std::make_unique<DeactivateModuleCommand>(facade_, index));
    return true;
}

bool MoveHandler::handle(const std::string& cmd) {
    std::istringstream iss(cmd);
    std::string keyword;
    if (!(iss >> keyword) || keyword != "move") {
        return passToNext(cmd);
    }

    std::string fromToken;
    std::string toToken;
    int fromIndex = 0;
    int toIndex = 0;
    if (!(iss >> fromToken >> toToken) ||
        !parseInt(fromToken, fromIndex) ||
        !parseInt(toToken, toIndex) ||
        hasExtraTokens(iss)) {
        LoggingProxy::log(LogSource::CENTER, "Ошибка аргументов. Использование: move <индекс> <новый_индекс>");
        return true;
    }

    facade_.executeCommand(std::make_unique<MoveModuleCommand>(facade_, fromIndex, toIndex));
    return true;
}

bool RepairHandler::handle(const std::string& cmd) {
    std::istringstream iss(cmd);
    std::string keyword;
    if (!(iss >> keyword) || keyword != "repair") {
        return passToNext(cmd);
    }

    std::string indexToken;
    int index = 0;
    if (!(iss >> indexToken) || !parseInt(indexToken, index) || hasExtraTokens(iss)) {
        LoggingProxy::log(LogSource::CENTER, "Ошибка аргументов. Использование: repair <индекс>");
        return true;
    }

    facade_.executeCommand(std::make_unique<RepairModuleCommand>(facade_, index));
    return true;
}

bool EnergyHandler::handle(const std::string& cmd) {
    std::istringstream iss(cmd);
    std::string keyword;
    if (!(iss >> keyword) || keyword != "energy") {
        return passToNext(cmd);
    }

    std::string indexToken;
    std::string percentToken;
    int index = 0;
    int percent = 0;
    if (!(iss >> indexToken >> percentToken) ||
        !parseInt(indexToken, index) ||
        !parseInt(percentToken, percent) ||
        hasExtraTokens(iss)) {
        LoggingProxy::log(LogSource::CENTER, "Ошибка аргументов. Использование: energy <индекс> <процент 0..100>");
        return true;
    }

    facade_.executeCommand(std::make_unique<DistributeEnergyCommand>(facade_, index, percent));
    return true;
}

bool ResearchHandler::handle(const std::string& cmd) {
    std::istringstream iss(cmd);
    std::string keyword;
    if (!(iss >> keyword) || keyword != "research") {
        return passToNext(cmd);
    }

    std::string indexToken;
    int index = 0;
    if (!(iss >> indexToken) || !parseInt(indexToken, index) || hasExtraTokens(iss)) {
        LoggingProxy::log(LogSource::CENTER, "Ошибка аргументов. Использование: research <индекс>");
        return true;
    }

    facade_.executeCommand(std::make_unique<StartResearchCommand>(facade_, index));
    return true;
}

bool BuildHandler::handle(const std::string& cmd) {
    std::istringstream iss(cmd);
    std::string keyword;
    if (!(iss >> keyword) || keyword != "build") {
        return passToNext(cmd);
    }

    std::string type;
    if (!(iss >> type)) {
        LoggingProxy::log(LogSource::CENTER, "Ошибка аргументов. Использование: build <тип> \"<имя>\"");
        return true;
    }

    iss >> std::ws;
    std::string rawName;
    std::getline(iss, rawName);

    while (!rawName.empty() && (rawName.back() == ' ' || rawName.back() == '\t' || rawName.back() == '\r')) {
        rawName.pop_back();
    }

    if (rawName.empty()) {
        LoggingProxy::log(LogSource::CENTER, "Ошибка аргументов: не указано имя модуля. Использование: build <тип> \"<имя>\"");
        return true;
    }

    std::string name;
    if (rawName.front() == '"') {
        if (rawName.size() < 2 || rawName.back() != '"' || rawName.find('"', 1) != rawName.size() - 1) {
            LoggingProxy::log(LogSource::CENTER, "Ошибка аргументов: некорректные кавычки в имени модуля.");
            return true;
        }
        name = rawName.substr(1, rawName.size() - 2);
    } else {
        if (rawName.find('"') != std::string::npos || rawName.find(' ') != std::string::npos) {
            LoggingProxy::log(LogSource::CENTER, "Ошибка аргументов: имя из нескольких слов необходимо заключить в кавычки.");
            return true;
        }
        name = rawName;
    }

    if (name.empty()) {
        LoggingProxy::log(LogSource::CENTER, "Ошибка аргументов: имя модуля не может быть пустым.");
        return true;
    }

    facade_.executeCommand(std::make_unique<BuildModuleCommand>(facade_, type, name));
    return true;
}

bool StatusHandler::handle(const std::string& cmd) {
    std::istringstream iss(cmd);
    std::string keyword;
    if (!(iss >> keyword) || keyword != "status") {
        return passToNext(cmd);
    }

    std::string indexToken;
    if (!(iss >> indexToken)) {
        facade_.executeCommand(std::make_unique<StatusCommand>(facade_, -1));
        return true;
    }

    int index = 0;
    if (!parseInt(indexToken, index) || hasExtraTokens(iss)) {
        LoggingProxy::log(LogSource::CENTER, "Ошибка аргументов. Использование: status [индекс]");
        return true;
    }

    facade_.executeCommand(std::make_unique<StatusCommand>(facade_, index));
    return true;
}

bool HelpHandler::handle(const std::string& cmd) {
    std::istringstream iss(cmd);
    std::string keyword;
    if (!(iss >> keyword) || keyword != "help") {
        return passToNext(cmd);
    }

    if (hasExtraTokens(iss)) {
        LoggingProxy::log(LogSource::CENTER, "Ошибка аргументов. Использование: help");
        return true;
    }

    std::cout << "Доступные команды (индексация модулей с 0):\n"
              << "  build <тип> \"<имя>\"        - построить модуль (типы: crew, medical, solar, reactor, biology, physics)\n"
              << "  activate <индекс>          - включить модуль\n"
              << "  deactivate <индекс>        - выключить модуль\n"
              << "  move <индекс> <новый>      - переместить модуль на новую позицию\n"
              << "  repair <индекс>            - отремонтировать модуль (1 у.е. энергии = 1 ед. прочности)\n"
              << "  energy <индекс> <процент>  - распределить энергию (0..100%)\n"
              << "  research <индекс>          - запустить исследование в лаборатории\n"
              << "  status [индекс]            - показать общую статистику или статус одного модуля\n"
              << "  help                       - показать справку\n"
              << "  exit                       - выход из программы"
              << std::endl;
    return true;
}

bool ExitHandler::handle(const std::string& cmd) {
    std::istringstream iss(cmd);
    std::string keyword;
    if (!(iss >> keyword) || keyword != "exit") {
        return passToNext(cmd);
    }

    if (hasExtraTokens(iss)) {
        LoggingProxy::log(LogSource::CENTER, "Ошибка аргументов. Использование: exit");
        return true;
    }

    facade_.requestExit();
    LoggingProxy::log(LogSource::CENTER, "Завершение работы системы управления станцией.");
    return true;
}