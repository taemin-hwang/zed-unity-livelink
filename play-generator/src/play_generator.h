#ifndef _PLAY_GENERATOR_H_
#define _PLAY_GENERATOR_H_

#include <memory>

#include "config/config_parser.h"
#include "generator/generator.h"
#include "transfer/PracticalSocket.h"
#include "transfer/json.h"

class PlayGenerator {
public:
    PlayGenerator();
    virtual ~PlayGenerator() = default;

    void init();
    void run();
    void shutdown();

private:
    std::shared_ptr<ConfigParser> config_parser_;
    std::unique_ptr<Generator> generator_;
};

#endif // _PLAY_GENERATOR_H_