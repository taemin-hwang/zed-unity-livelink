#include "play_generator.h"

PlayGenerator::PlayGenerator() {
    config_parser_ = std::make_shared<ConfigParser>("../etc/config.json");
    generator_ = std::make_unique<Generator>(config_parser_);
}

void PlayGenerator::init() {

}

void PlayGenerator::run() {
    generator_->generate();
}

void PlayGenerator::shutdown() {

}