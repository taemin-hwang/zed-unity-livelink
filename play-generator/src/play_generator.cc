#include "play_generator.h"

#include <chrono>
#include <thread>

PlayGenerator::PlayGenerator() {
    config_parser_ = std::make_shared<ConfigParser>("../etc/config.json");
    generator_ = std::make_unique<Generator>(config_parser_);
}

void PlayGenerator::init() {

}

void PlayGenerator::run() {
    int frame_num = 0;

    std::string ip_addr = config_parser_->get_ip_addr();
    int port = config_parser_->get_port();
    int delay = config_parser_->get_delay();

    UDPSocket sock;

    while(frame_num < 2500) {
        auto generated_packet = generator_->generate(frame_num);
        sock.sendTo(generated_packet.data(), generated_packet.size(), ip_addr.c_str(), port);

        frame_num++;
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }
}

void PlayGenerator::shutdown() {

}