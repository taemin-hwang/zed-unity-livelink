#ifndef __SENDER_RUNNER_HDR__
#define __SENDER_RUNNER_HDR__

#include <sl/Fusion.hpp>
#include <thread>
#include <memory>
#include <utility>

#include "config/ConfigParser.h"

class SenderRunner {

public:
    SenderRunner(std::shared_ptr<ConfigParser> config_parser);
    ~SenderRunner();

    SenderRunner(const SenderRunner& other) = delete;
    SenderRunner& operator=(const SenderRunner& other) = delete;

    SenderRunner(SenderRunner&& other) {
        config_parser_ = other.config_parser_;
        zed = std::move(other.zed);
        init_params = other.init_params;
        runner = std::move(other.runner);
        running = other.running;

        other.config_parser_ = nullptr;
        other.running = false;
    }

    SenderRunner& operator=(SenderRunner&& other) {
        config_parser_ = other.config_parser_;
        zed = std::move(other.zed);
        init_params = other.init_params;
        runner = std::move(other.runner);
        running = other.running;

        other.config_parser_ = nullptr;
        other.running = false;

        return *this;
    }

    bool open(sl::InputType, sl::BODY_FORMAT body_format);
    void start();
    void stop();

private:
    std::shared_ptr<ConfigParser> config_parser_;
    sl::Camera zed;
    sl::InitParameters init_params;
    void work();
    std::thread runner;
    bool running;
};

#endif // ! __SENDER_RUNNER_HDR__
