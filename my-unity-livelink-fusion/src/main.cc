#include "LiveLinker.h"
#include <memory>
#include <iostream>

int main(int argc, char **argv) {
    std::unique_ptr<LiveLinker> live_linker = std::make_unique<LiveLinker>();
    live_linker->Initialize(argc, argv);
    if (live_linker->Start() < 0) {
        std::cout << "LiveLinker::Start() failed." << std::endl;
        return 1;
    }
    live_linker->Stop();

    return 0;
}

