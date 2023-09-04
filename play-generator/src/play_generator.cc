#include "play_generator.h"

PlayGenerator::PlayGenerator() {
    config_parser_ = std::make_unique<ConfigParser>("../etc/config.json");
}