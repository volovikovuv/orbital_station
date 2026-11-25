#pragma once
#include <memory>
#include <string>

class StationFacade;

class Handler {
protected:
    StationFacade& facade_;
    std::unique_ptr<Handler> next_;

    bool passToNext(const std::string& cmd) const;

public:
    explicit Handler(StationFacade& facade);
    virtual ~Handler() = default;

    Handler* setNext(std::unique_ptr<Handler> next);
    virtual bool handle(const std::string& cmd) = 0;
};

class ActivateHandler : public Handler {
public:
    using Handler::Handler;
    bool handle(const std::string& cmd) override;
};

class DeactivateHandler : public Handler {
public:
    using Handler::Handler;
    bool handle(const std::string& cmd) override;
};

class MoveHandler : public Handler {
public:
    using Handler::Handler;
    bool handle(const std::string& cmd) override;
};

class RepairHandler : public Handler {
public:
    using Handler::Handler;
    bool handle(const std::string& cmd) override;
};

class EnergyHandler : public Handler {
public:
    using Handler::Handler;
    bool handle(const std::string& cmd) override;
};

class ResearchHandler : public Handler {
public:
    using Handler::Handler;
    bool handle(const std::string& cmd) override;
};

class BuildHandler : public Handler {
public:
    using Handler::Handler;
    bool handle(const std::string& cmd) override;
};

class StatusHandler : public Handler {
public:
    using Handler::Handler;
    bool handle(const std::string& cmd) override;
};

class HelpHandler : public Handler {
public:
    using Handler::Handler;
    bool handle(const std::string& cmd) override;
};

class ExitHandler : public Handler {
public:
    using Handler::Handler;
    bool handle(const std::string& cmd) override;
};