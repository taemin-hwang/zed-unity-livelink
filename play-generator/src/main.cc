#include <iostream>
#include <memory>

#include "play_generator.h"

int main() {
    std::unique_ptr<PlayGenerator> play_generator = std::make_unique<PlayGenerator>();

    play_generator->init();
    play_generator->run();
    play_generator->shutdown();
}