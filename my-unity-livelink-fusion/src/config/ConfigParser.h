#ifndef _CONFIG_PARSER_H_
#define _CONFIG_PARSER_H_

#include <string>

// include rapidjson
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"

class ConfigParser {
public:
    ConfigParser(std::string path);
    virtual ~ConfigParser() = default;

    inline std::string GetAddress() { std::cout << "[IP ADDR   ] " << addr_ << std::endl; return addr_; }
    inline int GetPort() { std::cout << "[RORT NUM  ] " << port_ << std::endl;  return port_; }
    inline bool IsViewerOn() { std::cout << "[VIEWER    ] " << is_enable_viewer_ << std::endl;
                                return is_enable_viewer_ == "on" || is_enable_viewer_ == "ON" || is_enable_viewer_ == "On"; }
    inline std::string GetResolution() { std::cout << "[RESOLUTION] " << resolution_ << std::endl; return resolution_; }
    inline int GetFPS() { std::cout << "[TARGET FPS] " << fps_ << std::endl; return fps_; }
    inline int GetAccuracy() { std::cout << "[ACCURACY  ] " << accuracy_ << std::endl; return accuracy_; }

private:
    std::string addr_ = "";
    int port_ = 0;
    std::string is_enable_viewer_ = "off";
    std::string resolution_ = "HD720";
    int fps_ = 30;
    int accuracy_ = 2;
};

#endif