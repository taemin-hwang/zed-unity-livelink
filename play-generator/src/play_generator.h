#ifndef _PLAY_GENERATOR_H_
#define _PLAY_GENERATOR_H_

#include <memory>

#include "config/config_parser.h"

class PlayGenerator {
public:
    PlayGenerator();
    virtual ~PlayGenerator() = default;

private:
    std::unique_ptr<ConfigParser> config_parser_;
};

#endif // _PLAY_GENERATOR_H_